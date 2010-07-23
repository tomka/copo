/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
---          pre-convolved environment maps to compute light color.
---          two versions of the environment map to accurately vary the light color.
-------------------------------------------------------*/

varying vec3 normal;
varying vec3 position;

uniform float ks;
uniform float kd;

uniform samplerCube diffuseEnvmap;
uniform samplerCube specularEnvmap;

void main(void)
{
   vec3 N = normalize(gl_NormalMatrix * normal);

    vec3 tCoord = reflect(normalize(position), N);
    tCoord = vec3(gl_TextureMatrix[0] * vec4(tCoord, 1.0));
    vec3 specularColor = vec3(textureCube(specularEnvmap, tCoord));

    tCoord = N;
    tCoord = vec3(gl_TextureMatrix[0] * vec4(tCoord, 1.0));
    vec3 diffuseColor = vec3(textureCube(diffuseEnvmap, tCoord));

    vec3 color = ( kd * diffuseColor + ks * specularColor) * gl_Color.rgb;

    gl_FragColor = vec4(color, gl_Color.a);
}