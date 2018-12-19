#version 420 core
precision mediump float;

const int TEXTURE_MARGIN = 8;
const int TEXTURE_SIZE = 33;

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

//Density function texture
layout(binding = 1) uniform sampler3D density;

//The chunks position in world space
uniform vec3 chunkWorldPosition;
//6 bits each for XYZ, 2 divider bits then 4 bits for each edge
layout(location=0) in uint xyzEdges;

out vec4 worldPosition1;
out vec3 normal1;
out vec4 worldPosition2;
out vec3 normal2;
out vec4 worldPosition3;
out vec3 normal3;

float calculateDensity(vec3 worldPosition) {
	/// Sphere
	float density =	(16 - distance(worldPosition, vec3(0,0,0)));
	/*vec3 warp = vec3(*/
			/*snoise(worldPosition * 0.02),*/
			/*snoise((worldPosition + vec3(1.0)) * 0.01),*/
			/*snoise((worldPosition - vec3(1.0)) * 0.02));*/
	/*density -= 12*snoise((worldPosition+warp*20)/25.0);*/

	///Flat land
	//float density = -worldPosition.y;
	/*vec3 warp = vec3(*/
			/*snoise(worldPosition * 0.02),*/
			/*snoise((worldPosition + vec3(1.0)) * 0.01),*/
			/*snoise((worldPosition - vec3(1.0)) * 0.02));*/
	/*density -= 86.9*snoise((worldPosition+warp*20)/240.0);*/
	/*density -= 32*snoise((worldPosition+warp*20)/80.0);*/
	/*density -= 22*snoise((worldPosition+warp*20)/50.0);*/
	/*density -= 12*snoise((worldPosition+warp*20)/25.0);*/
	/*density -= 6*snoise((worldPosition+warp*20)/55.0);*/
	/*density -= 1.0*snoise((worldPosition+warp*20)/9.0);*/

	return density;
}

float calculateAmbient(vec3 pos) {
	const int RAY_COUNT = 32;
	float visibility = 0.0;
	float d = 1.0 / (TEXTURE_SIZE + (2 * TEXTURE_MARGIN));
	//Calculate AO using texture
	vec3 UVW = (pos.xyz + TEXTURE_MARGIN) * d;
	for(int i = 0; i < RAY_COUNT; i++) {
		float thisRay = 1.0;
		vec3 dir = rayDirections[i];
		for(int j = 1; j < 8; j ++) {
			float v = texture(density, UVW+dir*d*j).r;
			thisRay *= clamp(v*9999, 0.0, 1.0);
		}

		//calculate AO in worldspace using density function
		vec3 chunkPosition = vec3(chunkWorldPosition+vec3(-16.0));
		float stepSize = 1.0;
		for(int j = 5; j < 5; j++) {
			float v = calculateDensity(pos+chunkPosition+dir*j*stepSize);
			thisRay *= clamp(v*9999, 0.0, 1.0);
		}
		visibility += thisRay;
	}
	return (1 - visibility / RAY_COUNT);
}

vec3 getGradient(vec3 uvw) {
	float densityTextureSize = TEXTURE_SIZE + (2.0 * TEXTURE_MARGIN);
	float d = 1.0/densityTextureSize;

	vec3 gradient;

	gradient.x = texture( density, uvw + vec3(d, 0, 0)).x -
		texture(density, uvw + vec3(-d, 0, 0)).x;
	gradient.y = texture( density, uvw + vec3(0, d, 0)).x -
		texture(density, uvw + vec3(0, -d, 0)).x;
	gradient.z = texture(density, uvw + vec3(0, 0, d)).x -
		texture(density, uvw + vec3(-d, 0, -d)).x;

	return -normalize(gradient);
}

float interpolate(float a, float b) {
	return clamp(a / (a-b), 0.0, 1.0);
}

void main() {
	float d = 1.0 / (TEXTURE_SIZE + 2 * TEXTURE_MARGIN);
	vec4 chunkPosition = vec4(chunkWorldPosition-vec3(16.0), 0.0);

	ivec3 positionInChunk;
	positionInChunk.x = int(xyzEdges >> 26);
	positionInChunk.y = int((xyzEdges & 0x03F00000)  >> 20);
	positionInChunk.z = int((xyzEdges & 0x000FC000) >> 14);


	{
		uint edge1 = (xyzEdges & 0x00000F00) >> 8;
		ivec2 edgeVertices = edgeToVertices[edge1];

		ivec3 EdgeVert1 = vert_to_texcoord[edgeVertices.x] + positionInChunk;
		ivec3 EdgeVert2 = vert_to_texcoord[edgeVertices.y] + positionInChunk;
		float firstVertexDensityValue = texelFetch(density, EdgeVert1 + TEXTURE_MARGIN,0).r;
		float secondVertexDensityValue = texelFetch(density, EdgeVert2 + TEXTURE_MARGIN, 0).r;
		float t = interpolate(firstVertexDensityValue, secondVertexDensityValue);

		vec4 vertexVoxelPosition = vec4( EdgeVert1 * (1.0-t) + EdgeVert2 * t, 1.0);

		vec3 UVW = (vertexVoxelPosition.xyz + TEXTURE_MARGIN) * d;
		worldPosition1 = vertexVoxelPosition + chunkPosition;
		worldPosition1.w = calculateAmbient(vertexVoxelPosition.xyz);
		normal1 = getGradient(UVW);
	}
	{
		uint edge2 = (xyzEdges & 0x000000F0) >> 4;
		ivec2 edgeVertices = edgeToVertices[edge2];

		ivec3 EdgeVert1 = vert_to_texcoord[edgeVertices.x] + positionInChunk;
		ivec3 EdgeVert2 = vert_to_texcoord[edgeVertices.y] + positionInChunk;
		float firstVertexDensityValue = texelFetch(density, EdgeVert1 + TEXTURE_MARGIN,0).r;
		float secondVertexDensityValue = texelFetch(density, EdgeVert2 + TEXTURE_MARGIN, 0).r;
		float t = interpolate(firstVertexDensityValue, secondVertexDensityValue);

		vec4 vertexVoxelPosition = vec4( EdgeVert1 * (1.0-t) + EdgeVert2 * t, 1.0);

		vec3 UVW = (vertexVoxelPosition.xyz + TEXTURE_MARGIN) * d;
		worldPosition2 = vertexVoxelPosition + chunkPosition;
		worldPosition2.w = calculateAmbient(vertexVoxelPosition.xyz);
		normal2 = getGradient(UVW);
	}
	{
		uint edge3 = (xyzEdges & 0x0000000F);
		ivec2 edgeVertices = edgeToVertices[edge3];

		ivec3 EdgeVert1 = vert_to_texcoord[edgeVertices.x] + positionInChunk;
		ivec3 EdgeVert2 = vert_to_texcoord[edgeVertices.y] + positionInChunk;
		float firstVertexDensityValue = texelFetch(density, EdgeVert1 + TEXTURE_MARGIN,0).r;
		float secondVertexDensityValue = texelFetch(density, EdgeVert2 + TEXTURE_MARGIN, 0).r;
		float t = interpolate(firstVertexDensityValue, secondVertexDensityValue);

		vec4 vertexVoxelPosition = vec4( EdgeVert1 * (1.0-t) + EdgeVert2 * t, 1.0);

		vec3 UVW = (vertexVoxelPosition.xyz + TEXTURE_MARGIN) * d;
		worldPosition3 = vertexVoxelPosition + chunkPosition;
		worldPosition3.w = calculateAmbient(vertexVoxelPosition.xyz);
		normal3 = getGradient(UVW);
	}
}
