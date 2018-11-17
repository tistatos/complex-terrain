#version 420 core
in vec3 vertexPosition;
uniform vec3 position;

uniform mat4 m;

layout(std140) uniform camera {
	mat4 v;
	mat4 p;
	mat4 vp;
};

void main() {
	gl_Position = vp * m * vec4(vertexPosition, 1.0);
}
