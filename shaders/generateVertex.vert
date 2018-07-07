#version 420 core

layout(binding=0) uniform sampler3D density;
layout(location=1) in vec3 position;

out vec4 worldPosition;
out vec3 normal;

vec3 getGradient(vec3 uvw) {
	float densityTextureSize = 32.0 + 2.0 * 2.0;
	float d = 1.0/densityTextureSize;

	vec3 gradient;

	gradient.x = texture( density, uvw + vec3(d, 0, 0)).x -
		texture(density, uvw + vec3(-d, 0, 0)).x;
	gradient.y = texture( density, uvw + vec3(0, d, 0)).y -
		texture(density, uvw + vec3(0, -d, 0)).y;
	gradient.z = texture(density, uvw + vec3(0, 0, d)).z -
		texture(density, uvw + vec3(-d, 0, -d)).z;

	return gradient;
}

void main() {
	worldPosition = vec4(position, 1.0);

	vec3 gradient = getGradient(vec3(0));

	normal = -normalize(gradient);
}
