#version 420 core
uniform int empty;

void main() {
	gl_FragColor = empty > 0 ?
		vec4(1.0, 0.0, 0.0, 1.0) :
		vec4(0.0, 1.0, 0.0, 1.0);
}
