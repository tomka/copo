/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
-------------------------------------------------------*/

uniform float A ;
uniform float B ;
uniform float kd ;

uniform sampler2D texture;
uniform int useTexture ;

varying vec3 normal ;
varying vec3 light ;
varying vec3 view ;
varying float attenuation ;


float FillOrenNayar( in vec2 dotProducts )
{
   float VdotN = dotProducts.x ; // 2.0 * dotProducts.x - 1.0 ;
   float LdotN = dotProducts.y ; // 2.0 * dotProducts.y - 1.0 ;
   float angleV = acos( VdotN ) ;
   float angleL = acos( LdotN ) ;
   float alpha = max( angleV, angleL );
   float beta = min( angleV, angleL );
   return sin(alpha) * tan(beta) ;
}


void main ()
{	
   vec3 N = normalize (normal); // renormalized 
   vec3 L = normalize (light);  // renormalized 
   vec3 E = normalize (view);   //renormalized
   
   vec2 dotProducts ;
   dotProducts.x = dot (E,N) ;
   dotProducts.y = dot (L,N) ;
   float Irradiance = max(0.0, dotProducts.y);
	
   float AngleDifference = max (0.0, dot(normalize(E - N * dotProducts.x), normalize(L - N * dotProducts.y)));
	                             
   float AngleTerm = FillOrenNayar( dotProducts );
		

   float eval = (A + B * AngleDifference * AngleTerm) * Irradiance;

   vec3 ambientAndEmision =  vec3 (gl_FrontLightModelProduct.sceneColor) * attenuation ;
   vec3 meshColor = (useTexture == 1) ? vec3(texture2D (texture, gl_TexCoord[0].st)) : gl_FrontMaterial.diffuse.xyz ;
   vec3 diffuse = kd * eval 
                * (gl_LightSource[0].diffuse.xyz * attenuation * meshColor) ;
    
   vec3 color = ambientAndEmision + diffuse ;	
   color = clamp( color , 0.0, 1.0 );    

   gl_FragColor  = vec4(color, 1.0);

}

