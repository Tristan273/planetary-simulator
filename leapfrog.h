#ifndef LEAPFROG_H
#define LEAPFROG_H

#include "physics.h"

void leapfrog_step(struct body *bodies, int N, double dt);

#endif