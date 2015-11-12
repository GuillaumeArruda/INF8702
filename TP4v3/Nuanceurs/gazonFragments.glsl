uniform sampler2D colorMap;

// Les trois cartes d'ombrage � utiliser pour chacune
// des trois lumi�res
uniform sampler2DShadow shadowMap1;
uniform sampler2DShadow shadowMap2;
uniform sampler2DShadow shadowMap3;

uniform int pointLightOn;
uniform int spotLightOn;
uniform int dirLightOn;
uniform int fogOn;
uniform int shadowMapOn;

// Les trois coordonn�es de texture transform�es.
// pour chacune des trois lumieres, cette coordonn�e (s,t)
// de texture indique o� dans la carte d'ombrage corres
// pondante on doit �chantillonner la "distance" du fragment
// courant � la lumi�re en question.
in vec4 ShadowCoordLight0;
in vec4 ShadowCoordLight1;
in vec4 ShadowCoordLight2;

// varyings pour le calcul de l'�clairage par fragments
in vec3 fragNormal;
in vec3 fragLight0Vect;
in vec3 fragLight1Vect;
in vec3 fragLight2Vect;
// Accumulateurs pour les facteurs d'�clairage
vec4 Ambient;
vec4 Diffuse;


// Calcul pour une lumi�re ponctuelle
void pointLight(in vec3 lightVect)
{
    float nDotVP;       // Produit scalaire entre VP et la normale
    float attenuation;  // facteur d'att�nuation calcul�
    float d;            // distance entre lumi�re et fragment
    vec3  VP;           // Vecteur lumi�re

    // Calculer vecteur lumi�re
    VP = lightVect;

    // Calculer distance � la lumi�re
    d = length(VP);

    // Normaliser VP
    VP = normalize(VP);

    // Calculer l'att�nuation due � la distance
    attenuation = 1.0 / (gl_LightSource[0].constantAttenuation +
	gl_LightSource[0].linearAttenuation * d +
	gl_LightSource[0].quadraticAttenuation * d * d);

    nDotVP = max(0.0, dot(fragNormal, VP));

    // Calculer les contributions ambiantes et diffuses
    Ambient  += gl_LightSource[0].ambient * attenuation;
    Diffuse  += gl_LightSource[0].diffuse * nDotVP * attenuation;
}


// Calcul pour une lumi�re directionnelle
void directionalLight(in vec3 lightVect)
{
    float nDotVP;         // Produit scalaire entre VP et la normale

    nDotVP = max(0.0, dot(fragNormal, normalize(vec3 (gl_LightSource[1].position))));

    // Calculer les contributions ambiantes et diffuses
    Ambient  += gl_LightSource[1].ambient;
    Diffuse  += gl_LightSource[1].diffuse * nDotVP;
}


// Calcul pour une lumi�re "spot"
void spotLight(in vec3 lightVect)
{
    float nDotVP;            // Produit scalaire entre VP et la normale
    float spotDot;           // Cosinus angle entre VP et direction spot
    float spotAttenuation;   // Facteur d'att�nuation du spot
    float attenuation;       // Facteur d'att�nuation du � la distance
    float d;                 // Distance � la lumi�re
    vec3  VP;                // Vecteur lumi�re

    // Calculer le vecteur Lumi�re
    VP = lightVect;

    // Calculer la distance � al lumi�re
    d = length(VP);

    // Normaliser VP
    VP = normalize(VP);

    // Calculer l'att�nuation due � la distance
    attenuation = 1.0 / (gl_LightSource[2].constantAttenuation +
	gl_LightSource[2].linearAttenuation * d +
	gl_LightSource[2].quadraticAttenuation * d * d);

    // Le fragment est-il � l'int�rieur du c�ne de lumi�re ?
    spotDot = dot(-VP, normalize(gl_LightSource[2].spotDirection));

    if (spotDot < gl_LightSource[2].spotCosCutoff)
    {
	spotAttenuation = 0.0; // en dehors... aucune contribution
    }
    else
    {
	spotAttenuation = pow(spotDot, gl_LightSource[1].spotExponent);

    }

    // Combine les att�nuation du spot et de la distance
    attenuation *= spotAttenuation;

    nDotVP = max(0.0, dot(fragNormal, VP));

    // Calculer les contributions ambiantes et diffuses
    Ambient  += gl_LightSource[2].ambient * attenuation;
    Diffuse  += gl_LightSource[2].diffuse * nDotVP * attenuation;
}


vec4 flight(in vec3 normal)
{
    vec4 color;
    vec3 ecPosition3;

    // R�initialiser les accumulateurs
    Ambient  = vec4 (0.0);
    Diffuse  = vec4 (0.0);

    if (pointLightOn == 1) {
	pointLight(fragLight0Vect);
    }

    if (dirLightOn == 1) {
	directionalLight(fragLight1Vect);
    }

    if (spotLightOn == 1) {
	spotLight(fragLight2Vect);
    }

    color = (gl_FrontLightModelProduct.sceneColor +
	Ambient  * gl_FrontMaterial.ambient +
	Diffuse  * gl_FrontMaterial.diffuse);
    color = clamp( color, 0.0, 1.0 );
    return color;
}


// lecture de la depthmap et prise de d�cision pour l'ombrage du fragment
float calculerOmbrage(sampler2DShadow shadowMap, vec4 shadowCoordLight)
{
    float facteurOmbrage = 1.0;
    // TODO:
    // Compl�tez ici la fonction pour comparer la distance frag-lumiere 
    // avec profondeur lue dans la carte afin de prendre une d�cision sur
    // l'ombrage... et renvoyez le facteur multiplicatif dans l'un ou l'autre
    // des cas.
	if(shadowCoordLight.z / shadowCoordLight.w > shadow2DProj(shadowMap,shadowCoordLight).r)
	{
		facteurOmbrage = 0.2;
	}
    return (facteurOmbrage);
}


void main (void) 
{   
    vec4 color;
    // color = gl_Color;  // Il est tr�s important de ne pas obtenir la couleur provenant du NS.
    // Car on ne fait plus aucun calculs dans le NS !

    // �chantilloner la couleur du gazon dans la texture colorMap
    color = texture(colorMap, gl_TexCoord[0].xy);
    color *= flight(fragNormal);

    // modulation finale de la couleur avec du brouillard
    if (fogOn == 1) {
	float fog;

	fog = (gl_Fog.end - gl_FogFragCoord) * gl_Fog.scale;
	fog = clamp(fog, 0.0, 1.0);
	color = vec4(mix( vec3(gl_Fog.color), vec3(color), fog), color.a);
    }

    // Ajouter l'ombrage si activ�
    if(shadowMapOn == 1)
    {
	// Additionner l'effet des 3 ombres
		if (pointLightOn == 1) {
			color *= calculerOmbrage(shadowMap1, ShadowCoordLight0);;
		}
		if (dirLightOn == 1) {
			color *= calculerOmbrage(shadowMap2, ShadowCoordLight1);;
		}
		if (spotLightOn == 1) {
			color *= calculerOmbrage(shadowMap3, ShadowCoordLight2);;
		}
    }

    // couleur finale du gazon
    gl_FragColor = color;
}