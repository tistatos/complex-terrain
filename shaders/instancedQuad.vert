#version 420 core
precision mediump float;

out flat int instanceID;

void main() {
	const vec4 quad[4]  = vec4[4] ( vec4 (-1.0, -1.0, 1.0, 1.0),
																	vec4 ( 1.0, -1.0, 1.0, 1.0),
																	vec4 (-1.0,  1.0, 1.0, 1.0),
																	vec4 ( 1.0,  1.0, 1.0, 1.0) );
	instanceID = gl_InstanceID;
	gl_Position = quad[gl_VertexID];
}

