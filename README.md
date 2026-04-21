Planetary Simulator

Tristan LE COTTY

Zacharie AMELIN

The goal of the project is to build a simulation of the interactions between a large number of celestial bodies including stars, planets and asteroids. For now, we're implementing it in 2 dimensions and considering that when two bodies collide, they're rebounding of off each other, conserving global momentum. Later on, we'll probably consider that they can spawn a random number of smaller bodies, while still conserving the global momentum.
In order to numerically solve the equations we're given, we will use the leap-frog integration method, which seemed quite accurate for this problem.
Finally, we're using the SLD2 library for the graphics