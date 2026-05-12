#include <SDL2/SDL.h>
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

void render_trail(SDL_Renderer *renderer, SDL_Window *window, struct body *body, Trail *t, double zoom) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    if (body->type == 0 || t->count < 2) return;
    SDL_SetRenderDrawColor(renderer, body->color_r, body->color_g, body->color_b, 160);
    int prev = (t->head - t->count + TRAIL_LEN) % TRAIL_LEN;
    for (int k = 1; k < t->count; k++) {
        int cur = (prev + 1) % TRAIL_LEN;
        SDL_RenderDrawLine(renderer, (int)(t->x[prev] * zoom + w/2 * (1 - zoom)), (int)(t->y[prev] * zoom + h/2 * (1 - zoom)), (int)(t->x[cur] * zoom + w/2 * (1 - zoom)), (int)(t->y[cur] * zoom + h/2 * (1 - zoom)));
        prev = cur;
    }
}

void draw_filled_circle(SDL_Renderer *renderer, int cx, int cy, int r) {
    for (int dy = -r; dy <= r; dy++) {
        int dx = (int)sqrt(r*r - dy*dy);


        SDL_RenderDrawLine(renderer, cx - dx, cy + dy,cx + dx, cy + dy);
    }
}


void render_body(SDL_Renderer *renderer, SDL_Window *window, struct body body, double zoom){
    if(body.type != 0){
        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        // choice of color
        SDL_SetRenderDrawColor(renderer, body.color_r, body.color_g, body.color_b, 255);

        draw_filled_circle(renderer, (int)(body.position.x * zoom + w/2 * (1 - zoom)), (int)(body.position.y * zoom + h/2 * (1 - zoom)), (int)(body.radius * zoom));
    }
}


void render_scene(SDL_Renderer *renderer, SDL_Window *window, struct body *bodies, int N, double zoom) {
    /* Clear the screen with solid black each frame. */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    /* Draw trails behind the bodies. */
    for (int i = 0; i < N; i++)
        render_trail(renderer, window, &bodies[i], &trails[i], zoom);

    /* Draw the bodies on top. */
    for (int i = 0; i < N; i++)
        render_body(renderer, window, bodies[i], zoom);

    SDL_RenderPresent(renderer);
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