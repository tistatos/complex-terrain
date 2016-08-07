#version 420 core
precision mediump float;

void main() {
  const vec4 quad[4]  = vec4[4] { vec4 (-1.0, -1.0, 1.0, 1.0),
                                  vec4 ( 1.0, -1.0, 1.0, 1.0),
                                  vec4 (-1.0,  1.0, 1.0, 1.0),
                                  vec4 ( 1.0,  1.0, 1.0, 1.0) );
  gl_Position = quad[gl_VertexID];
}

