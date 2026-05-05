#include <SDL2/SDL.h>
#include <stdbool.h>
#include "physics.h"
#include "leapfrog.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

void draw_filled_circle(SDL_Renderer *renderer, int cx, int cy, int r) {
    for (int dy = -r; dy <= r; dy++) {
        int dx = (int)sqrt(r*r - dy*dy);


        SDL_RenderDrawLine(renderer, cx - dx, cy + dy,cx + dx, cy + dy);
    }
}


void render_body(SDL_Renderer *renderer, struct body body){
    if(body.type != 0){


        // choice of color
        SDL_SetRenderDrawColor(renderer, 180, 180, 255, 255);


        draw_filled_circle(renderer, (int)body.position.x, (int)body.position.y, (int)body.radius);
    }
}


void render_scene(SDL_Renderer *renderer, struct body *bodies, int N, int fading) {
    if(fading == 1){
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 10);
        SDL_Rect fade = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(renderer, &fade);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }


    for (int i = 0; i < N; i++) {
        render_body(renderer, bodies[i]);
    }


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


int main() {
    int N;
    if(fscanf(stdin, "%d", &N)==0){
        return 1;
    }

    struct body* bodies = malloc(N*sizeof(struct body));

    for(int i=0; i<N; i++){
        if((fscanf(stdin, "%d", &bodies[i].id)==0) || 
        (fscanf(stdin, "%lf", &bodies[i].mass)==0) || 
        (fscanf(stdin, "%lf", &bodies[i].radius)==0) || 
        (fscanf(stdin, "%lf", &bodies[i].position.x)==0) || 
        (fscanf(stdin, "%lf", &bodies[i].position.y)==0) || 
        (fscanf(stdin, "%lf", &bodies[i].velocity.x)==0) || 
        (fscanf(stdin, "%lf", &bodies[i].velocity.y)==0) ||
        (fscanf(stdin, "%d", &bodies[i].type)==0)){
            return 1;
        }
    }
    
    // In order to be able to reset the simulation : create a copy of the initial value of bodies
    struct body* initial_bodies = malloc(N*sizeof(struct body));
    for(int i=0; i<N; i++){
        initial_bodies[i].id = bodies[i].id;
        initial_bodies[i].mass = bodies[i].mass;
        initial_bodies[i].radius = bodies[i].radius;
        initial_bodies[i].position.x = bodies[i].position.x;
        initial_bodies[i].position.y = bodies[i].position.y;
        initial_bodies[i].velocity.x = bodies[i].velocity.x;
        initial_bodies[i].velocity.y = bodies[i].velocity.y;
        initial_bodies[i].type = bodies[i].type;
    }

    double dt = 0.05;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "N-body simulation",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);


    bool running = true;
    SDL_Event event;

    int paused = 0;
    int backwards = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT){
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_p) { // checks if the pressed key is P
                    paused = 1 - paused; // inverts the value of paused
                } 
                else if (event.key.keysym.sym == SDLK_r){ // checks if the pressed key is R
                    for(int i=0; i<N; i++){
                        bodies[i] = initial_bodies[i]; // resets each body to its inital value
                    }
                }

                else if (event.key.keysym.sym == SDLK_LEFT) { // checks if the pressed key is the left arrow
                    backwards = 1; // the simulation has to run back
                } else if (event.key.keysym.sym == SDLK_RIGHT) { // checks if the pressed key is the right arrow
                    backwards = 0; // the simulation will have to run forwards
                }
            }
        }
        

        if(paused == 0 && backwards == 0){ // the simulation is running forwards

            leapfrog_step(bodies, N, dt); // modifies the position and velocity of the bodies
            render_scene(renderer, bodies, N, 1);

        } else if(paused == 0 && backwards == 1){ // the simulation is running backwards

            leapfrog_step(bodies, N, -dt); // modifies the position and velocity of the bodies
            render_scene(renderer, bodies, N, 1);

        } else if (paused == 1) { // the simulation is paused

            render_scene(renderer, bodies, N, 1);
        }

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free(bodies);
    free(initial_bodies);
    return 0;
}