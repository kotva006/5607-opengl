// identity vertex shader
#version 120
attribute vec4 vertex_position;
attribute vec4 vertex_color;
varying vec4 color;
uniform mat4 M;

void main()  {
	gl_Position = M*vertex_position;  // pass on the vertex position unchanged
	color = vertex_color;
}