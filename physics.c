#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "physics.h"

/* FUNCTIONS */

//returns the norm of a vector u
double norm(struct vector u){
    return sqrt(u.x*u.x + u.y*u.y);
}
//returns w the sum of two vectors u and v
struct vector add_2_vectors(struct vector u, struct vector v){
    struct vector w;

    w.x = u.x + v.x;
    w.y = u.y + v.y;

    return w;
}
//returns u, the sum of an array of vectors
struct vector add_N_vectors(struct vector *T, int N){
    struct vector u;
    u.x = 0;
    u.y = 0;

    for(int i = 0; i<N; i++){
        u = add_2_vectors(u, T[i]);
    }

    return u;
}

//returns w: the vector u-v
struct vector substract_2_vectors(struct vector u, struct vector v){
    struct vector w;


    w.x = u.x - v.x;
    w.y = u.y - v.y;


    return w;
}

/*returns the vector grav_force which is the gravitationnal force that
the first body applies on the second*/
struct vector simulate_grav_force2(struct body body1, struct body body2){
    // The force body1 applies on body2
    struct vector grav_force;
    struct vector OM;

    OM.x = body2.position.x - body1.position.x;
    OM.y = body2.position.y - body1.position.y;

    double d = norm(OM);
    double d3 = d * d * d;
    grav_force.x = -G * body1.mass * body2.mass * OM.x / d3;
    grav_force.y = -G * body1.mass * body2.mass * OM.y / d3;
    
    return grav_force;
}

/*returns the vector grav_force which is the gravitationnal force that
N bodies applie on the first one*/
struct vector simulate_grav_forceN(struct body body1, struct body *T, int N){
    struct vector grav_force = {0.0, 0.0};
    
    for(int i = 0; i<N; i++){
        if(T[i].id != body1.id && T[i].type != 0){
            grav_force = add_2_vectors(grav_force, simulate_grav_force2(T[i], body1));
        }
    }

    return grav_force;
}

/*Returns a body formed by merging two colliding bodies */
struct body merge_bodies(struct body body1, struct body body2){
    struct body sum;

    sum.id = body1.id;
    sum.mass = body1.mass + body2.mass;
    sum.radius = cbrt(body1.radius*body1.radius*body1.radius + body2.radius*body2.radius*body2.radius);

    sum.position.x = (body1.position.x + body2.position.x)/2;
    sum.position.y = (body1.position.y + body2.position.y)/2;

       
    sum.velocity.x = body1.mass/sum.mass * body1.velocity.x + body2.mass/sum.mass * body2.velocity.x ;
    sum.velocity.y = body1.mass/sum.mass * body1.velocity.y + body2.mass/sum.mass * body2.velocity.y ;
    sum.type = body1.type;
    sum.color_r = (body1.color_r + body2.color_r)/2;
    sum.color_g = (body1.color_g + body2.color_g)/2;
    sum.color_b = (body1.color_b + body2.color_b)/2;
    if (body1.mass > body2.mass){
        sum.name = body1.name;
    }
    else{
        sum.name = body2.name;
    }
    return sum;
}

void rebound_bodies(struct body *body1, struct body *body2){

    double norm1 = norm(body1->velocity);
    double norm2 = norm(body2->velocity);

    if (norm1 == 0.0 || norm2 == 0.0) return;

    double x1 = body1->velocity.x;
    double y1 = body1->velocity.y;

    double x2 = body2->velocity.x;
    double y2 = body2->velocity.y;

    body1->velocity.x = x2/norm2 * norm1;
    body1->velocity.y = y2/norm2 * norm1;

    body2->velocity.x = x1/norm1 * norm2;
    body2->velocity.y = y1/norm1 * norm2;
}

int are_colliding(struct body body1, struct body body2){
    double distance = norm(substract_2_vectors(body1.position, body2.position));


    if(distance < body1.radius + body2.radius){
        return 1;
    }
    else{
        return 0;
    }
}


double calculate_potential_energy(struct body body1, struct body *T, int N){
    double kinetic_energy = 0;
    struct vector OM;
    double d;
   
    for(int i = 0; i<N; i++){
        if(T[i].id != body1.id && T[i].type != 0){

            OM.x = T[i].position.x - body1.position.x;
            OM.y = T[i].position.y - body1.position.y;

            d = norm(OM);
            kinetic_energy -= G * body1.mass * T[i].mass / d;
        }
    }


    return kinetic_energy;
}


double calculate_kinetic_energy(struct body body1){
    return body1.mass * norm(body1.velocity)* norm(body1.velocity)  / 2;
}

double calculate_total_energy(struct body* bodies, int N){
    double E = 0;


    struct vector OM;
    double d;


    for(int i = 0; i < N; i++){
        if(bodies[i].type == 0) continue;


        E += calculate_kinetic_energy(bodies[i]);


        for(int j = i+1; j < N; j++){
            if(bodies[j].type == 0) continue;


            OM.x = bodies[i].position.x - bodies[j].position.x;
            OM.y = bodies[i].position.y - bodies[j].position.y;


            d = norm(OM);


            E -= G * bodies[i].mass * bodies[j].mass / d;
        }
    }
    return E;
}