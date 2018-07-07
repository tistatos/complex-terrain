#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(std140) uniform camera {
	mat4 v;
	mat4 p;
	mat4 vp;
};

out vec3 interpolatedColor;

void main()
{
	gl_Position = vp * vec4(inPosition,1.0);
	interpolatedColor = inColor;
}
