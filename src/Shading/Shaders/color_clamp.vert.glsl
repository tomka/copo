varying float xpos;
varying float ypos;
varying float zpos;

void main(void)
{
	vec4 position = gl_Vertex;
	position = normalize(position);
	
  xpos = clamp(position.x,0.0,1.0);
  ypos = clamp(position.y,0.0,1.0);
  zpos = clamp(position.z,0.0,1.0);


  gl_Position = gl_ModelViewProjectionMatrix * position;
}