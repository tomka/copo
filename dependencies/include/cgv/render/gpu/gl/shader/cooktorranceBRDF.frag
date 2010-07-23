/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
-------------------------------------------------------*/

uniform float kd ;
uniform float ks ; 
uniform float m ;          //constant for these shader
uniform float n ;          //index of refraction
uniform float invpi ;

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
    vec3 V = normalize (view);   //renormalized

    vec3 H = normalize (L + V);  //halfway-vector
    
    float N_dot_H = dot (N,H) ;
    float N_dot_V = dot (N,V) ;
    float N_dot_L = dot (N,L) ;
    float V_dot_H = dot (V,H) ;
    
    // Calculate geometric self-shadowing term
    float G = min(min(1.0,(2.0*N_dot_H*N_dot_V)/V_dot_H), (2.0*N_dot_H*N_dot_L)/V_dot_H);
    
    // Calculate distribution term D 
    float N_dot_H_2 = N_dot_H * N_dot_H;
    float sin2_alpha = 1.0 - N_dot_H_2;          // sin^2(a) = 1.0 - cos^2(a) 
    float tan2_alpha = sin2_alpha / N_dot_H_2;   // tan^2(a) = sin^2(a) / cos^2(a) 
    
    //... using Beckmann distribution function 
    float D = exp(-tan2_alpha/(m * m)) / (m * m * (N_dot_H_2 * N_dot_H_2));
    
    // Calculate fresnel term F 
    float c = V_dot_H;
    float g = sqrt(n * n + c * c - 1.0);
    float temp_term = ((g - c) * (g - c)) / ((g + c) * (g + c));
    float tt2 = (c * (g + c) - 1.0) / (c * (g - c) + 1.0);
    //is valid only for k = 0, but it's apparently pretty close no matter what k is, within physical reason.
    float F = 0.5 * temp_term * (1.0 + tt2 * tt2);
    
    //   Rs = specular reflectance, function of N, L, V, F, D, and G
    float Rs = (F * invpi) *  (D / N_dot_L) * (G / N_dot_V);
     
    vec3 ambientAndEmision =  vec3 (gl_FrontLightModelProduct.sceneColor) * attenuation ;
    vec3 meshColor = (useTexture == 1) ? vec3(texture2D (texture, gl_TexCoord[0].st)) : gl_FrontMaterial.diffuse.xyz ;
    vec3 diffuse = kd *  max (dot (N, L), 0.0)  
                 * (gl_LightSource[0].diffuse.xyz * attenuation * meshColor) ;
    vec3 specular = ks * Rs 
                  * (gl_LightSource[0].specular.xyz * attenuation * gl_FrontMaterial.specular.xyz );
                  
    
    vec3 color = ambientAndEmision + diffuse + specular ;
    color = clamp( color, 0.0, 1.0 );
    	
    gl_FragColor  = vec4(color, 1.0);

}

