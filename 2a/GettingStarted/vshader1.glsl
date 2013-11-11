// identity vertex shader
#version 120
attribute vec4 vertex_position;

void main()  {
	gl_Position = vertex_position;  // pass on the vertex position unchanged
}