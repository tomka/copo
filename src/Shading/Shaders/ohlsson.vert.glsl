//>	uniform float4x4 eyeViewMatrixIV,
//>	uniform float4x4 eyeViewMatrix,
//>	uniform float4x4 textureMatrix,
//>	uniform float4x4 modelViewMatrix,
//>	uniform float4x4 projectionMatrix,
//> out float4 oPosition   :  POSITION,    // Position in project space
//> out half4  oTexCoord   :  TEXCOORD0,   // Projective texCoord, for depth map
//> out half4  objectPos   :  TEXCOORD1,   // Object position in world space
//> out fixed3 oNormal     :  TEXCOORD2,   // Normal in world space
//> out fixed3 eyeDirection  : TEXCOORD3 ) // Eye coordinates (ie viewpoint)
//> 									   // in world space

uniform mat4 eyeViewMatrixIV;
uniform mat4 eyeViewMatrix;
uniform mat4 textureMatrix;
uniform mat4 projectionMatrix;

varying mat4 oPosition;
varying mat4 oTexCoord;
varying mat4 objectPos;
varying vec3 oNormal;
varying vec3 eyeDirection;


void main()
{
	// Commented lines starting with "\\>" are the original ohlsson CG code

	//> float4 position : POSITION,
	//> float3 normal   : NORMAL,
	vec4 position = gl_Vertex;
	vec3 normal = gl_Normal;

	// Create the object to world space matrix (i.e. model matrix)
	//> float4x4 modelMatrix = mul( eyeViewMatrixIV, modelViewMatrix );
	mat4 modelMatrix = matrixCompMult( eyeViewMatrixIV, gl_ModelViewMatrix );

	// Transform the object space position into world coordinats
	//> float4 objectWorldPos = mul( modelMatrix, position );
	mat4 objectWorldPos = matrixCompMult( modelMatrix, position );

	// Transform the object normal position into world coordinates
	//> oNormal = mul( modelMatrix, float4(normal, 0)).xyz;
	oNormal = matrixCompMult( modelMatrix, vec4(normal, 0.0)).xyz;

	// Take the eye position direction into world space for light
	//> calculation, store the direction
	//> eyeDirection = normalize(
	//>        mul(
	//>                eyeViewMatrixIV,
	//>                float4(0,0,0,1)
	//>        ).xyz - objectWorldPos.xyz
	//> );
	eyeDirection = normalize(
			matrixCompMult(
					eyeViewMatrixIV,
					vec4(0.0, 0.0, 0.0, 1.0)
			).xyz - objectWorldPos.xyz
		);

	// multiply the object position in world space by the texture matrix
	//> oTexCoord = mul( textureMatrix, objectWorldPos );
	//> objectPos = objectWorldPos;
	oTexCoord = matrixCompMult( textureMatrix, objectWorldPos );
	objectPos = objectWorldPos;

	//> float inc = max( dot( normal, float3(0, 1, 0)), 0);
	float inc = max( dot( normal, vec3(0.0, 1.0, 0.0)), 0.0);

	//> float raise = 0.04*smoothstep(0.7, 1, inc);
	float raise = 0.04 * smoothstep(0.7, 1.0, inc);

	//> oPosition = mul(
	//>        projectionMatrix,
	//>        mul(
	//>                eyeViewMatrix,
	//>                objectWorldPos + float4(0,1,0,0)*raise
	//>        )
	//> );
	oPosition = matrixCompMult(
		        projectionMatrix,
		        matrixCompMult(
		                eyeViewMatrix,
		                objectWorldPos + vec4(0.0, 1.0, 0.0, 0.0) * raise
		        )
		);

	//DEBUG: Transforming The Vertex
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
