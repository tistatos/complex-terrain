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


### TODO:
- frustum culling
		- frustum visualization DONE
		- detatch from camera: partially done, need to have camera matrix onto
		- determine error
- fog post processing
- improved generation
- profiling of draw time

- dilation erosion on the density data to avoid the small dots that are present
- only use quaternions for rotation
