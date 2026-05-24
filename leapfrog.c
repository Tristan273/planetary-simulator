#include "leapfrog.h"
/* Updates the positions and velocities of N bodies over a timestep dt using the leapfrog 
integration method */
void leapfrog_step(struct body *bodies, int N, double dt) {
    struct vector acc[N];

    // Compute the accceleration a(t) for each body
    for (int i = 0; i < N; i++) {
        struct vector force = simulate_grav_forceN(bodies[i], bodies, N);
        acc[i].x = force.x / bodies[i].mass;
        acc[i].y = force.y / bodies[i].mass;
    }

    // Velocity Half step : v(t + dt/2)
    for (int i = 0; i < N; i++) {
        bodies[i].velocity.x += acc[i].x * dt / 2.0;
        bodies[i].velocity.y += acc[i].y * dt / 2.0;
    }

    //  Position step : x(t + dt)
    for (int i = 0; i < N; i++) {
        bodies[i].position.x += bodies[i].velocity.x * dt;
        bodies[i].position.y += bodies[i].velocity.y * dt;
    }

    // Recompute a(t + dt)
    for (int i = 0; i < N; i++) {
        struct vector force = simulate_grav_forceN(bodies[i], bodies, N);
        acc[i].x = force.x / bodies[i].mass;
        acc[i].y = force.y / bodies[i].mass;
    }

    // Second half velocity step : v(t + dt)
    for (int i = 0; i < N; i++) {
        bodies[i].velocity.x += acc[i].x * dt / 2.0;
        bodies[i].velocity.y += acc[i].y * dt / 2.0;
    }
}