uniform vec3 lightPosition ;

varying float attenuation ;
varying vec3 local_in, local_out;  // Unit vector in "u" and "v" directions 


void ReferenceSystem(in vec3 vZ, out vec3 local_x, out vec3 local_y)
{
   vec3 otro = vec3(0.0,1.0,0.0) ;
   vec3 _vX = cross(otro, vZ) ;
   
   if ( length(_vX) < 1.0e-5 )
   {
      otro = vec3(1.0,0.0,0.0) ;
      _vX = cross(otro,vZ) ;
   } 
   vec3 vX  = normalize( _vX );
   vec3 _vY = cross(vZ,vX) ; 
   vec3 vY  = normalize( _vY );

   local_x = vec3( dot(local_x,vX), dot(local_x,vY), dot(local_x,vZ) );
   local_y = vec3( dot(local_y,vX), dot(local_y,vY), dot(local_y,vZ) );
}

void main ()
{
    vec3 normal   = gl_NormalMatrix * gl_Normal;
    vec3 position = (gl_ModelViewMatrix * gl_Vertex).xyz;
    vec3 view     = normalize(vec3 (-position));

    vec3 light   = vec3(gl_LightSource[0].position) - position;
    float distance = length(light);
    light = normalize(light);

    attenuation = 1.00 / (gl_LightSource[0].constantAttenuation +
 			gl_LightSource[0].linearAttenuation * distance +
			gl_LightSource[0].quadraticAttenuation * distance * distance);    

    local_in  = light ; // world vector 
    local_out = view  ; // world vector 
    ReferenceSystem( normal, local_in, local_out ); // WorldToLocal transformation

    gl_Position = ftransform ();
}

