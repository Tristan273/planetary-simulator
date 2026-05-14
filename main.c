#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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
    TTF_Init();

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

    // To write something inside of the window
    TTF_Font *font = TTF_OpenFont("arial.ttf", 18);


    bool running = true;
    SDL_Event event;

    int paused = 0;
    int backwards = 0;

    int frame = 0; // count the frames in the simulation to make something happen every X frames

    int speed = 1;

    double zoom = 1.0;
    double cam_x = WINDOW_WIDTH/2;
    double cam_y = WINDOW_HEIGHT/2;

    // To drag the mouse
    int dragging = 0;
    int last_mouse_x = 0;   
    int last_mouse_y = 0;

    int followed_body = -1; // -1 when no body is followed, 0,...,N-1 being the id of the eventually followed body


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
                else if (event.key.keysym.sym == SDLK_UP) {
                    zoom *= 1.1;   // zoom in
                }
                else if (event.key.keysym.sym == SDLK_DOWN) {
                    zoom /= 1.1;   // zoom out
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {

                if(event.button.button == SDL_BUTTON_LEFT) {
                    dragging = 1;

                    last_mouse_x = event.button.x;
                    last_mouse_y = event.button.y;

                    int mx = event.button.x;
                    int my = event.button.y;

                    int w, h;
                    SDL_GetWindowSize(window, &w, &h);

                    double world_x = (mx - w / 2) / zoom + cam_x;
                    double world_y = (my - h / 2) / zoom + cam_y;

                    followed_body = -1; // resets the followed body every time there is a left click

                    // Test if bodies are getting clicked on 
                    for(int i = 0; i < N; i++) {
                        if(bodies[i].type == 0) continue;

                        double dx = world_x - bodies[i].position.x;
                        double dy = world_y - bodies[i].position.y;
                        double dist2 = dx*dx + dy*dy;

                        if(dist2 <= bodies[i].radius * bodies[i].radius) {
                            followed_body = i;
                            break;
                        }
                    }
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {

                if(event.button.button == SDL_BUTTON_LEFT) {
                    dragging = 0;
                }
            }
            else if (event.type == SDL_MOUSEMOTION) {

                if(dragging) {
                    int dx = event.motion.x - last_mouse_x;
                    int dy = event.motion.y - last_mouse_y;

                    cam_x -= dx / zoom;
                    cam_y -= dy / zoom;

                    last_mouse_x = event.motion.x;
                    last_mouse_y = event.motion.y;
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

        // Removing the objects that are too far from the simulation
        for(int i=0; i < N; i++){
            if(bodies[i].position.x > 10000000 || bodies[i].position.y > 10000000 || bodies[i].position.x < -10000000 || bodies[i].position.y < -10000000){
                bodies[i].type = 0;
            }
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

            // set the camera to the followed body
            if(followed_body != -1 && bodies[followed_body].type != 0) {
                    cam_x = bodies[followed_body].position.x;
                    cam_y = bodies[followed_body].position.y;
            }
            render_scene(renderer, window, bodies, N, zoom, cam_x, cam_y, followed_body, font); // renders only after updating the body 'speed' times 

        } else if (paused == 1) { // the simulation is paused
                // set the camera to the followed body
                if(followed_body != -1 && bodies[followed_body].type != 0) {
                        cam_x = bodies[followed_body].position.x;
                        cam_y = bodies[followed_body].position.y;
                }
                render_scene(renderer, window, bodies, N, zoom, cam_x, cam_y, followed_body, font);
            }

        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_Quit();

    free(bodies);
    free(initial_bodies);
    return 0;
}