/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
---    Use with a fragment shader in EYE COORDINATE space
-------------------------------------------------------*/


varying vec3 normal ;
varying vec3 light ;
varying vec3 view ;
varying float attenuation ;


void main ()
{
    normal = gl_NormalMatrix * gl_Normal;
    vec3 position = (gl_ModelViewMatrix * gl_Vertex).xyz;
    view    = normalize(vec3 (-position));

    light   = vec3(gl_LightSource[0].position) - position;
    float distance = length(light);
    light = normalize(light);

    attenuation = 1.00 / (gl_LightSource[0].constantAttenuation +
 			gl_LightSource[0].linearAttenuation * distance +
			gl_LightSource[0].quadraticAttenuation * distance * distance);


	// send the texture coordinate to the fragment program 
	gl_TexCoord[0] = gl_MultiTexCoord0;	
	
    gl_Position = ftransform ();
}

