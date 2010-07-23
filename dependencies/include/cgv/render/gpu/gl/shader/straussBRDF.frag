/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
-------------------------------------------------------*/


uniform float kj ;         //some constant
uniform float kf ;         //some constant
uniform float kg ;         //some constant
uniform float INV2PI ;


uniform float invKf2 ;     //precomputation on CPU and dependent of model parameters
uniform float invDiffKf ;  //   smoothness and metalness
uniform float inv1Kg2 ;
uniform float invDiffKg ; 
uniform float drd ;
uniform float rn ;
uniform float e ;
uniform float m1 ;

uniform sampler2D texture;
uniform int useTexture ;

varying vec3 normal ;
varying vec3 light ;
varying vec3 view ;
varying float attenuation ;


   //--------------------------------------------------------------

float EvalF( in float x) 
{
   float invXKf2 = 1.0 / ((x - kf)*(x - kf)) ;
   return (invXKf2 - invKf2) * invDiffKf ;
}

   //--------------------------------------------------------------

float EvalG( in float x) 
{
   float invXKg2 = 1.0 / ((x - kg)*(x - kg)) ;
   return (inv1Kg2 - invXKg2) * invDiffKg ;
}

   //--------------------------------------------------------------

void main ()
{	
    vec3 N = normalize (normal); //renormalized		
    vec3 L = normalize (light);  //renormalized
    vec3 E = normalize (view);   //renormalized

    vec3 R = reflect(-L, N);        
    float rdotv = dot ( R,E );

/*     A Way
    vec3 H = normalize (L + E);  //halfway
    float cosdelta = dot( L,H ) ;

    float j = EvalF(rdotv) * EvalG(rdotv) * EvalG(cosdelta) ;
    float rj = min(1.0, rn+(rn+kj)*j) ;
    float rs = pow(-cosdelta,e) * rj ;                  


*   but B way looks better
*/
    float theta_in = acos( L.z );
    float theta_out = acos( E.z );
    rdotv = clamp (rdotv, 0.0, 1.0 ) ;
	
    float t_in = theta_in* INV2PI;
    float t_out = theta_out * INV2PI;
	
	float d1 = t_in - kf;
    float F =   (1.0/(d1*d1) - invKf2 ) * invDiffKf ;
	
	float d2 = t_out - kg ;
    float G =   (inv1Kg2 - 1.0/(d2*d2)) * invDiffKg;
	
    float sa = min(1.0, rn+(rn+kj)*F*G);
    float sc = m1 *(1.0-F);
    float rs = sa*sc * pow( rdotv, e ) / E.z ;                  
                  

    vec3 ambientAndEmision =  vec3 (gl_FrontLightModelProduct.sceneColor) * attenuation ;
    vec3 meshColor = (useTexture == 1) ? vec3(texture2D (texture, gl_TexCoord[0].st)) : gl_FrontMaterial.diffuse.xyz ;
    vec3 diffuse = drd * (gl_LightSource[0].diffuse.xyz * attenuation * meshColor) ;
    vec3 specular = rs * (gl_LightSource[0].specular.xyz * attenuation * gl_FrontMaterial.specular.xyz );
       
    vec3 color = ambientAndEmision + diffuse + specular ;  
    color = clamp ( color, 0.0, 1.0 );           
    
    gl_FragColor  = vec4( color , 1.0);

}

