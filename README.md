3D Particle Gravity Simulation
==============================

By default uses 3000 particles, all weighing 10 kilos and G=1.0.
Particles are shaded from blue to red based on how much gravitational acceleration they are experiencing (blue = none, red = a lot).
1 unit of distance is 1 meter, 1 unit of mass is one kilogram.

The simulation is done entirely on the cpu, and no instancing is used. The number of threads used is equal to std::thread::hardware_concurrency()/2.
This typically means half of your cpu's cores. The number of threads used for the simulation will be displayed in the terminal.
The Barnes-Hut algorithm is used to speed up the simulation, and you can view the tree by holding down Space.

To compile, all you need is raylib and a c++14 compatible compiler. The code itself should be platform independent, however i have only tested the code on Linux Mint.

If you want to change how many particles are used, pass the number of particles as the first command line argument.

# Controls

WASD:   Moving around
Q:      Move up
E:      Move down
MOUSE:  Look around

ENTER:  Toggle fullscreen, the windowed resolution is 1280x720. The fullscreen resolution is equal to the monitor's resolution

SPACE:  See the Barnes-Hut tree, visualized via wireframe cubes representing each node of the oct-tree

C:      Pause the simulation
F:      Toggle 2x simulation speed

R:      Spawn in a particle moving 25m/s in the x axis
X:      Spawn in a cluster of 100 particles with no starting velocity
