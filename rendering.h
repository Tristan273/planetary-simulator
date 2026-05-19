#ifndef RENDERING_H
#define RENDERING_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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

void render_trail(SDL_Renderer *renderer, SDL_Window *window, struct body *body, Trail *t, double zoom, double cam_x, double cam_y);

void draw_filled_circle(SDL_Renderer *renderer, int cx, int cy, int r);

void render_body(SDL_Renderer *renderer, SDL_Window *window, struct body body, double zoom, double cam_x, double cam_y);

void render_scene(SDL_Renderer *renderer, SDL_Window *window, struct body *bodies, int N, double zoom, double cam_x, double cam_y, int selected_body, TTF_Font *font, SDL_Rect btn_pause, SDL_Rect btn_slow, SDL_Rect btn_fast, SDL_Rect btn_reset, SDL_Rect btn_backwards, SDL_Rect btn_vectors);

void render_input_screen(SDL_Renderer* r);

SDL_Texture* make_text(SDL_Renderer* renderer, TTF_Font* font, const char* text);

void draw_arrow(SDL_Renderer *renderer, int x1, int y1, int x2, int y2);

void draw_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y);

void update_body(struct body *body, double dt);

void update_bodies(struct body *bodies, double dt, int N);

#endif