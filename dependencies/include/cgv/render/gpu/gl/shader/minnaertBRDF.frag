/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
-------------------------------------------------------*/

uniform float exp ;
uniform float kd ;
uniform float normFactor ; 

uniform sampler2D texture;
uniform int useTexture ;

varying vec3 normal ;
varying vec3 light ;
varying vec3 view ;
varying float attenuation ;

void main ()
{	
    vec3 N = normalize (normal); // renormalized 
    vec3 L = normalize (light);  // renormalized 
    vec3 E = normalize (view);   //renormalized

    float NdotL = dot(N,L) ;
    float NdotE = dot(N,E) ;
    float cosine = max( NdotL * NdotE, 0.0) ;
    float fr_eval = normFactor * pow( cosine, exp );
    
   
    vec3 ambientAndEmision =  vec3 (gl_FrontLightModelProduct.sceneColor) * attenuation ;
    vec3 meshColor = (useTexture == 1) ? vec3(texture2D (texture, gl_TexCoord[0].st)) : gl_FrontMaterial.diffuse.xyz ;
    vec3 diffuse = kd * fr_eval  
                 * (gl_LightSource[0].diffuse.xyz * attenuation * meshColor) ;
 
    vec3 color = ambientAndEmision + diffuse  ;   	
    gl_FragColor  = vec4(color, 1.0);

}

