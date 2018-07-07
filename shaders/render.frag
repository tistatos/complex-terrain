#version 420 core

in vec3 fragNormal;
out vec4 color;

void main() {
	color = vec4(fragNormal, 1.0);
}
