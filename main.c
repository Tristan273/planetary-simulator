#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "physics.h"
#include "leapfrog.h"
#include "rendering.h"


typedef enum {
    MODE_SIMULATION,
    MODE_CREATE_INPUT
} Mode;



int main() {
    int N;
    if(fscanf(stdin, "%d", &N)==0){
        return 1;
    }

    int capacity = N;
    struct body* bodies = malloc(capacity * sizeof(struct body));

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
    SDL_StartTextInput();

    SDL_Window *window = SDL_CreateWindow(
        "N-body simulation",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
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


    int creating_body = 0; // to check if we are currently creating a body

    double create_start_x, create_start_y;
    double create_current_x, create_current_y;


    // to input informations of the nwely added body
    Mode mode = MODE_SIMULATION;
    char input_mass[64] = "";
    char input_radius[64] = "";

    int active_field = 0; // 0=mass 1=radius

    while (running) {

        // ADDING BUTTONS
        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        // Button size
        int btn_w = w * 0.08;
        int btn_h = h * 0.05;
        int y = h - btn_h - 10; // for the buttons to be at the bottom of the window even when resizing the window

        // Pause button
        SDL_Rect btn_pause = {w * 0.05, y, btn_w, btn_h};

        // Slow button
        SDL_Rect btn_slow = {w * 0.05 + btn_w + 10, y, btn_w, btn_h};

        // Fast button
        SDL_Rect btn_fast = {w * 0.05 + 2*(btn_w + 10), y, btn_w, btn_h};

        // Reset button
        SDL_Rect btn_reset = {w * 0.05 + 3*(btn_w + 10), y, btn_w, btn_h};



        while (SDL_PollEvent(&event)) {
            if (mode == MODE_CREATE_INPUT) {

                if (event.type == SDL_QUIT){
                    running = false;
                }

                else if (event.type == SDL_TEXTINPUT) {
                    if (active_field == 0)
                        strcat(input_mass, event.text.text);
                    else if (active_field == 1)
                        strcat(input_radius, event.text.text);
                }

                else if (event.type == SDL_KEYDOWN) {

                    if (event.key.keysym.sym == SDLK_TAB) {
                        active_field = (active_field + 1) % 2;
                    }

                    else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        mode = MODE_SIMULATION;
                        input_mass[0] = '\0';
                        input_radius[0] = '\0';
                    }

                    else if (event.key.keysym.sym == SDLK_BACKSPACE) {
                        if (active_field == 0){
                            input_mass[0] = '\0';
                        }
                        else if (active_field == 1){
                            input_radius[0] = '\0';
                        } 
                    }

                    if (event.key.keysym.sym == SDLK_RETURN ) {

                        mode = MODE_SIMULATION;

                        double mass = atof(input_mass);
                        double radius = atof(input_radius);


                        if (N >= capacity) {
                            capacity *= 2;
                            bodies = realloc(bodies, capacity * sizeof(struct body));
                        }

                        struct body new_body;

                        new_body.id = N;
                        new_body.mass = mass;
                        new_body.radius = radius;

                        new_body.position.x = create_start_x;
                        new_body.position.y = create_start_y;

                        new_body.velocity.x = (create_current_x - create_start_x) * 0.5;
                        new_body.velocity.y = (create_current_y - create_start_y) * 0.5;

                        new_body.type = 1;

                        new_body.color_r = 150 + rand() % 106;
                        new_body.color_g = 150 + rand() % 106;
                        new_body.color_b = 150 + rand() % 106;

                        bodies[N] = new_body;
                        trail_clear(&trails[N]);

                        N++;

                        // reset input buffers
                        input_mass[0] = 0;
                        input_radius[0] = 0;
                    }
                }

                render_input_screen(renderer);


                SDL_Texture* tex_mass_label = make_text(renderer, font, "Mass:");
                SDL_Texture* tex_radius_label = make_text(renderer, font, "Radius:");

                SDL_Rect r_mass_label = {20, 50, 0, 0};
                SDL_Rect r_radius_label = {20, 100, 0, 0};

                SDL_QueryTexture(tex_mass_label, NULL, NULL, &r_mass_label.w, &r_mass_label.h);
                SDL_QueryTexture(tex_radius_label, NULL, NULL, &r_radius_label.w, &r_radius_label.h);


                SDL_Rect r_mass = {120, 50, 0, 0};
                SDL_Rect r_radius = {120, 100, 0, 0};

                SDL_Texture* tex_mass = make_text(renderer, font, input_mass);
                SDL_QueryTexture(tex_mass, NULL, NULL, &r_mass.w, &r_mass.h);
                SDL_Texture* tex_radius = make_text(renderer, font, input_radius);
                SDL_QueryTexture(tex_radius, NULL, NULL, &r_radius.w, &r_radius.h);

                // Highlight the mass
                if (active_field == 0) {
                    SDL_Rect box = {r_mass.x - 5, r_mass.y - 5, r_mass.w + 10, r_mass.h + 10};
                    SDL_SetRenderDrawColor(renderer, 80, 80, 200, 255);
                    SDL_RenderDrawRect(renderer, &box);
                }

                // Highlight the radius 
                if (active_field == 1) {
                    SDL_Rect box = {r_radius.x - 5, r_radius.y - 5, r_radius.w + 10, r_radius.h + 10};
                    SDL_SetRenderDrawColor(renderer, 80, 80, 200, 255);
                    SDL_RenderDrawRect(renderer, &box);
                }

                // Render the texts
                SDL_RenderCopy(renderer, tex_mass_label, NULL, &r_mass_label);
                SDL_RenderCopy(renderer, tex_radius_label, NULL, &r_radius_label);
                SDL_RenderCopy(renderer, tex_mass, NULL, &r_mass);
                SDL_RenderCopy(renderer, tex_radius, NULL, &r_radius);

                SDL_RenderPresent(renderer);

                SDL_DestroyTexture(tex_mass);
                SDL_DestroyTexture(tex_radius);

                continue;
            }


            if (event.type == SDL_QUIT){
                running = false;
            }

            else if (event.type == SDL_MOUSEWHEEL) {
                int mx, my;
                SDL_GetMouseState(&mx, &my);


                // Position monde sous le curseur avant le zoom
                double world_x = (mx - w / 2.0) / zoom + cam_x;
                double world_y = (my - h / 2.0) / zoom + cam_y;


                if (event.wheel.y > 0) {
                    zoom *= 1.1;  // zoom in
                } else if (event.wheel.y < 0) {
                    zoom /= 1.1;  // zoom out
                }


                // Ajuster la caméra pour zoomer sur le curseur
                cam_x = world_x - (mx - w / 2.0) / zoom;
                cam_y = world_y - (my - h / 2.0) / zoom;
            }

            else if (event.type == SDL_KEYDOWN) {
                
                if (event.key.keysym.sym == SDLK_p) { // checks if the pressed key is P
                    paused = 1 - paused; // inverts the value of paused
                } 

                else if (event.key.keysym.sym == SDLK_DELETE) {
                    if (followed_body != -1) {
                        bodies[followed_body].type = 0;
                        trail_clear(&trails[followed_body]);
                        followed_body = -1;
                    }
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

                int mx = event.button.x;
                int my = event.button.y;

                double world_x = (mx - w / 2) / zoom + cam_x;
                double world_y = (my - h / 2) / zoom + cam_y;

                if(event.button.button == SDL_BUTTON_LEFT) {
                    dragging = 1;

                    last_mouse_x = event.button.x;
                    last_mouse_y = event.button.y;

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

                    // Test if buttons are getting cliked on

                    if (mx >= btn_pause.x && mx <= btn_pause.x + btn_pause.w && my >= btn_pause.y && my <= btn_pause.y + btn_pause.h){
                    paused = 1 - paused;
                    }
                    else if (mx >= btn_slow.x && mx <= btn_slow.x + btn_slow.w && my >= btn_slow.y && my <= btn_slow.y + btn_slow.h){
                        if(speed > 1){
                            speed /= 2; // slow the simulation down
                        }
                    }
                    else if (mx >= btn_fast.x && mx <= btn_fast.x + btn_fast.w && my >= btn_fast.y && my <= btn_fast.y + btn_fast.h){
                        speed *= 2; // runs the simulation faster
                    }
                    else if (mx >= btn_reset.x && mx <= btn_reset.x + btn_reset.w && my >= btn_reset.y && my <= btn_reset.y + btn_reset.h){
                        for(int i=0; i<N; i++){
                            bodies[i] = initial_bodies[i]; // resets each body to its inital value
                            trail_clear(&trails[i]);
                        }
                    }
                }
                if (event.button.button == SDL_BUTTON_RIGHT){

                    if (N < MAX_BODIES){ // Does nothing if N is greater than the maximum amount of bodies allowed 

                        creating_body = 1;

                        create_start_x = world_x;
                        create_start_y = world_y;

                        create_current_x = world_x;
                        create_current_y = world_y;

                    }
                    else {
                        printf("Maximum amount of bodies reached.");
                    }
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {

                if(event.button.button == SDL_BUTTON_LEFT) {
                    dragging = 0;
                }
                if (event.button.button == SDL_BUTTON_RIGHT && creating_body){
                    creating_body = 0;
                    mode = MODE_CREATE_INPUT;
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
                if (creating_body){
                    int mx = event.motion.x;
                    int my = event.motion.y;

                    create_current_x = (mx - w / 2) / zoom + cam_x;
                    create_current_y = (my - h / 2) / zoom + cam_y;
                }
            }
        }

        frame++; 

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

        if(mode == MODE_SIMULATION){

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
                render_scene(renderer, window, bodies, N, zoom, cam_x, cam_y, followed_body, font, btn_pause, btn_slow, btn_fast, btn_reset); // renders only after updating the body 'speed' times 

            } else if (paused == 1) { // the simulation is paused
                    // set the camera to the followed body
                    if(followed_body != -1 && bodies[followed_body].type != 0) {
                            cam_x = bodies[followed_body].position.x;
                            cam_y = bodies[followed_body].position.y;
                    }
                    render_scene(renderer, window, bodies, N, zoom, cam_x, cam_y, followed_body, font, btn_pause, btn_slow, btn_fast, btn_reset);
            }
            
            if (creating_body) {
               
                int sx = (int)((create_start_x - cam_x) * zoom + w / 2);
                int sy = (int)((create_start_y - cam_y) * zoom + h / 2);
                int ex = (int)((create_current_x - cam_x) * zoom + w / 2);
                int ey = (int)((create_current_y - cam_y) * zoom + h / 2);

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                draw_arrow(renderer, sx, sy, ex, ey);

                double vx = (create_current_x - create_start_x) * 0.5;
                double vy = (create_current_y - create_start_y) * 0.5;
                double velocity_norm = sqrt(vx*vx + vy*vy);

                char velocity_buf[64];
                sprintf(velocity_buf, "Velocity : %.2f", velocity_norm);
                draw_text(renderer, font, velocity_buf, ex + 10, ey - 20);


                SDL_RenderPresent(renderer);
            }
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