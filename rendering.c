#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <math.h>
#include "physics.h"
#include "leapfrog.h"
#include "rendering.h"

/* One trail per body, indexed the same way as the bodies[] array */
Trail trails[MAX_BODIES];

//Appends position (x, y) to trail t
void trail_push(Trail *t, double x, double y) {
    t->x[t->head] = x;
    t->y[t->head] = y;
    t->head = (t->head + 1) % TRAIL_LEN;
    if (t->count < TRAIL_LEN) t->count++;
}

//Clears the current trail t
void trail_clear(Trail *t) {
    t->head  = 0;
    t->count = 0;
}

void render_trail(SDL_Renderer *renderer, SDL_Window *window, struct body *body, Trail *t, double zoom, double cam_x, double cam_y) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    if (body->type == 0 || t->count < 2) return;
    SDL_SetRenderDrawColor(renderer, body->color_r, body->color_g, body->color_b, 160);
    int prev = (t->head - t->count + TRAIL_LEN) % TRAIL_LEN;
    for (int k = 1; k < t->count; k++) {
        int cur = (prev + 1) % TRAIL_LEN;
        SDL_RenderDrawLine(renderer, (int)((t->x[prev] - cam_x) * zoom + w/2), (int)((t->y[prev] - cam_y) * zoom + h/2), (int)((t->x[cur] - cam_x) * zoom + w/2), (int)((t->y[cur] - cam_y) * zoom + h/2));
        prev = cur;
    }
}

void draw_filled_circle(SDL_Renderer *renderer, int cx, int cy, int r) {
    for (int dy = -r; dy <= r; dy++) {
        int dx = (int)sqrt(r*r - dy*dy);


        SDL_RenderDrawLine(renderer, cx - dx, cy + dy,cx + dx, cy + dy);
    }
}


void render_body(SDL_Renderer *renderer, SDL_Window *window, struct body body, double zoom, double cam_x, double cam_y){
    if(body.type != 0){
        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        // choice of color
        SDL_SetRenderDrawColor(renderer, body.color_r, body.color_g, body.color_b, 255);

        draw_filled_circle(renderer, (int)((body.position.x - cam_x) * zoom + w/2), (int)((body.position.y - cam_y) * zoom + h/2), (int)(body.radius * zoom));
    }
}


void draw_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y){
    SDL_Color color = {255,255,255,255};

    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect;

    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


void render_scene(SDL_Renderer *renderer, SDL_Window *window, struct body *bodies, int N, double zoom, double cam_x, double cam_y, int selected_body, TTF_Font *font, SDL_Rect btn_pause, SDL_Rect btn_slow, SDL_Rect btn_fast, SDL_Rect btn_reset) {
    /* Clear the screen with solid black each frame. */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    /* Draw trails behind the bodies. */
    for (int i = 0; i < N; i++)
        render_trail(renderer, window, &bodies[i], &trails[i], zoom, cam_x, cam_y);

    /* Draw the bodies on top. */
    for (int i = 0; i < N; i++)
        render_body(renderer, window, bodies[i], zoom, cam_x, cam_y);


    char buffer[256];

    // Render the text on the screen if a body is followed
    if(selected_body != -1 && bodies[selected_body].type != 0){
        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        struct body b = bodies[selected_body];

        sprintf(buffer, "Name %s", b.name);
        draw_text(renderer, font, buffer, w - 250, 20);

        sprintf(buffer, "Mass: %.2f", b.mass);
        draw_text(renderer, font, buffer, w - 250, 40);

        sprintf(buffer, "Radius: %.2f", b.radius);
        draw_text(renderer, font, buffer, w - 250, 60);

        sprintf(buffer, "Position: (%.2f, %.2f)", b.position.x, b.position.y);
        draw_text(renderer, font, buffer, w - 250, 80);

        sprintf(buffer, "Velocity: (%.2f, %.2f)", b.velocity.x, b.velocity.y);
        draw_text(renderer, font, buffer, w - 250, 100);

        sprintf(buffer, "Kinetic energy : %.2f", calculate_kinetic_energy(b));
        draw_text(renderer, font, buffer, w - 250, 120);

        sprintf(buffer, "Potential energy : %.2f", calculate_potential_energy(b, bodies, N));
        draw_text(renderer, font, buffer, w - 250, 140);

        sprintf(buffer, "Body Energy : %.2f", calculate_kinetic_energy(b) + calculate_potential_energy(b, bodies, N));
        draw_text(renderer, font, buffer, w - 250, 160);
    }

    sprintf(buffer, "Total system energy %.2f", calculate_total_energy(bodies, N));
    draw_text(renderer, font, buffer, 0, 20);


    // Render all the buttons
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &btn_pause);
    SDL_RenderFillRect(renderer, &btn_slow);
    SDL_RenderFillRect(renderer, &btn_fast);
    SDL_RenderFillRect(renderer, &btn_reset);

    // Render the button text
    draw_text(renderer, font, "Pause", btn_pause.x + 25, btn_pause.y + 10);
    draw_text(renderer, font, "Slow",  btn_slow.x + 25,  btn_slow.y + 10);
    draw_text(renderer, font, "Fast",  btn_fast.x + 25,  btn_fast.y + 10);
    draw_text(renderer, font, "Reset",  btn_reset.x + 25,  btn_reset.y + 10);
    

    SDL_RenderPresent(renderer);
}


void render_input_screen(SDL_Renderer* r){
    SDL_SetRenderDrawColor(r, 20, 20, 20, 255);
    SDL_RenderClear(r);

    // simple debug rectangles
    SDL_Rect box = {100, 100, 400, 40};
    SDL_RenderFillRect(r, &box);
}

SDL_Texture* make_text(SDL_Renderer* renderer, TTF_Font* font, const char* text) {
    SDL_Color white = {255, 255, 255, 255};

    SDL_Surface* surf = TTF_RenderText_Solid(font, text, white);
    if (!surf) return NULL;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    return tex;
}

void draw_arrow(SDL_Renderer *renderer, int x1, int y1, int x2, int y2) {
    // Ligne principale
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);

    // Calcul de la pointe
    double dx = x2 - x1;
    double dy = y2 - y1;
    double len = sqrt(dx*dx + dy*dy);
    if (len < 1) return;

    // Vecteur normalisé
    double nx = dx / len;
    double ny = dy / len;

    // Taille de la pointe
    int arrow_size = 10;

    // Les deux côtés de la pointe
    int ax = x2 - (int)(arrow_size * (nx - 0.5 * ny));
    int ay = y2 - (int)(arrow_size * (ny + 0.5 * nx));
    int bx = x2 - (int)(arrow_size * (nx + 0.5 * ny));
    int by = y2 - (int)(arrow_size * (ny - 0.5 * nx));

    SDL_RenderDrawLine(renderer, x2, y2, ax, ay);
    SDL_RenderDrawLine(renderer, x2, y2, bx, by);
}



void update_body(struct body *body, double dt){
    body->position.x += body->velocity.x * dt;
    body->position.y += body->velocity.y * dt;
}

void update_bodies(struct body *bodies, double dt, int N){
    for(int i=0; i<N; i++){
        update_body(&bodies[i], dt);
    }
}