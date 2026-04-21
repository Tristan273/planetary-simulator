#include <SDL2/SDL.h>
#include <stdbool.h>
#include "physics.h"

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

int main() {
    struct body earth = {1, 10, 10, {200.0, 200.0}, {0.0, 0.0}, 1};
    struct body sun = {2, 20, 20, {400.0, 200.0}, {0.0, 0.0}, 1};

    struct body bodies[] = {earth, sun};
    int N = 2;

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

        render_scene(renderer, bodies, N);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}