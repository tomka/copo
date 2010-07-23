/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
---    Use with a fragment shader in LOCAL COORDINATE space
-------------------------------------------------------*/

varying vec3 light ;
varying vec3 normal ;
varying vec3 view ;
varying float attenuation ;

attribute vec3 tangent;
attribute vec3 binormal;

void main ()
{   
    vec3 tvec = normalize(gl_NormalMatrix * tangent);
    vec3 bvec = normalize(gl_NormalMatrix * binormal);
    normal = normalize(gl_NormalMatrix * gl_Normal);

    
    vec3 position = (gl_ModelViewMatrix * gl_Vertex).xyz;
    vec3 lightPos = vec3(gl_LightSource[0].position) ;
    
    float distance = length(light - position);
    attenuation = 1.00 / (gl_LightSource[0].constantAttenuation +
 			gl_LightSource[0].linearAttenuation * distance +
			gl_LightSource[0].quadraticAttenuation * distance * distance);
       
    vec3 v;
    v.x = dot(lightPos, tvec);
    v.y = dot(lightPos, bvec);
    v.z = dot(lightPos, normal);
    light = normalize(v);

    v.x = dot(position, tvec);
    v.y = dot(position, bvec);
    v.z = dot(position, normal);
    view = normalize(v);    

	// send the texture coordinate to the fragment program 
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
    gl_Position = ftransform ();
}

