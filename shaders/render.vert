#version 420 core

in vec4 position;
in vec3 normal;

layout(std140) uniform camera {
	mat4 v;
	mat4 p;
	mat4 vp;
};

out vec3 fragNormal;

void main() {
	fragNormal = (normal + 1.0) * 0.5;
	gl_Position = vp*position;
}
