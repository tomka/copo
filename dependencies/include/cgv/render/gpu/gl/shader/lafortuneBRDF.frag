/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
-------------------------------------------------------*/

uniform float kd ;
uniform float ks ;
uniform float exp ;
uniform float Cxy ;
uniform float Cz ;
uniform float normFactor ;

uniform sampler2D texture;
uniform int useTexture ;

varying vec3 normal ;
varying vec3 light ;
varying vec3 view ;
varying float attenuation ;

void main ()
{	
    vec3 local_x = normalize (light);  // renormalized 
    vec3 local_y = normalize (view); // renormalized 

    float x_y = local_x.x * local_y.x + local_x.y * local_y.y ; 
    float z   = local_x.z * local_y.z ;
       
    float lobe = -x_y * Cxy + z * Cz ;
    float fr = pow( lobe, exp ) * normFactor ;

 
    vec3 ambientAndEmision =  vec3 (gl_FrontLightModelProduct.sceneColor) * attenuation ;
    vec3 meshColor = (useTexture == 1) ? vec3(texture2D (texture, gl_TexCoord[0].st)) : gl_FrontMaterial.diffuse.xyz ;
    vec3 diffuse = kd * max (dot (normal, light), 0.0)  
                 * (gl_LightSource[0].diffuse.xyz * attenuation * meshColor) ;
                     
    vec3 specular = ks * fr
                  * (gl_LightSource[0].specular.xyz * attenuation * gl_FrontMaterial.specular.xyz );
                 
    
    vec3 color = ambientAndEmision + diffuse + specular ;
    color = clamp( color, 0.0, 1.0 );
    gl_FragColor  = vec4(color, 1.0);  

}

