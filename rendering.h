#ifndef RENDERING_H
#define RENDERING_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "physics.h"
#include "leapfrog.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define TRAIL_LEN  300
#define MAX_BODIES 256

typedef struct {
    double x[TRAIL_LEN];
    double y[TRAIL_LEN];
    int head;
    int count;
} Trail;

extern Trail trails[MAX_BODIES];

void trail_push(Trail *t, double x, double y);

void trail_clear(Trail *t);

void render_trail(SDL_Renderer *renderer, SDL_Window *window, struct body *body, Trail *t, double zoom);

void draw_filled_circle(SDL_Renderer *renderer, int cx, int cy, int r);

void render_body(SDL_Renderer *renderer, SDL_Window *window, struct body body, double zoom);

void render_scene(SDL_Renderer *renderer, SDL_Window *window, struct body *bodies, int N, double zoom);

void update_body(struct body *body, double dt);

void update_bodies(struct body *bodies, double dt, int N);

#endif