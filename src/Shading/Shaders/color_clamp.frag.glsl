varying float xpos;
varying float ypos;
varying float zpos;

void main (void)
{
   gl_FragColor = vec4 (xpos, ypos, zpos, 1.0);
}
