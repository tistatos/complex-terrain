#version 420 core
in vec3 vertexPosition;
uniform vec3 position;
uniform vec3 size;

layout(std140) uniform camera {
	mat4 v;
	mat4 p;
	mat4 vp;
};

void main() {
	gl_Position = vp*vec4((vertexPosition*size/2.0)+position, 1.0);
}
