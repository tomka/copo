/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
-------------------------------------------------------*/

uniform float d ;
uniform float h ;
uniform float kd ;
uniform float ks ;
uniform float exp ;

const int subdiv = 0 ;  //min value 4, normal value 16

const float PI = 3.14159265 ;
const float PIMED = 1.57079 ;

uniform sampler2D texture;
uniform int useTexture ;

varying vec3 normal ;
varying vec3 light ;
varying vec3 view ;
varying float attenuation ;


float SinTheta(in vec3 w) 
{
   float s = sqrt(max(0.0, 1.0 - w.z*w.z));
   if (w.z < 0.0) s *= -1.0;
   return s;
}

float SinPhi(in vec3 w) 
{
   return w.y / SinTheta(w);
}

void main ()
{
   vec3 N = normalize (normal); //renormalized		
   vec3 I = normalize (light);  //renormalized
   vec3 O = normalize (view);   //renormalized
   vec3 H = normalize (I + O);  //halfway
   
   
   float cosThIn  = abs(dot( N,I ));
   float cosThOut = abs(dot( N,O ));
 
   float iy = SinTheta( I ) * SinPhi( I );   
   float oy = SinTheta( O ) * SinPhi( O );   
   float iy2 = iy * iy;
   float oy2 = oy * oy;
      
   //angle at what cylinder could start to vary
   float phi_d = (d < 2.0) ? asin(d/2.0) : PIMED ;
   float phi_h = acos( h );//antiguo H.z
   float thM = min(phi_d, phi_h);

   //floor_lengh computation
   float sqrt1h2 = sqrt(1.0 - h*h) ;
   float f = max( d-2.0 * sqrt1h2,0.0 );

   //projected light vector angle and view vector angle in BN plane
   //--------------------------------------------------------------
   //this part of the code is giving me problems. I have 3 implementations
   // Case 1
   float thL = acos( sqrt(1.0/(iy2/(cosThIn*cosThIn)+1.0)) );
   float thE = acos( sqrt(1.0/(oy2/(cosThOut*cosThOut)+1.0)) );
   // Case 2 : wrong
   //..............................................................
   //float thL = acos(I.z/sqrt((I.y*I.y)+(I.z*I.z)));
   //float thE = acos(O.z/sqrt((O.y*O.y)+(O.z*O.z)));
   // Case 3 : dependent on the tangent space 
   //..............................................................
   //vec3 L = vec3(0.0, I.y, I.z); float thL = acos( dot(L,N) );
   //vec3 E = vec3(0.0, O.y, O.z); float thE = acos( dot(E,N) );   
   //..............................................................
   

   //self-shadowing angle (as opposite of \theta_L) from L viewing
   float thSS = PIMED - thL;

   float thS,fi,fs;
   float cosThL = cos(thL);
   if (thSS > thM) {
      thS = thSS = thM;
      fi = f;      //length of the floor -> lit
      fs = 0.0;    //length of the floor -> shadow
   } else {
      if ((1.0/cosThL) < (d-1.0))
         thS = PIMED;
      else
         thS = thL + asin( d*cosThL - 1.0 );

      thS = min( thS,thM );
      fs = min( 1.0/cosThL - sqrt1h2 - h*tan(thL), f );
      fs = max( fs,0.0 );
      fi = f - fs;
   }

   //idem for the self-hiding angle from \theta_E
   float thSH = PIMED - thE;

   float thH,fv,fh;
   float cosThE = cos(thE);
   if (thSH > thM) {
      thH = thSH = thM;
      fv = f;      //length of the floor -> visible
      fh = 0.0;    //length of the floor -> hide
   } else {
      if ((1.0/cosThE) < (d-1.0))
         thH = PIMED;
      else
         thH = thE + asin( d*cosThE - 1.0 );
  	    
      thH = min( thH,thM );
      fh = min( 1.0/cosThE - sqrt1h2 - h*tan(thE), f );
      fh = max( fh,0.0 );
      fv = f - fh;
   }

   //compute lenght of the cylinders visible (lv) and visible-illuminated (lvi) after projected on plane
   //using arc visible and illuminated thl, the
   float thl,the,lv,lvi,fvi,phE,phs,phi;
   lv = (sin(thH-thE) + sin(thSH+thE)) / cosThE ;
   
   //test L, E in the same side of N... or not
   if ((iy > 0.0) == (oy > 0.0)) 
   {
      thl = min(thS,thH);
      the = min(thSS,thSH);
      lvi = (sin(thl-thE) + sin(the+thE)) / cos(thE);
      fvi = min(fv,fi);
      phE = PIMED - thE;
   } else {
      thl = min(thS,thSH);
      the = min(thH,thSS);
      lvi = (sin(the-thE) + sin(thl+thE))/ cos(thE);
      fvi = max(f-fh-fs,0.0);
      phE = PIMED + thE;
   }

   phi = PIMED - thl;
   phs = PIMED + the;
	
   //vector L in projected plane has component (Ln, Lb, Lt)
   float Ln = cosThIn ;//I.z, 
   float Lb = abs( iy );


   //approx diffuse term (N * L) integral := 2 term / correction factor
   //--------------------------------------------------------------------------
   float diffPh =  phs - phi ;
   float diffterm1 = sin(phE)*diffPh + 0.5*(cos(phE-phi-phi) - cos(phE-phs-phs));
   float diffterm2 = cos(phE)*diffPh + 0.5*(sin(phE-phi-phi) - sin(phE-phs-phs));
   float correction = sin(phE-phi)-sin(phE-phs);

   /* as in paper but no good results
   float sin2Phs = sin(phs) * sin(phs) ;
   float sin2Phi = sin(phi) * sin(phi) ;
   float common1 = sin2Phs - sin2Phi ;
   float common2 = phs - phi - sin(phs)*cos(phs) + sin(phi)*cos(phi) ;
   float diffterm1 = ( cos(phE) * common1 ) + ( sin(phE) * common2 ) ;
   float diffterm2 = ( sin(phE) * common1 ) + ( cos(phE) * common2 ) ;
   */

   float diffcyl = ( (Ln * 0.5 * diffterm1) + (Lb * 0.5 * diffterm2) ) / correction;
   float difffloor = Ln;

   float totaldiff = kd * (lvi*diffcyl + fvi*difffloor) / (lv+fv);



   //approx specular term with (N*H)^exp integral 
   //--------------------------------------------------------------------------
   //sampled normal N´ in projected plane has component (sin phi, cos phi, 0)
   float Hn = abs( dot( H,N ));//H.z, 
   float Hb = (iy > 0.0) ? H.y : -H.y;
   float sinPhE = sin(phE);
   float cosPhE = cos(phE);

   //sample cylinder surface and get \theta_{p_i}.    
   float sd = max(2.0, sqrt(exp)/1.2);
   sd *= max(0.2,phs-phi);   
   //int subdiv = int (max(2.0,sd)); //The number of subdivision must be constant for some graphics cards!!!!!
   //subdiv*=2;
   
   float dph = diffPh / float(subdiv);
   float sdph = sin(dph), cdph = cos(dph);
   float sph = sin(phi), cph = cos(phi);
   float s = 0.0, sc = 0.0;

   int i;
   bool impar = true ;
   float coeff, NdotH, NdotL, nsph, ncph ;
   for (i=0; i<=subdiv; i++) 
   {
      if ((i==0) || (i==subdiv))    coeff = 1.0;
      sc+=coeff;
      NdotH = Hn*sph + Hb*cph;//update
      NdotL = Ln*sph + Lb*cph;//update
      s += coeff * (sph*sinPhE + cph*cosPhE) * NdotL * NdotH / (exp-exp * NdotH+NdotH);
      nsph = sdph * cph + cdph * sph;//update
      ncph = cdph * cph - sdph * sph;//update
      sph = nsph; 
      cph = ncph;      
      if (impar) 
      { 
         coeff = 4.0;
         impar = false;
      }
      else 
      {
         coeff = 2.0;
         impar = true;
      }
   }

   float speccyl = s/sc * (phs-phi) / correction;
   float specfloor = Ln*Hn / (exp-exp*Hn+Hn);
   float totalspec = ks * (lvi*speccyl+fvi*specfloor) / (lv+fv);

   totaldiff = totaldiff / cosThIn ;
   totalspec = totalspec / cosThIn ;
   totalspec = clamp( totalspec, 0.0, 1.0 );
   
 	
   vec3 ambientAndEmision =  vec3 (gl_FrontLightModelProduct.sceneColor) * attenuation ;
   vec3 meshColor = (useTexture == 1) ? vec3(texture2D (texture, gl_TexCoord[0].st)) : gl_FrontMaterial.diffuse.xyz ;
   vec3 diffuse = totaldiff * (gl_LightSource[0].diffuse.xyz * attenuation * meshColor) ;
   vec3 specular = totalspec * (gl_LightSource[0].specular.xyz * attenuation * gl_FrontMaterial.specular.xyz );    
   
   vec3 color = ambientAndEmision + diffuse + specular ;	
   color = clamp( color , 0.0, 1.0 );  
           
   gl_FragColor  = vec4(totalspec,totalspec,totalspec , 1.0);
   
}

