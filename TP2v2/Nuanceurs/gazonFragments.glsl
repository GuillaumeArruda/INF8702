
uniform sampler2D colorMap;
uniform int pointLightOn;
uniform int spotLightOn;
uniform int dirLightOn;

// Accumulateurs pour les facteurs d'�clairage (AMBIANT ET DIFFUS UNIQUEMENT)
// On POSE d'embl�e que le mat�riau du gazon n'a PAS de composante sp�culaire
vec4 Ambient;
vec4 Diffuse;

varying vec3 transformedNormal;
varying vec3 eyePosition;
varying vec3 positionVertex;


// Calcul pour une lumi�re ponctuelle
void pointLight(in int i, in vec3 normal, in vec3 eye, in vec3 ecPosition3)
{
   float nDotVP;       // Produit scalaire entre VP et la normale
   float attenuation;  // facteur d'att�nuation calcul�
   float d;           // distance entre lumi�re et fragment
   vec3  VP;           // Vecteur lumi�re

   // Calculer vecteur lumi�re
   VP = vec3 (gl_LightSource[i].position) - ecPosition3;

   // Calculer distance � la lumi�re
   d = length(VP);

   // Normaliser VP
   VP = normalize(VP);

   // Calculer l'att�nuation due � la distance
   attenuation = 1.0  / (gl_LightSource[i].constantAttenuation +
       gl_LightSource[i].linearAttenuation * d +
       gl_LightSource[i].quadraticAttenuation * d * d);

   nDotVP = max(0.0, dot(normal, VP));

   // Calculer les contributions ambiantes et diffuses
   Ambient  += gl_LightSource[i].ambient * attenuation;
   Diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation;
}


// Calcul pour une lumi�re directionnelle
void directionalLight(in int i, in vec3 normal)
{
   float nDotVP;         // normal . light direction
   float nDotHV;         // normal . light half vector
   float pf;             // power factor

   nDotVP = max(0.0, dot(normal, normalize(vec3 (gl_LightSource[i].position))));

   // Calculer les contributions ambiantes et diffuses
   Ambient  += gl_LightSource[i].ambient;
   Diffuse  += gl_LightSource[i].diffuse * nDotVP;
}


// Calcul pour une lumi�re "spot"
void spotLight(in int i, in vec3 normal, in vec3 eye, in vec3 ecPosition3)
{
   float nDotVP;            // Produit scalaire entre VP et la normale
   float spotDot;           // Cosinus angle entre VP et direction spot
   float spotAttenuation;   // Facteur d'att�nuation du spot
   float attenuation;       // Facteur d'att�nuation du � la distance
   float d;                 // Distance � la lumi�re
   vec3  VP;                // Vecteur lumi�re

   // Calculer le vecteur Lumi�re
   VP = vec3 (gl_LightSource[i].position) - ecPosition3;

   // Calculer la distance � al lumi�re
   d = length(VP);

   // Normaliser VP
   VP = normalize(VP);

   // Calculer l'att�nuation due � la distance
   attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
       gl_LightSource[i].linearAttenuation * d +
       gl_LightSource[i].quadraticAttenuation * d * d);

   // Le fragment est-il � l'int�rieur du c�ne de lumi�re ?
   spotDot = dot(-VP, normalize(gl_LightSource[i].spotDirection));

   if (spotDot < gl_LightSource[i].spotCosCutoff)
   {
       spotAttenuation = 0.0; // light adds no contribution
   }
   else
   {
       spotAttenuation = pow(spotDot, gl_LightSource[i].spotExponent);
   }
   
   // Combine les att�nuation du spot et de la distance
   attenuation *= spotAttenuation;

   nDotVP = max(0.0, dot(normal, VP));

   // Calculer les contributions ambiantes et diffuses
   Ambient  += 3.0 * gl_LightSource[i].ambient * attenuation;
   Diffuse  += 3.0 * gl_LightSource[i].diffuse * nDotVP * attenuation;
}

vec4 flight(in vec3 normal, in vec3 eyePosition, in vec3 positionVertex)
{
    vec4 color;

    // R�initialiser les accumulateurs
    Ambient  = vec4 (0.0);
    Diffuse  = vec4 (0.0);
  
    if (pointLightOn == 1) {
        pointLight(0, normal, eyePosition, positionVertex);
    }

    if (dirLightOn == 1) {
        directionalLight(1, normal);
    }
    
    if (spotLightOn == 1) {
        spotLight(2, normal, eyePosition, positionVertex);
    }

    color = gl_FrontLightModelProduct.sceneColor +
      Ambient  * gl_FrontMaterial.ambient +
      Diffuse  * gl_FrontMaterial.diffuse;
    color = clamp( color, 0.0, 1.0 );
    return color;
}

void main (void) 
{
   vec4 color;

   //ecolor = gl_Color;
   color = texture2D(colorMap, gl_TexCoord[0].xy);
   color *= flight(transformedNormal,eyePosition,positionVertex);
   gl_FragColor = color;
}