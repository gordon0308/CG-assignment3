

#version 150


uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

in vec3 a_vertex;
out vec3 texcoords;
void main( void ) {
	texcoords=a_vertex;
	gl_Position = projection_matrix * modelview_matrix * vec4( a_vertex, 1.0 );

}