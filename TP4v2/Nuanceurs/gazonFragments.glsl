uniform sampler2D colorMap;

uniform int pointLightOn;
uniform int spotLightOn;
uniform int dirLightOn;

// varyings pour le calcul de l'éclairage par fragments
varying vec3 fragNormal;
varying vec3 fragLight0Vect = vec3(0.0, 0.0, 0.0);
varying vec3 fragLight1Vect = vec3(0.0, 0.0, 0.0);
varying vec3 fragLight2Vect = vec3(0.0, 0.0, 0.0);

// Accumulateurs pour les facteurs d'éclairage
vec4 Ambient;
vec4 Diffuse;

// Calcul pour une lumière ponctuelle
void pointLight(in vec3 lightVect)
{
   float nDotVP;       // Produit scalaire entre VP et la normale
   float attenuation;  // facteur d'atténuation calculé
   float d;            // distance entre lumière et fragment
   vec3  VP;           // Vecteur lumière

   // Calculer vecteur lumière
   VP = lightVect;

   // Calculer distance à la lumière
   d = length(VP);

   // Normaliser VP
   VP = normalize(VP);

   // Calculer l'atténuation due à la distance
   attenuation = 1.0 / (gl_LightSource[0].constantAttenuation +
       gl_LightSource[0].linearAttenuation * d +
       gl_LightSource[0].quadraticAttenuation * d * d);

   nDotVP = max(0.0, dot(fragNormal, VP));

   // Calculer les contributions ambiantes et diffuses
   Ambient  += gl_LightSource[0].ambient * attenuation;
   Diffuse  += gl_LightSource[0].diffuse * nDotVP * attenuation;
}


// Calcul pour une lumière directionnelle
void directionalLight(in vec3 lightVect)
{
   float nDotVP;         // Produit scalaire entre VP et la normale

   nDotVP = max(0.0, dot(fragNormal, normalize(vec3 (gl_LightSource[1].position))));

   // Calculer les contributions ambiantes et diffuses
   Ambient  += gl_LightSource[1].ambient;
   Diffuse  += gl_LightSource[1].diffuse * nDotVP;
}


// Calcul pour une lumière "spot"
void spotLight(in vec3 lightVect)
{
   float nDotVP;            // Produit scalaire entre VP et la normale
   float spotDot;           // Cosinus angle entre VP et direction spot
   float spotAttenuation;   // Facteur d'atténuation du spot
   float attenuation;       // Facteur d'atténuation du à la distance
   float d;                 // Distance à la lumière
   vec3  VP;                // Vecteur lumière

   // Calculer le vecteur Lumière
   VP = lightVect;

   // Calculer la distance à al lumière
   d = length(VP);

   // Normaliser VP
   VP = normalize(VP);

   // Calculer l'atténuation due à la distance
   attenuation = 1.0 / (gl_LightSource[2].constantAttenuation +
       gl_LightSource[2].linearAttenuation * d +
       gl_LightSource[2].quadraticAttenuation * d * d);

   // Le fragment est-il à l'intérieur du cône de lumière ?
   spotDot = dot(-VP, normalize(gl_LightSource[2].spotDirection));

   if (spotDot < gl_LightSource[2].spotCosCutoff)
   {
       spotAttenuation = 0.0; // en dehors... aucune contribution
   }
   else
   {
       spotAttenuation = pow(spotDot, gl_LightSource[2].spotExponent);

   }

   // Combine les atténuation du spot et de la distance
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

    // Réinitialiser les accumulateurs
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

void main (void) 
{
    vec4 color;
    // color = gl_Color;  // Il est très important de ne pas obtenir la couleur provenant du NS.
                          // Car on ne fait plus aucun calculs dans le NS !
    color = texture2D(colorMap, gl_TexCoord[0].xy);
    color *= flight(fragNormal);
    gl_FragColor = color;
}