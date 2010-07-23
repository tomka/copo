/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
-------------------------------------------------------*/

uniform float kd ;
uniform float ks ;
uniform float sigma0;
uniform float tau ;
uniform float lambda ;
uniform float refraction ;
uniform float extinction ;
uniform float SQRT_PI;

uniform sampler2D texture;
uniform int useTexture ;

varying vec3 normal ;
varying vec3 light ;
varying vec3 view ;
varying float attenuation ;

//in Numerical Recipes
float erfc(in float d )
{
    float d1 = abs(d);
    float d2 = 1.0 / (1.0 + 0.5 * d1);
    
    float sum = (-d1 * d1 - 1.2655122299999999) 
             + d2 * (1.00002368 + d2 * (0.37409196 + d2 * (0.096784179999999997 
	         + d2 * (-0.18628806000000001 + d2 * (0.27886807000000002 + d2 * (-1.13520398 + d2 * (1.4885158700000001
	         + d2 * (-0.82215223000000004 + d2 * 0.17087277000000001)))))))); 
	float d3 = d2 * exp2( sum );
    if(d >= 0.0)
        return d3;
    else
        return 2.0 - d3;
}

float S(in float d)
{
    if(d > 2.2471164185778946E+27)
    {
        return 1.0;
    } else
    {
        float d1 = (tau * d) / (2.0 * sigma0);
        float d2 = erfc(d1);
        float d3 = 0.5 * (exp2(-(d1*d1)) / (SQRT_PI * d1) - d2);
        return (1.0 - 0.5 * d2) / (d3 + 1.0);
    }
}

float K(in float d)
{
    float d1 = 9.9999999999999995E-007;
    float d2 = 1000000000000.0;
    float d3 = d2;
    if(abs(d) < d1)   d3 = 0.0;
    else
       if(d < d2)
           d3 = d * erfc(tau / (2.0 * d * sigma0));
    return d3;
}


float sigma(in float d, in float d1)
{
    float val = 0.0;
    float d2 = sqrt(1.0 - (d*d)) / d;
    float d3 = sqrt(1.0 - (d1*d1)) / d1;
    float d4 = K(d2) + K(d3);
    if(d4 == 0.0)
        val = sigma0;
    else
    {
        float d5 = sqrt(0.039788735772973836) * d4;
        float d6 = d5 > 1.0 ? sqrt(2.0 * log2(d5)) : d5;
        int i = 0;
		do
		{
			if(i >= 100)
				 break;
			float d7 = exp2(0.5 * d6 * d6);
			float d8 = d6 * d7 - d5;
			float d9 = (1.0 + (d6*d6)) * d7;
			float d10 = d8 / d9;
			d6 -= d10;
			if(abs(d10) < 1E-008 && abs(d8) < 1E-008)
				break;
			 i++;
		 } while(true);
	     val = sigma0 / sqrt(1.0 + (d6*d6));
	 }
	 return val ; 
}

float g(in float d, in float d1)
{
    float value = ((6.2831853071795862 * sigma(d, d1) * (d + d1)) / lambda);
    return value * value ;
}


float fresnel( in float d, in float d1, in float d2)
{
     if(d1 == 0.0 && d2 == 0.0)
         return 1.0;
     else
     {
	 float d3 = d*d;
	 float d4 = 1.0 - d3;
	 float d5 = sqrt(d4);
	 float d6 = d4 / d;
	 float d7 = d6*d6;
	 float d8 = d1*d1;
	 float d9 = d2*d2;
	 float d10 = d8 - d9 - d4;
	 float d11 = sqrt( (d10*d10) + 4.0 * (d9*d9));
	 float d12 = 0.5 * (d11 + d10);
	 float d13 = 0.5 * (d11 - d10);
	 float d14 = sqrt(d12);
	 float d15 = d12 + d13 + d3;
	 float d16 = 2.0 * d14 * d;
	 float d17 = (d15 - d16) / (d15 + d16);
	 float d18 = d12 + d13 + d7;
	 float d19 = 2.0 * d14 * d6;
	 float d20 = (d17 * (d18 - d19)) / (d18 + d19);
	 return 0.5 * (d20 + d17);
     }
}

float D( in vec3 vector3d, in vec3 vector3d1, in float d, in int i)
{
     float d1 = 1.0 / lambda;
     vec3 vector3d2 = (vector3d1  - vector3d) * (6.2831853071795862 * d1);
     float d2 = tau * tau;
     float d3 = ((vector3d2.x * vector3d2.x) + (vector3d2.y * vector3d2.y)) * d2;
     float d4 = 1.0;
     float d5 = 0.0;
     float d6 = -1.0;
     float d7 = 0.0;
     int j = 1;
     do
     {
          if(j >= i || d7 <= d6 && d7 <= 9.9999999999999998E-013)
               break;
          float j2 = float(j);
          d4 *= d / j2;
          float d8 = exp2(-d - d3 /(4.0 * j2));
          d6 = d7;
          d7 = (d4 * d8) / j2;
          //if(float.isInfinite(d7))     break;
          
          d5 += d7;
          j++;
     } while(true);
     float d8 = 3.1415926535897931 * d1 ;
     return d2 * d8 * d8 * 0.25 * d5;
}

float G( in vec3 vector3d, in vec3 vector3d1)
{
     float d = lambda / 6.2831853071795862;
     vec3 vector3d2 = (vector3d1 - vector3d) * (1.0 / d);
     vec3 vector3d3 = cross(vector3d, normal);
     vec3 vector3d4 = cross(vector3d1, normal);
     float d1 = length(vector3d3);
     float d2 = length(vector3d4);
     if(d1 > 4.9406564584124654E-323)
         vector3d3 *= 1.0 / d1;
     else
        if(d2 > 4.9406564584124654E-323)
            vector3d3 = normalize(vector3d4);
        else
        {
            vector3d3 = vec3(0.0, 1.0, 0.0);
            vector3d4 = vec3(0.0, 1.0, 0.0);
        }
     if(d2 > 4.9406564584124654E-323)
         vector3d4 *= 1.0 / d2;
     else
         vector3d4 = vector3d3;
     vec3 vector3d5 = cross(vector3d3, vector3d);
     vec3 vector3d6 = cross(vector3d4, vector3d1);
     vec3 vector3d7 = cross(vector3d1, vector3d);

     float d3 = dot(vector3d4, vector3d);
     float d4 = dot(vector3d6, vector3d);
     float d5 = dot(vector3d3, vector3d1);
     float d6 = dot(vector3d5, vector3d1);
     float d7 = length(vector3d2);
     float d8 =  ((d * d7) / (vector3d2.z * length(vector3d7))) * ( ((d3*d3) + (d4*d4)) * ((d5*d5) + (d6*d6)));
     return d8 * d8 ;
}

float rho_sp(in float d, in float d1, in float d2, in float d3)
{
     float d4 = fresnel(d, d2, d3);
     float d5 = g(d, d);
     float d6 = S(d1);
     return (d4 * d6 * d6 * exp2(-d5)) / d;
}

float rho_dd(in vec3 vector3d, in vec3 vector3d1, in float d, in float d1)
{
     vec3 vector3d2 = vector3d1 - vector3d;
     float th  = 0.5 * length(vector3d2) ;
     float d2 = fresnel(th, d, d1);
     float d3 = G(vector3d, vector3d1);
     float d4 = -dot(vector3d, normal);
     float d5 = dot(vector3d1, normal);
     float d6;
     if(d4 != 1.0)
     {
         float d8 = d4 / sqrt(1.0 - d4 * d4);
         d6 = S(d8);
     } else
     {
         d6 = 1.0;
     }
     float d7;
     if(d5 != 1.0)
     {
         float d9 = d5 / sqrt(1.0 - d5 * d5);
         d7 = S(d9);
     } else
     {
         d7 = 1.0;
     }
     float d10 = d6 * d7;
     float d11 = g(d4, d5);
     float d12 = D(vector3d, vector3d1, d11, 0x7fffffff);
     return (d2 * d3 * d10 * d12) / (3.1415926535897931 * d4 * d5);
}



void main ()
{	
    vec3 N = normalize (normal); //renormalized		
    vec3 L = normalize (light);  //renormalized
    vec3 E = normalize (view);   //renormalized

    float cosine = dot(L,N);
    float normcos = cosine / sqrt(1.0 - cosine * cosine);
    float dd = rho_dd(L, E, refraction, extinction) ;
    float sp = rho_sp(cosine, normcos, refraction, extinction);
    

    vec3 meshColor = (useTexture == 1) ? vec3(texture2D (texture, gl_TexCoord[0].st)) : gl_FrontMaterial.diffuse.xyz ;
    vec3 diffuse = kd *  dd
                 * (gl_LightSource[0].diffuse.xyz * attenuation * meshColor) ;
    vec3 specular = ks * sp 
                     * (gl_LightSource[0].specular.xyz * attenuation * gl_FrontMaterial.specular.xyz );
                  
    vec3 color = diffuse + specular ;
    color = clamp( color, 0.0, 1.0 );
    gl_FragColor  = vec4(color, 1.0);

}

