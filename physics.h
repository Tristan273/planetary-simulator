#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define G 50  // attraction force constant

/* STRUCTURES */
struct vector{
    double x;
    double y;
};

struct body{
    int id;

    double mass;
    double radius;

    struct vector position;
    struct vector velocity;

    int color_r;
    int color_g;
    int color_b;
    
    int type;
    
    char* name;
};

/* FUNCTION PROTOTYPES */

double norm(struct vector u);

struct vector add_2_vectors(struct vector u, struct vector v);

struct vector add_N_vectors(struct vector *T, int N);

struct vector simulate_grav_force2(struct body body1, struct body body2);

struct vector simulate_grav_forceN(struct body body1, struct body *T, int N);

struct body merge_bodies(struct body body1, struct body body2);

void rebound_bodies(struct body *body1, struct body *body2);

int are_colliding(struct body body1, struct body body2);

struct vector substract_2_vectors(struct vector u, struct vector v);

double calculate_potential_energy(struct body body1, struct body *T, int N);

double calculate_kinetic_energy(struct body body1);

double calculate_total_energy(struct body* bodies, int N);


#endif