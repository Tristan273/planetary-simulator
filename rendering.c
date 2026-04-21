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
    struct body earth = {1, 10000, 10, {600, 400}, {0.0, 0.0}, 1};
    struct body moon = {2, 1, 3, {750.0, 400.0}, {0.0, 40}, 1};

    struct body bodies[] = {earth, moon};

    int N = 2;
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
    return 0;
}