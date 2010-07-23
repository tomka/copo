/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
-------------------------------------------------------*/

uniform float kd ;
uniform float ks ; 
uniform float kv ; 
uniform float invomega ;
uniform float invtau ;
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
    vec3 E = normalize (view);   // renormalized

    vec3 H = normalize((L+E)*0.5);
    float alfa  = dot( L, H);
    float cbeta = H.z;
    float beta  = acos( cbeta );
    
    float invin  = 1.0 / L.z;
    float invout = 1.0 / E.z;
        
    float fresnel = ks + (1.0 - ks) * pow ( (1.0-cbeta), 5.0 ) ;
    float SO = ( 1.0 + beta * invomega * exp(-2.0*alfa) * invtau ) 
             / ( 1.0 + beta * invomega) ;

    float fr_d = invpi * kd * max (dot (N, L), 0.0) ;
    float fr_s = invpi * ks * alfa * beta * beta * SO * fresnel * invin * invout ;
    float fr_v = invpi * kv * ( 2.0 * kv / max(L.z+E.z, 0.0) );
    
    
    vec3 ambientAndEmision = vec3 (gl_FrontLightModelProduct.sceneColor) * attenuation ;
    vec3 meshColor = (useTexture == 1) ? vec3(texture2D (texture, gl_TexCoord[0].st)) : gl_FrontMaterial.diffuse.xyz ;
    vec3 diffuse = (fr_d + fr_v) 
                 * (gl_LightSource[0].diffuse.xyz * attenuation * meshColor) ;
    vec3 specular = fr_s 
                  * (gl_LightSource[0].specular.xyz * attenuation * gl_FrontMaterial.specular.xyz );
                  
    
    vec3 color = ambientAndEmision + diffuse + specular ;
    color = clamp( color, 0.0, 1.0 );
    
    gl_FragColor  = vec4(color, 1.0);

}

