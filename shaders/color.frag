#version 330 core

in vec3 interpolatedColor;
out vec4 outColor;

void main()
{
	outColor = vec4(interpolatedColor , 1.0);
}
