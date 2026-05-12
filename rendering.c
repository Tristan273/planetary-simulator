#include <SDL2/SDL.h>
#include <stdbool.h>
#include "physics.h"
#include "leapfrog.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

/* ── Trails ─────────────────────────────────────────────────────────────── */
#define TRAIL_LEN  300   /* number of past positions kept per body */
#define MAX_BODIES 256   /* maximum number of bodies supported */
/*
 * Structure representing the trail of a body.
 * We use a circular buffer: instead of a ever-growing array,
 * we write in a loop into a fixed-size array of length TRAIL_LEN.
 * The oldest positions are automatically overwritten by new ones.
 */
typedef struct {
    double x[TRAIL_LEN];  /* X coordinates of the last TRAIL_LEN positions */
    double y[TRAIL_LEN];  /* Y coordinates of the last TRAIL_LEN positions */
    int    head;          /* index of the next slot to write (0 to TRAIL_LEN-1) */
    int    count;         /* number of currently stored positions               */
                          /* equals TRAIL_LEN once the buffer is full           */
} Trail;


/* One trail per body, indexed the same way as the bodies[] array */
static Trail trails[MAX_BODIES];

//Appends position (x, y) to trail t
static void trail_push(Trail *t, double x, double y) {
    t->x[t->head] = x;
    t->y[t->head] = y;
    t->head = (t->head + 1) % TRAIL_LEN;
    if (t->count < TRAIL_LEN) t->count++;
}

//Clears the current trail t
static void trail_clear(Trail *t) {
    t->head  = 0;
    t->count = 0;
}

static void render_trail(SDL_Renderer *renderer, struct body *body, Trail *t) {
    if (body->type == 0 || t->count < 2) return;
    SDL_SetRenderDrawColor(renderer, body->color_r, body->color_g, body->color_b, 160);
    int prev = (t->head - t->count + TRAIL_LEN) % TRAIL_LEN;
    for (int k = 1; k < t->count; k++) {
        int cur = (prev + 1) % TRAIL_LEN;
        SDL_RenderDrawLine(renderer,
                           (int)t->x[prev], (int)t->y[prev],
                           (int)t->x[cur],  (int)t->y[cur]);
        prev = cur;
    }
}

void draw_filled_circle(SDL_Renderer *renderer, int cx, int cy, int r) {
    for (int dy = -r; dy <= r; dy++) {
        int dx = (int)sqrt(r*r - dy*dy);


        SDL_RenderDrawLine(renderer, cx - dx, cy + dy,cx + dx, cy + dy);
    }
}


void render_body(SDL_Renderer *renderer, struct body body){
    if(body.type != 0){


        // choice of color
        SDL_SetRenderDrawColor(renderer, body.color_r, body.color_g, body.color_b, 255);


        draw_filled_circle(renderer, (int)body.position.x, (int)body.position.y, (int)body.radius);
    }
}


void render_scene(SDL_Renderer *renderer, struct body *bodies, int N) {

    /* Clear the screen with solid black each frame. */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    /* Draw trails behind the bodies. */
    for (int i = 0; i < N; i++)
        render_trail(renderer, &bodies[i], &trails[i]);

    /* Draw the bodies on top. */
    for (int i = 0; i < N; i++)
        render_body(renderer, bodies[i]);

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
        (fscanf(stdin, "%d", &bodies[i].type)==0) ||
        (fscanf(stdin, "%d", &bodies[i].color_r)==0) ||
        (fscanf(stdin, "%d", &bodies[i].color_g)==0) ||
        (fscanf(stdin, "%d", &bodies[i].color_b)==0)){
            return 1;
        }
        trail_clear(&trails[i]);
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
        initial_bodies[i].color_r = bodies[i].color_r;
        initial_bodies[i].color_g = bodies[i].color_g;
        initial_bodies[i].color_b = bodies[i].color_b;
    }

    double dt = 0.001;

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

    int frame = 0; // count the frames in the simulation to make something happen every X frames

    int speed = 1;

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
                else if (event.key.keysym.sym == SDLK_s) { // checks if the pressed key is S
                    if(speed > 1){
                        speed /= 2; // slow the simulation down
                    }
                }
                else if (event.key.keysym.sym == SDLK_f) { // checks if the pressed key is F
                    speed *= 2; // runs the simulation faster
                }
            }
        }

        frame++; 

        // Display the enrgies every X seconds
        if(frame % 60 == 0){ // the following things happen every 60 frames
            for(int i = 0; i < N; i++){
                if(bodies[i].type == 0) continue;


                double Ek = calculate_kinetic_energy(bodies[i]);
                double Ep = calculate_potential_energy(bodies[i], bodies, N);


                printf("Body %d: Ek = %f | Ep = %f | E = %f\n", bodies[i].id, Ek, Ep, Ek + Ep);
            }
            printf("\n");
        } 

        // Test the collisions
        for(int i = 0; i < N; i++){
            if (bodies[i].type == 0) continue;


            for(int j = i + 1; j < N; j++){
                if (bodies[j].type == 0) continue;


                if (are_colliding(bodies[i], bodies[j])) {


                    bodies[i] = merge_bodies(bodies[i], bodies[j]);
                    bodies[j].type = 0;


                    break;
                }
            }
        }
        for(int i = 0; i < N; i++){                                              /* <- ajouter */
            if(bodies[i].type != 0)                                              /* <- ajouter */
                trail_push(&trails[i], bodies[i].position.x, bodies[i].position.y); /* <- ajouter */
        }   
        if(paused == 0){
            for(int k = 0; k < speed; k++){
                if(backwards == 0){ // the simulation is running forwards

                    leapfrog_step(bodies, N, dt); // modifies the position and velocity of the bodies

                } else if(paused == 0 && backwards == 1){ // the simulation is running backwards

                    leapfrog_step(bodies, N, -dt); // modifies the position and velocity of the bodies

                } 
            }
            render_scene(renderer, bodies, N); 

        } else if (paused == 1) { // the simulation is paused

                render_scene(renderer, bodies, N);
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