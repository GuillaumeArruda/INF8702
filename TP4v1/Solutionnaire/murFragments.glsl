uniform int pointLightOn;
uniform int spotLightOn;
uniform int dirLightOn;
uniform int fogOn;
uniform int parallaxOn;
uniform int animModele;

uniform float time;           

uniform sampler2D frontColorMap;  // Unité de texture pour le dessus
uniform sampler2D normalMap;      // Unité de texture pour le bruit
uniform sampler2D heightMap;      // Unité de texture pour le parallax mapping

varying vec3 Light1Dir;   // Vecteur vers la Lumière 0 en espace Tangent
varying vec3 Light2Dir;   // Vecteur vers la Lumière 1 en espace Tangent
varying vec3 Light3Dir;   // Vecteur vers la Lumière 2 en espace Tangent
varying vec3 EyeDir;      // le vecteur camera qui sera transformé en espace tangent
varying vec3 SpotDir;     // la direction du spot en espace tangent


// Accumulateurs pour les facteurs d'éclairage
vec4 Ambient;
vec4 Diffuse;
vec4 Specular;


// Calcul pour une lumière ponctuelle
void pointLight(in int i, in vec3 normal, in vec3 eyeDir, in vec3 lightDir, in float shininess)
{
   float nDotVP;       // normal . light direction
   float attenuation;  // computed attenuation factor
   float d;            // distance from surface to light source
   vec3 VP;            // direction from surface to light position
   vec3 HV;            // the halfVector
   float specCos;

   // Compute distance between surface and light position
   d = length(lightDir);

   // Normalize the vector from surface to light position
   VP = normalize(lightDir);

   // Compute the halfvector
   HV = normalize(normalize(eyeDir) + VP);

   // Compute attenuation
   attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
      gl_LightSource[i].linearAttenuation * d +
      gl_LightSource[i].quadraticAttenuation * d * d);

   nDotVP  = max(0.0, dot(normal, VP));
   specCos = max(0.0, dot(normal, HV));

   Ambient  += gl_LightSource[i].ambient * attenuation;
   Diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation;
   Specular += gl_LightSource[i].specular * pow(specCos, shininess);
}


// Calcul pour une lumière "spot"
void spotLight(in int i, in vec3 normal, in vec3 eyeDir, in vec3 lightDir, in float shininess)
{
   float nDotVP;            // normal . light direction
   float spotDot;           // cosine of angle between spotlight
   float spotAttenuation;   // spotlight attenuation factor
   float attenuation;       // computed attenuation factor
   float d;                 // distance from surface to light source
   vec3  VP;                // direction from surface to light position
   vec3 HV;                 // the halfVector
   float specCos;

   // Compute distance between surface and light position
   d = length(lightDir);

   // Normalize the vector from surface to light position
   VP = normalize(lightDir);

   // Compute the halfvector
   HV = normalize(normalize(eyeDir) + VP);

   // Compute attenuation
   attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
      gl_LightSource[i].linearAttenuation * d +
      gl_LightSource[i].quadraticAttenuation * d * d);

   // See if point on surface is inside cone of illumination
   spotDot = dot(-VP, normalize(SpotDir));

   if (spotDot < gl_LightSource[i].spotCosCutoff)
      spotAttenuation = 0.0; // light adds no contribution
   else
      spotAttenuation = pow(spotDot, gl_LightSource[i].spotExponent);

   // Combine the spotlight and distance attenuation.
   attenuation *= spotAttenuation;

   nDotVP = max(0.0, dot(normal, VP));
   specCos = max(0.0, dot(normal, HV));

   Ambient  += gl_LightSource[i].ambient * attenuation;
   Diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation;
   Specular += gl_LightSource[i].specular * pow(specCos, shininess);
}


// Calcul pour une lumière directionnelle
void directionalLight(in int i, in vec3 normal, vec3 lightDir, in vec3 eyeDir, float shininess)
{
   float nDotVP;         // normal . light direction
   vec3 HV;              // the halfVector
   float specCos;

   // Compute the halfvector
   HV = normalize(normalize(eyeDir) + normalize(lightDir));

   nDotVP = max(0.0, dot(normal, normalize(vec3 (gl_LightSource[i].position))));
   specCos = max(0.0, dot(normal, HV));

   Ambient  += gl_LightSource[i].ambient;
   Diffuse  += gl_LightSource[i].diffuse * nDotVP;
   Specular += gl_LightSource[i].specular * pow(specCos, shininess);
}

// fonction d'éclairage pour l'ambiant et le diffus
void lighting(inout vec4 color, in vec3 normal, in vec3 eyeDir, in vec3 light1Dir, in vec3 light2Dir, in vec3 light3Dir, in float shini, in vec4 texColor)
{
      // Propriétés de la surface
   vec4 matAmbiant    = gl_FrontMaterial.ambient;
   vec4 matDiffuse    = gl_FrontMaterial.diffuse;
   vec4 matSpecular   = gl_FrontMaterial.specular;
   float matShininess = gl_FrontMaterial.shininess;

   // Clear the light intensity accumulators
   Ambient  = vec4 (0.0, 0.0, 0.0, 1.0);
   Diffuse  = vec4 (0.0, 0.0, 0.0, 1.0);
   Specular = vec4 (0.0, 0.0, 0.0, 1.0);

   // Calcul des 3 lumières
   if (pointLightOn == 1) {
      pointLight(0, normal, eyeDir, light1Dir, shini);
   }

   if (dirLightOn == 1) {
      directionalLight(1, normal, eyeDir, light2Dir, shini);
   }

   if (spotLightOn == 1) {
      spotLight(2, normal, eyeDir, light3Dir, shini);
   }

   // Ajout des contribution lumineuses calculées
   color =  gl_FrontLightModelProduct.sceneColor;
   color += Ambient * matAmbiant;
   color += Diffuse * matDiffuse;
   color *=  texColor;
   color += Specular * matSpecular;
   color =  clamp(color, 0.0, 1.0);
}


void main (void) 
{
   vec4 color = vec4(0.0, 0.0, 0.0, 1.0); 
   vec3 normal;            // Vecteur normal en espace tangent
   vec4 readColor;         // La couleur lue dans la carte de couleur
   vec3 readNormal;        // la normale lue dans la carte de normales
   vec2 texCoord = gl_TexCoord[0].st;

   float scale = 0.12;
   float bias  = 0.06;

   if (parallaxOn == 1) {
      // Obtenir la hauteur du texel et calculer la nouvelle coordonnée de texture à échantillonner
      float hauteur = texture2D(heightMap, texCoord).r;
      hauteur       = (hauteur * scale) - bias;
      vec2 delta    = (normalize(EyeDir).xy * hauteur);
      delta.y       = -delta.y;   // openGL assume y à l'envers...
      texCoord      = texCoord + delta;
   }

   // Échantillonnage de la texture
   readColor = texture2D(frontColorMap, texCoord);

   // Échantillonnage de la carte de normales
   readNormal = texture2D(normalMap, texCoord).rgb;
   // ajustement de la normale entre -1.0 et 1.0
   normal = normalize((readNormal - 0.5) * 2.0);
   normal.y = -normal.y;   // openGL assume y à l'envers...
   if (gl_FrontFacing == false)
      normal.z = -normal.z;

   // Calcul de la lumière ambiente et diffuse si en mode lumières
   lighting(color, normal, EyeDir, Light1Dir, Light2Dir, Light3Dir, gl_FrontMaterial.shininess, readColor); 

   // modulation finale de la couleur avec du brouillard
   if (fogOn == 1) {
      float fog;

      fog = (gl_Fog.end - gl_FogFragCoord) * gl_Fog.scale;
      fog = clamp(fog, 0.0, 1.0);
      color = vec4(mix( vec3(gl_Fog.color), vec3(color), fog), color.a);
   }

   gl_FragColor = color;
}