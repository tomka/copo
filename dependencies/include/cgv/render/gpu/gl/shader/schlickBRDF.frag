/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
-------------------------------------------------------*/

uniform float ks ;
uniform float kd ;
uniform float power ;

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
   vec3 E = normalize (view);   // renormalized
   
  
   float irradiance = max(0.0, dot (L,N) );
	
   vec3 R = normalize(2.0 * N * dot(N, L) - L);
   float RdotV = max(dot(R, E), 0.0);
	
   float schlickTerm = RdotV / (power - (power * RdotV) + RdotV);
	
      
   vec3 ambientAndEmision =  vec3 (gl_FrontLightModelProduct.sceneColor) * attenuation ;
   vec3 meshColor = (useTexture == 1) ? vec3(texture2D (texture, gl_TexCoord[0].st)) : gl_FrontMaterial.diffuse.xyz ;
   vec3 diffuse = kd *  irradiance
                 * (gl_LightSource[0].diffuse.xyz * attenuation * meshColor) ;
   vec3 specular = ks * schlickTerm 
                  * (gl_LightSource[0].specular.xyz * attenuation * gl_FrontMaterial.specular.xyz );
                  
    
   vec3 color = ambientAndEmision + diffuse + specular ;
   color = clamp( color, 0.0, 1.0 );   

   gl_FragColor  = vec4(color, 1.0);

}

