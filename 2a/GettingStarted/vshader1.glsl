// identity vertex shader
#version 120
attribute vec4 vertex_position;
attribute vec4 vertex_color;
varying vec4 color;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()  {
	gl_Position = P*V*M*vertex_position/vertex_position.w;  // pass on the vertex position unchanged
	color = vertex_color;
}