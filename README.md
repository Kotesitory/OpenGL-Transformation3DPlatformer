# Simple opengl procedurally generated tarrain

I changed my mind about the platformer. The initial idea was to use nurbs and splines to create a 3D platformer with transforming terrain. I implemented that idea to some point, but finally changed my mind this January and went for another project. 

## New plan

For my new plan i decided to build something different but similar in nature. I wanted to attempt to procedurally generate terrain and use some noise function. For the terrain generation i stumbled upon the Marching Cubes algorithm. So i used that to generat terrain from data sampled from Perlin noise. I used [SimplexNoise](https://github.com/SRombauts/SimplexNoise) for the noise generation.

I also used help for my original setup from [The Cherno](https://www.youtube.com/user/TheChernoProject). This was used to setup the groundwork for building shaders, OpenGL setup and other helpful tips. I also used code from [OpenGLPrj](https://github.com/joksim/OpenGLPrj), a resources repository for the course of Computer Graphics. And finnaly I used a class for rendering spheres found here.(https://www.songho.ca/opengl/gl_sphere.html) 

It is far from finished. I have only implemented the terrain genlration, using voxels for Marching Cubes. I manage the terrain i chunks and generate it as needed. There is a lot of optimization left to do. I have only implemented Phong lighting and nothing else special in the shaders. Also a rudimented ray-picking system that I plan to furter develop to be used for terrain manipulation(this also doesn't work completely yet). There is no collision detection for tessting reasons and also I haven't gotten to that yet but I have a plan on how it would be implemented.

<a href="https://media3.giphy.com/media/XgTKtxZuAZfymgPcP7/giphy.webp"><img src="https://media3.giphy.com/media/XgTKtxZuAZfymgPcP7/giphy.webp" title="Ray-casting"/></a>

<a href="https://media3.giphy.com/media/JU9oC4nRmYzduTUkG5/giphy.webp"><img src="https://media3.giphy.com/media/JU9oC4nRmYzduTUkG5/giphy.webp" title="Moving aroung"/></a>

<a href="https://media2.giphy.com/media/gGktZmZVzhrA4gbnzE/giphy.webp"><img src="https://media2.giphy.com/media/gGktZmZVzhrA4gbnzE/giphy.webp" title="Looking around"/></a>

<a href="https://media3.giphy.com/media/YOLtOwHcObQbwpykl4/giphy.webp"><img src="https://media3.giphy.com/media/YOLtOwHcObQbwpykl4/giphy.webp" title="Generating terrain"/></a>
