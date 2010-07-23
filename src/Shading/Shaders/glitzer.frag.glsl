uniform sampler2D texNormal;

varying vec3 lightVec;
varying vec3 normVec;
varying vec3 viewVec;
varying float dist;
varying vec3 vertexPos;

void main() {
	// not sure
	vec3 normalT = normalize(texture2D(texNormal, vertexPos.xy + vertexPos.z).xyz);
	vec3 upT = vec3(0.0, 1.0, 0.0);

	vec3 normal = normalize(normVec);

	normal -= upT - normalT;

	
	vec3 eye = normalize(viewVec);
	vec3 light = normalize(lightVec);

	//vec3 reflectVec = reflect(-light, normal);
	vec3 halfVec = normalize(eye + light);

	// specular
	float specular = max(dot(halfVec, normal), 0.0);
	specular = pow(specular, 64.0);

	//diffuse
	//float attenuation = 1.0 / (dist * dist);
	float diffuse = max(dot(eye, normVec), 0.0);
	
	// ambient
	float ambient = 0.5;

	gl_FragColor = vec4(0.5, 0.5, 0.8, 1.0) * ambient + diffuse * vec4(0.3, 0.3, 0.9, 1.0) + specular * vec4(1.0, 1.0, 1.0, 1.0);
}