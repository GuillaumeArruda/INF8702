uniform sampler2D colorMap;

uniform int pointLightOn;
uniform int spotLightOn;
uniform int dirLightOn;

// Accumulateurs pour les facteurs d'éclairage (AMBIANT ET DIFFUS UNIQUEMENT)
// On POSE d'emblée que le matériau du gazon n'a PAS de composante spéculaire
vec4 Ambient;
vec4 Diffuse;

// Calcul pour une lumière ponctuelle
void pointLight(in int i, in vec3 normal, in vec3 eye, in vec3 ecPosition3)
{
   float nDotVP;       // Produit scalaire entre VP et la normale
   float attenuation;  // facteur d'atténuation calculé
   float d;            // distance entre lumière et fragment
   vec3  VP;           // Vecteur lumière

   // Calculer vecteur lumière
   VP = vec3 (gl_LightSource[i].position) - ecPosition3;

   // Calculer distance à la lumière
   d = length(VP);

   // Normaliser VP
   VP = normalize(VP);

   // Calculer l'atténuation due à la distance
   attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
       gl_LightSource[i].linearAttenuation * d +
       gl_LightSource[i].quadraticAttenuation * d * d);

   nDotVP = max(0.0, dot(normal, VP));

   // Calculer les contributions ambiantes et diffuses
   Ambient  += gl_LightSource[i].ambient * attenuation;
   Diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation;
}


// Calcul pour une lumière directionnelle
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


// Calcul pour une lumière "spot"
void spotLight(in int i, in vec3 normal, in vec3 eye, in vec3 ecPosition3)
{
   float nDotVP;            // Produit scalaire entre VP et la normale
   float spotDot;           // Cosinus angle entre VP et direction spot
   float spotAttenuation;   // Facteur d'atténuation du spot
   float attenuation;       // Facteur d'atténuation du à la distance
   float d;                 // Distance à la lumière
   vec3  VP;                // Vecteur lumière

   // Calculer le vecteur Lumière
   VP = vec3 (gl_LightSource[i].position) - ecPosition3;

   // Calculer la distance à al lumière
   d = length(VP);

   // Normaliser VP
   VP = normalize(VP);

   // Calculer l'atténuation due à la distance
   attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
       gl_LightSource[i].linearAttenuation * d +
       gl_LightSource[i].quadraticAttenuation * d * d);

   // Le fragment est-il à l'intérieur du cône de lumière ?
   spotDot = dot(-VP, normalize(gl_LightSource[i].spotDirection));

   if (spotDot < gl_LightSource[i].spotCosCutoff)
   {
       spotAttenuation = 0.0; // light adds no contribution
   }
   else
   {
       spotAttenuation = pow(spotDot, gl_LightSource[i].spotExponent);

   }

   // Combine les atténuation du spot et de la distance
   attenuation *= spotAttenuation;

   nDotVP = max(0.0, dot(normal, VP));

   // Calculer les contributions ambiantes et diffuses
   Ambient  += 3.0 * gl_LightSource[i].ambient * attenuation;
   Diffuse  += 3.0 * gl_LightSource[i].diffuse * nDotVP * attenuation;
}


vec3 fnormal(void)
{
    // Calculer la normale
    vec3 normal = gl_NormalMatrix * gl_Normal;
    normal = normalize(normal);
    return normal;
}

void flight(in vec3 normal, in vec4 ecPosition)
{
    vec4 color;
    vec3 ecPosition3;
    vec3 eye;

    ecPosition3 = (vec3 (ecPosition)) / ecPosition.w;
    eye = vec3 (0.0, 0.0, -1.0);

    // Réinitialiser les accumulateurs
    Ambient  = vec4 (0.0);
    Diffuse  = vec4 (0.0);
  
    if (pointLightOn == 1) {
        pointLight(0, normal, eye, ecPosition3);
    }

    if (dirLightOn == 1) {
        directionalLight(1, normal);
    }
    
    if (spotLightOn == 1) {
        spotLight(2, normal, eye, ecPosition3);
    }

    color = gl_FrontLightModelProduct.sceneColor +
      Ambient  * gl_FrontMaterial.ambient +
      Diffuse  * gl_FrontMaterial.diffuse;
    color = clamp( color, 0.0, 1.0 );
    gl_FrontColor = color;
}


void ftexgen()
{
   // pour le mappage correct du gazon
   gl_TexCoord[0] = gl_MultiTexCoord0;
}


void main (void)
{
    vec3  transformedNormal;

    // Vecteur position en coord. de visualisation
    vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;

    gl_Position = ftransform();
    transformedNormal = fnormal();
    ftexgen();
    flight(transformedNormal, ecPosition);
}
