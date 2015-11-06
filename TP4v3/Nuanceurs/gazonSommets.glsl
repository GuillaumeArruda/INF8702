uniform int pointLightOn;
uniform int dirLightOn;
uniform int spotLightOn;
uniform int shadowMapOn;

// Les coordonn�es transform�es en espace projectif d'�clairage
// du sommet consid�r�, coordonn�es qui seront interpol�es � 
// travers la primitive, puis pass�es au fragments correspondants.
varying vec4 ShadowCoordLight0;
varying vec4 ShadowCoordLight1;
varying vec4 ShadowCoordLight2;

// varyings pour le calcul de l'�clairage par fragments
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


float ffog(in float ecDistance)
{
    return(abs(ecDistance));
}


vec4 calculerCoordProjectives(in int IDTextureMatrix)
{
    vec4 shadowCoordLight;

    // TODO :
    // calculer les coordonn�es du sommet en espace projectif d'�clairage 
    // � l'aide de la matrice projective indiqu�e en argument. Mettre
    // le r�sultat dans shadowCoordLight qui sera retourn� et lanc� vers
    // le nuanceur de fragments.
    // ... 
    // ...

    return (shadowCoordLight);
}


void main (void)
{
    // Eye-coordinate position of vertex, needed in various calculations
    vec3 ecPosition3;
    vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;

    // le calcul du fog doit prendre en compte la nouvelle coordonn�e
    gl_FogFragCoord = ffog((gl_ModelViewMatrix * gl_Vertex).z);

    // g�n�rer les coordonn�es de texture
    ftexgen();

    // Calculer les coordonn�es du sommet en espace projectif d'�clairage
    if(shadowMapOn == 1) {
	if (pointLightOn == 1) {
	    ShadowCoordLight0 = calculerCoordProjectives(1);
	}
	if (dirLightOn == 1) {
	    ShadowCoordLight1 = calculerCoordProjectives(2);
	}
	if (spotLightOn == 1) {
	    ShadowCoordLight2 = calculerCoordProjectives(3);
	}
    }

    // Do fixed functionality vertex transform
    ecPosition3 = (vec3 (ecPosition)) / ecPosition.w;
    fragLight0Vect = vec3 (gl_LightSource[0].position) - ecPosition3;
    fragLight1Vect = vec3 (gl_LightSource[1].position) - ecPosition3;
    fragLight2Vect = vec3 (gl_LightSource[2].position) - ecPosition3;
    fragNormal = fnormal();

    gl_Position = ftransform();
}
