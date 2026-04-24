**<u>Planetary simulator</u>**

by Zacharie AMELIN & Tristan LE COTTY

The goal of the project is to build a simulation of the interactions between a large number of celestial bodies including stars, planets and asteroids. For now, we're implementing it in 2 dimensions and considering that when two bodies collide, they're rebounding of off each other, conserving global momentum. Later on, we'll probably consider that they can spawn a random number of smaller bodies, while still conserving the global momentum.

In order to numerically solve the equations we're given, we will use the leap-frog integration method, which seemed quite accurate for this problem.

Finally, we're using the SLD2 library for the graphics


**How to run the simulation :**
1) Install the SDL2 library
2) Run "make" 
3) Run "./program < bodies.in" with the bodies.in file being as follows :
    - The first line of the file should contain an integer : the number of bodies in the simulation
    - Each following line will be the information for the different bodies separated by a blank space in the following order : id mass radius position.x position.y velocity.x velocity.y type
    The specifications for these values can be found below

int id : identification number of each body, has to be different for each body
double mass : mass of the body
double radius : radius of the body
double position.x : position of the center of the body on the x axis 
double position.y : position of the center of the body on the y axis 
double velocity.x : x coordinate of the velocity of the center of the body
double velocity.y : y coordinate of the velocity of the center of the body
int type : type of the body. type = 0 is not to be put as value initially, as bodies with a type of 0 are neither rendered nor taken into account for the calculation of the gravitationnal forces



**Controls for the simulation :**
- Pressing the 'P' key once will pause the simulation. Pressing 'P' again will resume it
- Pressing the left arrow key once will make the simulation run backwards in time. Pressing the right arrow key will make it run normally again.