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
out float fragAmbient;
out vec4 viewSpace;

void main() {
	fragNormal = normal;
	fragAmbient = position.w;
	fragPosition = position.xyz;
	viewSpace = v * vec4(fragPosition, 1.0);
	gl_Position = p * viewSpace;
}
