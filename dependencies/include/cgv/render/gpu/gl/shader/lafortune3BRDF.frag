const int CoefPerLobe = 3;
const int NumCoef = 27 ; 

uniform float kd ;
uniform float ks ;
uniform float Coeff[NumCoef] ;
uniform float alpha ;

varying vec3 local_in ;
varying vec3 local_out ;
varying float attenuation ;

void main ()
{	
    vec3 local_x = normalize (local_in);  // renormalized 
    vec3 local_y = normalize (local_out); // renormalized 

    float x_y = local_x.x * local_y.x + local_x.y * local_y.y ; 
    float z   = local_x.z * local_y.z ;

    float exp, lobe;
    
    float r = 0.0, g = 0.0, b = 0.0;
    for ( int i = 0; i<NumCoef; i+=3*CoefPerLobe )
    {
       exp  = Coeff[i+2] ;
       lobe = -x_y * Coeff[i] + z * Coeff[i+1] ;
       r += pow( lobe, exp ) ;

       exp  = Coeff[i+CoefPerLobe+2] ;
       lobe = -x_y * Coeff[i+CoefPerLobe] + z * Coeff[i+CoefPerLobe+1] ;
       g += pow( lobe, exp ) ;

       exp  = Coeff[i+2*CoefPerLobe+2] ;
       lobe = -x_y * Coeff[i+2*CoefPerLobe] + z * Coeff[i+2*CoefPerLobe+1] ;
       b += pow( lobe, exp ) ;
    }
  
    vec3 ambientAndEmision =  vec3 (gl_FrontLightModelProduct.sceneColor) * attenuation ;
    vec3 diffuse = kd 
                 * (gl_LightSource[0].diffuse.xyz * attenuation * gl_FrontMaterial.diffuse.xyz) ;
                     
    vec3 specular = ks * vec3 (r, g, b)
                  * (gl_LightSource[0].specular.xyz * attenuation * gl_FrontMaterial.specular.xyz );
                 
    
    vec3 color = ambientAndEmision + diffuse + specular ;
    color = clamp( color, 0.0, 1.0 );
    gl_FragColor  = vec4(color, alpha);  

}

