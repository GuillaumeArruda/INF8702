uniform sampler2D colorMap;

varying vec3 transformedNormal;
varying vec3 eyePosition;
varying vec3 positionVertex;
void ftexgen()
{
   // pour le mappage correct du gazon
   gl_TexCoord[0] = gl_MultiTexCoord0;
}

vec3 fnormal(void)
{
    // Calculer la normale
    vec3 normal = gl_NormalMatrix * gl_Normal;
    normal = normalize(normal);
    return normal;
}

void main (void)
{
    gl_Position = ftransform();
    transformedNormal = fnormal();
	eyePosition = -gl_Position.xyz;
	positionVertex = gl_ModelViewMatrix * gl_Vertex;
    ftexgen();
}
