uniform int pointLightOn;
uniform int spotLightOn;
uniform int dirLightOn;

// varyings pour le calcul de l'éclairage par fragments
varying vec3 fragNormal;
varying vec3 fragLight0Vect;
varying vec3 fragLight1Vect;
varying vec3 fragLight2Vect;


vec3 fnormal(void)
{
    //Compute the normal 
    vec3 normal = gl_NormalMatrix * gl_Normal;
    normal = normalize(normal);
    return normal;
}


void ftexgen()
{
   // pour le mappage correct du gazon
   gl_TexCoord[0] = gl_MultiTexCoord0;
}


void main (void)
{
    // Eye-coordinate position of vertex, needed in various calculations
    vec3 ecPosition3;
    vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;

    // Do fixed functionality vertex transform
    ecPosition3 = (vec3 (ecPosition)) / ecPosition.w;
    fragLight0Vect = vec3 (gl_LightSource[0].position) - ecPosition3;
    fragLight1Vect = vec3 (gl_LightSource[1].position) - ecPosition3;
    fragLight2Vect = vec3 (gl_LightSource[2].position) - ecPosition3;
    fragNormal = fnormal();
    ftexgen(); 
    gl_Position = ftransform();
}
