#version 420 core
precision mediump float;

layout(points) in;
layout(points, max_vertices = 3) out;

in vec4 worldPosition1[];
in vec3 normal1[];
in vec4 worldPosition2[];
in vec3 normal2[];
in vec4 worldPosition3[];
in vec3 normal3[];

out vec4 worldPosition;
out vec3 normal;

void main() {
	worldPosition = worldPosition1[0];
	normal = normal1[0];
	EmitVertex();

	worldPosition = worldPosition2[0];
	normal = normal2[0];
	EmitVertex();

	worldPosition = worldPosition3[0];
	normal = normal3[0];
	EmitVertex();
}
