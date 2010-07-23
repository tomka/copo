uniform sampler2D tex;

varying vec3 lightVec;
varying vec3 normVec;
varying vec3 viewVec;
varying float dist;
varying vec3 vertexPos;
varying vec2 tex_coords;

void main()
{
	vec4 color = texture2D(tex, tex_coords);
	gl_FragColor = color;
}
