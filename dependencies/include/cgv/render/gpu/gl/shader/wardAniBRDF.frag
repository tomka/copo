/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
-------------------------------------------------------*/

uniform float ks;
uniform float kd;
uniform float roughnessX ;
uniform float roughnessY ;

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

   //First Term
   float cosTheta = dot (N, L) ;
   float cosDelta = dot (N, E);
   float firstTerm = 1.0 / sqrt(cosTheta * cosDelta);
	
   //vec2 roughness = {0.9, 0.1};
	
   //Second Term
   float secondTerm = 1.0 / (12.56 * roughnessX * roughnessY);
	
   //Third Term
   vec3 direction = {0.0, 0.0, 1.0};
   vec3 X = normalize(cross(N, direction));
   vec3 Y = normalize(cross(N, X));

   float HdotX = dot(HalfVector, X);
   float HdotY = dot(HalfVector, Y);
   float HdotN = dot(HalfVector, N);
	
   float A = -2.0 * (pow((HdotX / roughnessX), 2.0) + pow((HdotY / roughnessY), 2.0));
   float B = 1.0 + HdotN;
   float thirdTerm = exp(A / B);	
	
   float irradiance = max(0.0, cosTheta);
   float specularTerm = firstTerm * secondTerm * thirdTerm;

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