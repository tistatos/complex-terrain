# Complex-terrain
Project in Procedural Methods for Images - TNM084

Based on the article "Generating Complex Procedural Terrains Using the GPU"
from GPU Gems 3

Written for openGL with glew, glfw and glm

### Get started
Do the usual
```
mkdir build
cd build
cmake ..
make
```
Done!

### Performance

initial terrain rendering of 5625 chunks takes about 17 seconds
next improvement rendering is 13.5 seconds (~20% improvement)

### TODO:
- frustum culling
		- frustum visualization DONE
		- detatch from camera: DONE
		- determine error: Sorta done... seems as if i try to use the tan value of the fov in a proper
				way, the size of the frustum will be incorrect
- fog post processing DONE
- improved generation DONE
- Even more improved generation

- Fade in of new chunks
- profiling of draw time
- dilation erosion on the density data to avoid the small dots that are present
- only use quaternions for rotation
