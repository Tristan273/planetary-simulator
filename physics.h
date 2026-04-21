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
    
    int type;
};

/* FUNCTION PROTOTYPES */

double norm(struct vector u);

struct vector add_2_vectors(struct vector u, struct vector v);

struct vector add_N_vectors(struct vector *T, int N);

struct vector simulate_grav_force2(struct body body1, struct body body2);

struct vector simulate_grav_forceN(struct body body1, struct body *T, int N);

struct body merge_bodies(struct body body1, struct body body2);

void rebound_bodies(struct body *body1, struct body *body2);

#endif