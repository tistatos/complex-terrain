#version 420 core
precision mediump float;

in ivec3 voxelPosition[];
out vec4 worldPosition;
out vec3 normal;

uniform vec3 chunkWorldPosition;

layout(points) in;
layout(points, max_vertices = 15) out;

//Density function texture
layout(binding = 1) uniform sampler3D density;
//case lookup - stores edges the triangle vertices lies on
layout(binding = 2) uniform isampler2D triTable;

//lookup table for amount of triangles form each case
const int edgeTable[256] =
{
	0, 1, 1, 2, 1, 2, 2, 3,  1, 2, 2, 3, 2, 3, 3, 2,  1, 2, 2, 3, 2, 3, 3, 4,  2, 3, 3, 4, 3, 4, 4, 3,
	1, 2, 2, 3, 2, 3, 3, 4,  2, 3, 3, 4, 3, 4, 4, 3,  2, 3, 3, 2, 3, 4, 4, 3,  3, 4, 4, 3, 4, 5, 5, 2,
	1, 2, 2, 3, 2, 3, 3, 4,  2, 3, 3, 4, 3, 4, 4, 3,  2, 3, 3, 4, 3, 4, 4, 5,  3, 4, 4, 5, 4, 5, 5, 4,
	2, 3, 3, 4, 3, 4, 2, 3,  3, 4, 4, 5, 4, 5, 3, 2,  3, 4, 4, 3, 4, 5, 3, 2,  4, 5, 5, 4, 5, 2, 4, 1,
	1, 2, 2, 3, 2, 3, 3, 4,  2, 3, 3, 4, 3, 4, 4, 3,  2, 3, 3, 4, 3, 4, 4, 5,  3, 2, 4, 3, 4, 3, 5, 2,
	2, 3, 3, 4, 3, 4, 4, 5,  3, 4, 4, 5, 4, 5, 5, 4,  3, 4, 4, 3, 4, 5, 5, 4,  4, 3, 5, 2, 5, 4, 2, 1,
	2, 3, 3, 4, 3, 4, 4, 5,  3, 4, 4, 5, 2, 3, 3, 2,  3, 4, 4, 5, 4, 5, 5, 2,  4, 3, 5, 4, 3, 2, 4, 1,
	3, 4, 4, 5, 4, 5, 3, 4,  4, 5, 5, 2, 3, 4, 2, 1,  2, 3, 3, 2, 3, 4, 2, 1,  3, 2, 4, 1, 2, 1, 1, 0
};

//lookup table to get vertices from edge
const ivec2 edgeToVertices[12] = {
	ivec2(0, 1),
	ivec2(1, 2),
	ivec2(2, 3),
	ivec2(3, 0),
	ivec2(4, 5),
	ivec2(5, 6),
	ivec2(6, 7),
	ivec2(7, 4),
	ivec2(0, 4),
	ivec2(1, 5),
	ivec2(2, 6),
	ivec2(3, 7)
};

//lookup table with the voxel edge vertices
const ivec3 voxelVerts[8] = {
	ivec3(0, 0, 0),
	ivec3(0, 1, 0),
	ivec3(1, 1, 0),
	ivec3(1, 0, 0),
	ivec3(0, 0, 1),
	ivec3(0, 1, 1),
	ivec3(1, 1, 1),
	ivec3(1, 0, 1)
};
const ivec3 vert_to_texcoord[8] = {
	ivec3(0, 0, 0), // v0
	ivec3(0, 1, 0), // v1
	ivec3(1, 1, 0), // v2
	ivec3(1, 0, 0), // v3
	ivec3(0, 0, 1), // v4
	ivec3(0, 1, 1), // v5
	ivec3(1, 1, 1), // v6
	ivec3(1, 0, 1)  // v7
};

vec3 getGradient(vec3 uvw) {
	float densityTextureSize = 32.0 + 2.0 * 2.0;
	float d = 1.0/densityTextureSize;

	vec3 gradient;

	gradient.x = texture( density, uvw + vec3(d, 0, 0)).x -
		texture(density, uvw + vec3(-d, 0, 0)).x;
	gradient.y = texture( density, uvw + vec3(0, d, 0)).x -
		texture(density, uvw + vec3(0, -d, 0)).x;
	gradient.z = texture(density, uvw + vec3(0, 0, d)).x -
		texture(density, uvw + vec3(-d, 0, -d)).x;

	return gradient;
}

void main() {
	ivec3 position = voxelPosition[0];

	//Get the density values in the corners of the voxel to Determine marching cube case
	uint caseID = 0;
	for (int i = 7; i >= 0; --i)
		caseID = caseID | ((texelFetch(density, 2 + position + voxelVerts[i], 0).r > 0) ? 1 : 0) << i;
	uint triangles = edgeTable[caseID];

	vec4 chunkPosition = vec4(chunkWorldPosition-vec3(16.0), 0.0);
	//Create triangles by:
	// 1. Get edges for this triangle
	// 2. get the edge-vertices of the edge
	// 3. determine the interpolated density value on the edge to create a new vertex there
	for(uint i = 0; i < triangles; i++) {
		// Compute the vertex position
		uint edge = texelFetch(triTable, ivec2(3 * i + 0, caseID), 0).r;
		ivec2 verts = edgeToVertices[edge];
		float firstVertexDensityValue = texelFetch(density, vert_to_texcoord[verts.x],
				0).r;
		float secondVertexDensityValue = texelFetch(density, vert_to_texcoord[verts.y],
				0).r;
		ivec3 EdgeVert1 = vert_to_texcoord[verts.x] + position;
		ivec3 EdgeVert2 = vert_to_texcoord[verts.y] + position;

		//float interpolation = clamp(firstVertexDensityValue /
				//(firstVertexDensityValue - secondVertexDensityValue), 0.0, 1.0);
		float interpolation = 0.5;
		vec4 vertexPosition = vec4(
				EdgeVert1 * (1.0-interpolation) + EdgeVert2 * interpolation, 1.0);

		float d = 1.0 / (33 + 2 * 2);
		vec3 UVW = (vertexPosition.xyz + 2) * d;
		vec3 gradient = getGradient(UVW);
		normal = -normalize(gradient);
		worldPosition = vertexPosition + chunkPosition;
		EmitVertex();



		edge = texelFetch(triTable, ivec2(3 * i + 1, caseID), 0).r;
		verts = edgeToVertices[edge];
		firstVertexDensityValue = texelFetch(density, vert_to_texcoord[verts.x], 0).r;
		secondVertexDensityValue = texelFetch(density, vert_to_texcoord[verts.y], 0).r;
		EdgeVert1 = vert_to_texcoord[verts.x] + position;
		EdgeVert2 = vert_to_texcoord[verts.y] + position;

		//interpolation = clamp(firstVertexDensityValue /
				//(firstVertexDensityValue - secondVertexDensityValue), 0.0, 1.0);
		worldPosition = vec4(
				EdgeVert1 * (1.0-interpolation) + EdgeVert2 * interpolation, 1.0);
		worldPosition += chunkPosition;
		EmitVertex();

		edge  = texelFetch(triTable, ivec2(3 * i + 2, caseID), 0).r;
		verts = edgeToVertices[edge];
		firstVertexDensityValue = texelFetch(density, vert_to_texcoord[verts.x], 0).r;
		secondVertexDensityValue = texelFetch(density, vert_to_texcoord[verts.y], 0).r;
		EdgeVert1 = vert_to_texcoord[verts.x] + position;
		EdgeVert2 = vert_to_texcoord[verts.y] + position;

		//interpolation = clamp(firstVertexDensityValue /
				//(firstVertexDensityValue - secondVertexDensityValue), 0.0, 1.0);
		worldPosition = vec4(
				EdgeVert1 * (1.0-interpolation) + EdgeVert2 * interpolation, 1.0);
		worldPosition += chunkPosition;
		EmitVertex();
	}
}
