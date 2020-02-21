# Simple opengl procedurally generated tarrain

In this project I procedurally generate terrain using a noise function. For the terrain generation i stumbled upon the Marching Cubes algorithm. I found it very interesting so I implemented it and used it to compute the triangle mesh for the terrain. I used [SimplexNoise](https://github.com/SRombauts/SimplexNoise) for the noise sampling. I devide the space into voxels to use with the Marching Cubes algorithm. Each vertex of each voxel is assigned a value sampled from the 3D Perlin noise function and later used to determine wether that vertex is inside or outside the terrain( Used for in the Marchin Cubes algorithm).

I also used help for my original setup from [The Cherno](https://www.youtube.com/user/TheChernoProject). This was used to setup the groundwork for building shaders, OpenGL setup, debugging and other helpful tips. I also used code from [OpenGLPrj](https://github.com/joksim/OpenGLPrj), a resources repository for the Computer Graphics course. And finnaly I used a class for rendering spheres found [here.](https://www.songho.ca/opengl/gl_sphere.html) 

## Terrain generation in action

<a href="https://media3.giphy.com/media/YOLtOwHcObQbwpykl4/giphy.webp"><img src="https://media3.giphy.com/media/YOLtOwHcObQbwpykl4/giphy.webp" title="Generating terrain"/></a>

## Here is a look at the generated terrain

<a href="https://media3.giphy.com/media/JU9oC4nRmYzduTUkG5/giphy.webp"><img src="https://media3.giphy.com/media/JU9oC4nRmYzduTUkG5/giphy.webp" title="Moving aroung"/></a>

The shader is very simple, it uses a single color and Phong lighting. I would like to develop this further in the future.

<a href="https://media2.giphy.com/media/gGktZmZVzhrA4gbnzE/giphy.webp"><img src="https://media2.giphy.com/media/gGktZmZVzhrA4gbnzE/giphy.webp" title="Looking around"/></a>

## Ray picking

I also attempted to implement ray picking for further development where i plan to user terrain manipulation (adding and removing terrain). I use the fast voxel traversal algorithm by *John Amanatides* and *Andrew Woo*, for the ray casting to determine the picked voxel. I render a sphere to represent the camera pointer, as can be seen.

<a href="https://media3.giphy.com/media/XgTKtxZuAZfymgPcP7/giphy.webp"><img src="https://media3.giphy.com/media/XgTKtxZuAZfymgPcP7/giphy.webp" title="Ray-casting"/></a>

It is far from finished. I manage the terrain i chunks and generate it as needed. There is a lot of optimization left to do. The shaders are too simple. I plan to finish this project and keep working on it in the future.







