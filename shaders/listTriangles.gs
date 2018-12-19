#version 420 core
/*precision mediump float;*/

const int TEXTURE_MARGIN = 8;
/*const int TEXTURE_SIZE = 33;*/

in ivec3 voxelPosition[];
out uint xyzEdges;

layout(points) in;
layout(points, max_vertices = 5) out;

//Density function texture
layout(binding = 1) uniform sampler3D density;
//case lookup - stores edges the triangle vertices lies on
layout(binding = 2) uniform isampler2D triTable;

/*//lookup table for amount of triangles form each case*/
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

/*//lookup table to get vertices from edge*/
/*const ivec2 edgeToVertices[12] = {*/
	/*ivec2(0, 1),*/
	/*ivec2(1, 2),*/
	/*ivec2(2, 3),*/
	/*ivec2(3, 0),*/
	/*ivec2(4, 5),*/
	/*ivec2(5, 6),*/
	/*ivec2(6, 7),*/
	/*ivec2(7, 4),*/
	/*ivec2(0, 4),*/
	/*ivec2(1, 5),*/
	/*ivec2(2, 6),*/
	/*ivec2(3, 7)*/
/*};*/

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

/*const ivec3 vert_to_texcoord[8] = {*/
	/*ivec3(0, 0, 0), // v0*/
	/*ivec3(0, 1, 0), // v1*/
	/*ivec3(1, 1, 0), // v2*/
	/*ivec3(1, 0, 0), // v3*/
	/*ivec3(0, 0, 1), // v4*/
	/*ivec3(0, 1, 1), // v5*/
	/*ivec3(1, 1, 1), // v6*/
	/*ivec3(1, 0, 1)  // v7*/
/*};*/

void main() {
	//position within voxel
	ivec3 positionInVoxel = voxelPosition[0];

	//Get the density values in the corners of the voxel to determine marching cube case
	uint caseID = 0;
	for (int i = 7; i >= 0; --i)
		caseID = caseID | ((texelFetch(density, TEXTURE_MARGIN + positionInVoxel + voxelVerts[i], 0).r
		> 0.0) ? 1 : 0) << i;
	uint triangles = edgeTable[caseID];
	if(caseID != 0 && caseID != 255) {
		for(uint i = 0; i < triangles; i++) {
			uint edge1 = texelFetch(triTable, ivec2(3 * i + 0, caseID), 0).r;
			uint edge2 = texelFetch(triTable, ivec2(3 * i + 1, caseID), 0).r;
			uint edge3 = texelFetch(triTable, ivec2(3 * i + 2, caseID), 0).r;
			xyzEdges =
				uint(positionInVoxel.x) << 26 |
				uint(positionInVoxel.y) << 20 |
				uint(positionInVoxel.z) << 14 |
				uint(0) << 12 |
				uint(edge1) << 8 |
				uint(edge2) << 4 |
				uint(edge3) << 0 ;
			EmitVertex();
		}
	}
}
