#version 420 core

in vec3 fragNormal;
in vec3 fragPosition;
in float fragAmbient;
out vec4 color;

void main() {
	//color = vec4(fragNormal, 1.0);
	//vec3 output = vec3(
			//(0.4 + 0.3*sin(fragPosition.x/10))
			//);
	vec3 output = (vec3(0.5)+fragNormal/2.0) * clamp(1.0 - 2.0 * fragAmbient, 0.0, 1.0);
	//color = vec4(fragNormal.x/32, fragNormal.y, fragNormal.z/32, 1.0);
	color = vec4(output, 1.0);
}
