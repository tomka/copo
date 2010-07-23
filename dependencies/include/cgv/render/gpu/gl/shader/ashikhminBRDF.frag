/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
-------------------------------------------------------*/


uniform float ks ; 
uniform float nu ; 
uniform float nv ; 
uniform float normAshik ;
uniform float root ;

uniform sampler2D texture;
uniform int useTexture ;

varying vec3 normal ;
varying vec3 light ;
varying vec3 view ;
varying float attenuation ;

float evalG( in vec3 n, in vec3 h, in vec3 e, in vec3 l )
{
   float ndoth = abs( dot(n,h) );
   float ldotn = abs( dot(n,l) );
   float edotn = abs( dot(n,e) );
   float edoth = abs( dot(h,e) );
   
   return min(1.0, min( (2.0 * ndoth * edotn / edoth),
                        (2.0 * ndoth * ldotn / edoth)));
}

float evalD( in vec3 h )
{
   float costheta = abs( h.z );
   float exp = (nu * h.x * h.x + nv * h.y * h.y) / (1.0 - costheta * costheta);
   return sqrt( (nu+1.0)*(nv+1.0) ) * pow( costheta, exp );
}

void main ()
{	
    vec3 N = normalize (normal); // renormalized 
    vec3 L = normalize (light);  // renormalized 
    vec3 E = normalize (view);   // renormalized

    vec3 H = normalize (L + E);  // halfway
    float kdoth = abs( dot(H,E) ); 
    float ldotn = abs( dot(N,L) );
    float edotn = abs( dot(N,E) );
    
    /*float phiH = atan( H.y / H.x) ;
    float cosphi = cos(phiH) ;
    float sinphi = sin(phiH);
        
       
    float denom1  = max( max(L.z,E.z), 0.0 ) * kh;    
    float denom2  = max( 1.0-(kh*kh), 0.0 );
    float exponent= ((nu * cosphi * cosphi) + (nv * sinphi * sinphi)) / denom2 ;
    
    float fr_s =  ( root * pow (kh, exponent) * evalF ) / denom1 ;
    */

    float fr_d = normAshik
                        * ( 1.0 - pow( 1.0 - (ldotn*0.5) , 5.0 ) )
                        * ( 1.0 - pow( 1.0 - (edotn*0.5) , 5.0 ) );
                        
    float evalF = ks + (1.0 - ks) * pow ( (1.0-kdoth), 5.0 ) ;
    
    float fr_s =  evalD( H ) * evalG( N, H, E, L ) * evalF / ( 4.0 * ldotn * edotn );
    
    
    vec3 ambientAndEmision =  vec3 (gl_FrontLightModelProduct.sceneColor) * attenuation ;
    vec3 meshColor = (useTexture == 1) ? vec3(texture2D (texture, gl_TexCoord[0].st)) : gl_FrontMaterial.diffuse.xyz ;
    vec3 diffuse  = fr_d * (gl_LightSource[0].diffuse.xyz  * attenuation * meshColor) ;
    vec3 specular = fr_s * (gl_LightSource[0].specular.xyz * attenuation * gl_FrontMaterial.specular.xyz );
                  
    
    vec3 color = ambientAndEmision + diffuse + specular ;
    color = clamp( color, 0.0, 1.0 );        
	
    gl_FragColor  = vec4(color, 1.0) ;	

}

