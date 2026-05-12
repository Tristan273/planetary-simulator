#include <SDL2/SDL.h>
#include <stdbool.h>
#include "physics.h"
#include "leapfrog.h"
#include "rendering.h"


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
        SDL_WINDOW_FULLSCREEN_DESKTOP
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
                        trail_clear(&trails[i]);
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
                else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
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

        if(paused == 0){
            for(int k = 0; k < speed; k++){
                if(backwards == 0){ // the simulation is running forwards

                    leapfrog_step(bodies, N, dt); // modifies the position and velocity of the bodies

                } else if(paused == 0 && backwards == 1){ // the simulation is running backwards

                    leapfrog_step(bodies, N, -dt); // modifies the position and velocity of the bodies

                } 
            }
            for(int i = 0; i < N; i++){
                if(bodies[i].type != 0){
                trail_push(&trails[i], bodies[i].position.x, bodies[i].position.y);
                }
            }
            render_scene(renderer, bodies, N); // renders only after updating the body 'speed' times 

        } else if (paused == 1) { // the simulation is paused

                render_scene(renderer, bodies, N);
            }

        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free(bodies);
    free(initial_bodies);
    return 0;
}