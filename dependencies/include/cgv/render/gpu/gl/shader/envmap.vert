/* ------------------------------------------------------
---    GLSL BRDF models
---    Rosana Montes [rosana@ugr.es]
---    Dresden stay - January 2007
---          pre-convolved environment maps to compute light color.
---          two versions of the environment map to accurately vary the light color.
-------------------------------------------------------*/

varying vec3 normal;
varying vec3 position;

void main(void)
{
    vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
    position = (vec3 (ecPosition)) / ecPosition.w;
    normal = gl_Normal;

    gl_FrontColor  = gl_FrontMaterial.diffuse ;
    gl_Position    = ftransform();
}
