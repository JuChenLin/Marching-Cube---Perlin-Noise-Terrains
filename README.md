## Perlin Noise Terrains with Marching Cube Algorithm
### **Generating Complex Procedural Terrains Using Marching Cube Algorithm**


### Functions

##### Perlin Noise
A popular way to define a terrain is using noise function, and Perlin Noise is a broadly adopted one among many noise gererating algorithms. I created two terrains with 2D and 3D Perlin Noise function respectively. To make the terrain look more natural, I repeated noise function several times with varying frequencies and amplitudes, then added them together. The result of the noise effect was just the density function needed for Marching Cube algorithm.

##### Marching Cube
Marching Cube is another essential algorithm to implement. There are many illustrations about how to build Marching Cube algorithm can be found on the internet. I just had to copy the lookup tables for covered vertices and intersected edges, then the algorithm was not difficult to create.

##### Color Mapping
Performing the density function and marching cube algorithm, a terrain surface could be well rendered. Then I created a color bar of typical gradient color of the ground, and mapped the color to the height of the terrain.


### API
OpenGL (Open Graphics Library)


### Results

##### 2D Terrain
![image]( https://github.com/JuChenLin/Marching-Cube---Perlin-Noise-Terrains/blob/master/Results/2D_1.png)
![image]( https://github.com/JuChenLin/Marching-Cube---Perlin-Noise-Terrains/blob/master/Results/2D_2.png)

##### 3D Terrain
![image]( https://github.com/JuChenLin/Marching-Cube---Perlin-Noise-Terrains/blob/master/Results/3D_1.png)
![image]( https://github.com/JuChenLin/Marching-Cube---Perlin-Noise-Terrains/blob/master/Results/3D_2.png)
