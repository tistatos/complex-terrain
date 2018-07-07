#version 420 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 m;

layout(std140) uniform camera {
	mat4 v;
	mat4 p;
	mat4 vp;
};

out vec3 fragColor;

void main() {
	gl_Position = vp * m * vec4(position, 1.0);
	fragColor = color;
}
