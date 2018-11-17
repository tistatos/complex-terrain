#version 420 core

in vec3 fragNormal;
in vec3 fragPosition;
in float fragAmbient;
in vec4 viewSpace;

out vec4 color;

const vec4 fogColor = vec4(vec3(0.6), 1.0);
const float fogDensity = 0.008;

void main() {
	//Calculate fog
	float dist = length(viewSpace);
	float fogFactor = clamp(1.0 / exp(dist*fogDensity), 0.0, 1.0);
	vec3 output = (vec3(0.5)+fragNormal/2.0);
	output *= clamp(fragAmbient, 0.0, 1.0);
	color = vec4(output, 1.0);

	color = mix(fogColor, color, fogFactor);
}
