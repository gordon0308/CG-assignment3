
#version 150

// manyAttributes.fp
// An example of using interpolated values from the previous stage

in vec4 colour; // The per-vertex, colour attribute from the previous stage.

				// Take note that _per-vertex attributes_ are sent from previous stage
				// whereas uniforms are available for access in both stages

in vec2 st;
uniform sampler2D texMap;

out vec4 fragColour;

void main(void) {


	fragColour = colour*texture(texMap, st);	// We now just have to set the already smoothed colour as our frag colour


}
