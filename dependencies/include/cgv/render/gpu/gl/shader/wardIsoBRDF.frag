/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
-------------------------------------------------------*/

uniform float ks;
uniform float kd;
uniform float roughness ;

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

   vec3 HalfVector = normalize(L + E);
	
   //float roughness = 0.25;

   float tan2NH    = -pow(tan(acos(dot(N, HalfVector))), 2.0);
   float RMS2      = pow(roughness, 2.0);
   float Den       = 6.28 * RMS2;
   float firstTerm = exp(tan2NH / RMS2) / Den;
	
   float cosTheta = dot (N, L) ;
   float cosDelta = dot (N, E) ;
	
   float secondTerm = 1.0 / sqrt(cosTheta * cosDelta);
	
   float irradiance = max(0.0, cosTheta);
	
   float specularTerm = firstTerm * secondTerm;


   vec3 ambientAndEmision =  vec3 (gl_FrontLightModelProduct.sceneColor) * attenuation ;
   vec3 meshColor = (useTexture == 1) ? vec3(texture2D (texture, gl_TexCoord[0].st)) : gl_FrontMaterial.diffuse.xyz ;
   vec3 diffuse = kd *  irradiance
                * (gl_LightSource[0].diffuse.xyz * attenuation * meshColor) ;
   vec3 specular = ks * specularTerm 
                 * (gl_LightSource[0].specular.xyz * attenuation * gl_FrontMaterial.specular.xyz );
                  
   vec3 color = ambientAndEmision + diffuse + specular ;
   color = clamp( color, 0.0, 1.0 );

   gl_FragColor  = vec4(color, 1.0);

}