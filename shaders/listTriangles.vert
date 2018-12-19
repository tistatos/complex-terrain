#version 420 core

layout(location=0) in ivec3 voxelInPosition;

out ivec3 voxelPosition;

void main() {
	voxelPosition = voxelInPosition;
}
