

#version 150

out vec4 fragColor;
uniform samplerCube cube_texutre;
in vec3 texcoords;
				
void main( void ) {


	fragColor = texture(cube_texutre, texcoords);
    
}