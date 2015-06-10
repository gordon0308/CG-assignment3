#version 150

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;

uniform vec4 light_pos;         // Light position in eye-space

uniform vec3 light_ambient;     // Light ambient RGBA values
uniform vec3 light_diffuse;     // Light diffuse RGBA values  
uniform vec3 light_specular;    // Light specular RGBA values

uniform vec3 mtl_ambient;  // Ambient surface colour
uniform vec3 mtl_diffuse;  // Diffuse surface colour
uniform vec3 mtl_specular; // Specular surface colour

in vec3 a_vertex;
in vec4 a_colour;   // The new, per-vertex attribute    
in vec2 a_tex_coord;
in vec3 a_normal;


out vec4 litColour;

// These control how quickly the light falls off based on distance
// Not used in this version of the shader.
const float constantAttn = 0.0;
const float linearAttn = 0.0;
const float quadraticAttn = 0.05;
uniform bool light_on;    // Light specular RGBA values

out vec4 colour;
out vec2 st;


const float shininess = 32;

vec3 fogColour = vec3(0.2,0.3,0.4);

vec3 exponentialFog(in vec3 rgb,//origninal colour
                    in float distance,  //camera to point distance
                    in float b) //the density of the fog
{
    float fogAmount = 1.0 - exp(-distance*b);
    return mix(rgb,fogColour,fogAmount);
}

vec3 linearfog(in vec3 rgb, in float distance)
{
    float fogFactor = (1.0/distance);
    vec3 result = mix(rgb,fogColour,1-fogFactor);
    return result;
}

// This function computes the Phong lit colour for a vertex
// Parameters are the vertex position and normal in eye space.
vec3 phongPointLight(in vec4 position, in vec3 norm)
{
    // s is the direction from the light to the vertex
    vec3 s = normalize(vec3(light_pos - position));

    // v is the direction from the eye to the vertex
    vec3 v = normalize(-position.xyz);

    // r is the direction of light reflected from the vertex
    vec3 r = reflect( -s, norm );
    
    vec3 ambient = light_ambient * mtl_ambient;
	
    // The diffuse component
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = light_diffuse * mtl_diffuse * sDotN;

    // Specular component
    vec3 spec = vec3(0.0);
    if ( sDotN > 0.0 )
		spec = light_specular * mtl_specular *
            pow( max( dot(r,v), 0.0 ), shininess );

    return ambient + diffuse + spec;
}
void main(void) {
    float fFogCoord = abs(a_vertex.z); 

   	vec3 eyeNorm = normalize( normal_matrix * a_normal);
   	vec4 eyePos = modelview_matrix * vec4(a_vertex, 1.0);
	//if(light_on){
    		//colour = vec4(exponentialFog(phongPointLight(eyePos, eyeNorm), fFogCoord, 0.05), 1.0);
	//}else{
		colour=vec4(exponentialFog((mtl_diffuse),fFogCoord, 0.05),1.0);
	//}
	gl_Position = projection_matrix * eyePos;
	 st = a_tex_coord;

}