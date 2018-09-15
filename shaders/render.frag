#version 420 core

in vec3 fragNormal;
in vec3 fragPosition;
in float fragAmbient;
out vec4 color;

void main() {
	vec3 output = (vec3(0.5)+fragNormal/2.0);
	output *= clamp(fragAmbient, 0.0, 1.0);
	color = vec4(output, 1.0);
}
