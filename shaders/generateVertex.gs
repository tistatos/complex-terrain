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

const vec3 rayDirections[32] = {
vec3(0.286582,	 0.257763, -0.922729),
vec3(-0.171812, -0.888079 	 , 0.426375 ),
vec3(0.440764, -0.502089 	 , -0.744066),
vec3(-0.841007, -0.428818 	 , -0.329882),
vec3(-0.380213, -0.588038 	 , -0.713898),
vec3(-0.055393, -0.207160 	 , -0.976738),
vec3(-0.901510, -0.077811 	 , 0.425706 ),
vec3(-0.974593, 0.123830 	 , -0.186643),
vec3(0.208042, -0.524280 	 , 0.825741 ),
vec3(0.258429, -0.898570 	 , -0.354663),
vec3(-0.262118, 0.574475 	 , -0.775418),
vec3(0.735212, 0.551820 	 , 0.393646 ),
vec3(0.828700, -0.523923 	 , -0.196877),
vec3(0.788742, 0.005727 	 , -0.614698),
vec3(-0.696885, 0.649338 	 , -0.304486),
vec3(-0.625313, 0.082413 	 , -0.776010),
vec3(0.358696, 0.928723 	 , 0.093864 ),
vec3(0.188264, 0.628978 	 , 0.754283 ),
vec3(-0.495193, 0.294596 	 , 0.817311 ),
vec3(0.818889, 0.508670 	 , -0.265851),
vec3(0.027189, 0.057757 	 , 0.997960 ),
vec3(-0.188421, 0.961802 	 , -0.198582),
vec3(0.995439, 0.019982 	 , 0.093282 ),
vec3(-0.315254, -0.925345 	 , -0.210596),
vec3(0.411992, -0.877706 	 , 0.244733 ),
vec3(0.625857, 0.080059 	 , 0.775818 ),
vec3(-0.243839, 0.866185 	 , 0.436194 ),
vec3(-0.725464, -0.643645 	 , 0.243768 ),
vec3(0.766785, -0.430702 	 , 0.475959 ),
vec3(-0.446376, -0.391664 	 , 0.804580 ),
vec3(-0.761557, 0.562508 	 , 0.321895 ),
vec3(0.344460, 0.753223 	 , -0.560359)
};

float calculateAmbient(vec3 pos) {
	float visibility = 0.0;
	float densityTextureSize = 32.0 + 2.0 * 2.0;
	float rstep = 1.0/densityTextureSize;
	for(int i = 0; i < 32; i++) {
		float thisRay = 1.0;
		vec3 dir = rayDirections[i];
		for(int j = 1; j < 8; j++) {
			/*float d = texture(density, pos + dir * j*0.00).r;*/
			float d = texture(density, pos+dir*rstep*0.01).r;
			thisRay *= clamp(d*9999, 0.0, 1.0);
		}
		visibility += thisRay;
	}
	return (1 - visibility / 32.0);
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
		float ambient = calculateAmbient(UVW);
		normal = -normalize(gradient);
		worldPosition = vertexPosition + chunkPosition;
		worldPosition.w = ambient;
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
		UVW = (worldPosition.xyz + 2) * d;
		gradient = getGradient(UVW);
		normal = -normalize(gradient);
		worldPosition += chunkPosition;
		ambient = calculateAmbient(UVW);
		worldPosition.w = ambient;
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

		UVW = (worldPosition.xyz + 2) * d;
		gradient = getGradient(UVW);
		normal = -normalize(gradient);
		worldPosition += chunkPosition;
		ambient = calculateAmbient(UVW);
		worldPosition.w = ambient;
		EmitVertex();
	}
}
