/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
-------------------------------------------------------*/

uniform float ks ;
uniform float kd ; 
uniform float ro ;
uniform float rm ; 
uniform float k  ;

uniform sampler2D texture;
uniform int useTexture ;

varying vec3 normal ;
varying vec3 light ;
varying vec3 view ;
varying float attenuation ;

float SphereDirac(float cosine)
{
    if (abs(cosine-1.0)>1e-5) 
        return 0.0;
    else 
        return 1.0;
}	

void main ()
{	
    vec3 N = normalize (normal); // renormalized 
    vec3 L = normalize (light);
    vec3 E = normalize (view);
    
    vec3 R = reflect(-L, N);

    float term1 = 1.0 - dot( N, L );
    float term2 = 1.0 - dot( N, E );
    float cosine = dot( R, E) ;
    float fresnel = ro + (1.0-ro) * pow(term1, 5.0) ;
    
    float reflected = fresnel * SphereDirac( cosine );
    float refracted = k * rm * ( 1.0-pow(term1,5.0) ) * ( 1.0-pow(term2,5.0) );

    float fr = abs( reflected + refracted );
	
	vec3 ambientAndEmision =  vec3( gl_FrontMaterial.ambient * gl_LightSource[0].ambient );
	
	vec3 meshColor = (useTexture == 1) ? vec3(texture2D (texture, gl_TexCoord[0].st)) : gl_FrontMaterial.diffuse.xyz ;
    vec3 diffuse = kd * max (dot (N, L), 0.0)
                 * (gl_LightSource[0].diffuse.xyz * attenuation * meshColor) ;                  
                     
    vec3 specular = ks * fr 
                  * gl_LightSource[0].specular.xyz * attenuation * gl_FrontMaterial.specular.xyz ;   
    
    vec3 color = ambientAndEmision + diffuse + specular ;
    color = clamp( color, 0.0, 1.0 );
    gl_FragColor  = vec4(color, 1.0); 

}

