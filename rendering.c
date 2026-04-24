#include <SDL2/SDL.h>
#include <stdbool.h>
#include "physics.h"
#include "leapfrog.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

void render_body(SDL_Renderer *renderer, struct body body){
    if(body.type != 0){
        SDL_Rect r = {
            body.position.x - body.radius,
            body.position.y - body.radius,
            body.radius * 2,
            body.radius * 2
        };

        SDL_RenderFillRect(renderer, &r);
    }
}

void render_scene(SDL_Renderer *renderer, struct body *bodies, int N) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

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


    double dt = 0.01;

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

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }


        leapfrog_step(bodies, N, dt);

        render_scene(renderer, bodies, N);


        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free(bodies);
    
    return 0;
}