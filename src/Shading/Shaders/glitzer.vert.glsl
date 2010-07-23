uniform vec3 lightPos;
uniform vec3 eyePos;

varying vec3 lightVec;
varying vec3 normVec;
varying vec3 viewVec;
varying float dist;
varying vec3 vertexPos;

varying vec2 tex_coords;

void main() {

	// tangent space nm
/*
	vec3 normal = gl_Normal * gl_NormalMatrix;
	mat3 tbn = mat3(tangent, cross(tangent, normal), normal);

	gl_TexCoord[0] = gl_MultiTexCoord0;

	//vec3 ecPosition = (gl_ModelViewMatrix * gl_Vertex).xyz;
	vec3 ecPosition = gl_Vertex.xyz;

	lightVec = normalize(lightPos - ecPosition);
	lightVec *= tbn;
	viewVec = normalize(-ecPosition);
	viewVec *= tbn;

	gl_Position = ftransform();
*/
	// copy texture cooridnates of texture unit 0 to texture unit 1
	// (glTexCoords saves only to gl_MultiTexCoord0 and we want to read from TEXTURE1)
	tex_coords = gl_MultiTexCoord1;

	dist = length(gl_Vertex.xyz - lightPos);

	vec3 ecPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	normVec = normalize(gl_NormalMatrix * gl_Normal);

	// bring light into eyespace
	lightVec = normalize((gl_ModelViewMatrix * vec4(lightPos, 1.0)).xyz - ecPosition);

	// egal
	viewVec = -ecPosition;
	//viewVec = normalize(cameraPos - (gl_ModelViewMatrix * gl_Vertex).xyz);

	// Transforming The Vertex
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	//gl_FrontColor = gl_Color;

	vertexPos = gl_Vertex.xyz;
}
