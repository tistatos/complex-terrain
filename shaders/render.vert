#version 420 core

in vec4 position;
in vec3 normal;

layout(std140) uniform camera {
	mat4 v;
	mat4 p;
	mat4 vp;
};

out vec3 fragNormal;
out vec3 fragPosition;

void main() {
	fragNormal = normal;
	gl_Position = vp*position;
	fragPosition = position.xyz;
}
