uniform int pointLightOn;
uniform int spotLightOn;
uniform int dirLightOn;
uniform int fogOn;
uniform int perlinOn;
uniform int animModele;

uniform float time;           // Temps pour l'animation displacement mapping

const float PI = 3.1415926535897932385;  // Pi
const float PI_INV = 1.0 / PI;        // Pi inversé

attribute vec3 Tangent;  // Vecteur tangent déjà  normalisé

varying vec3 Light0HV;   // HalfVector de la Lumière 0 en espace Tangent
varying vec3 Light1HV;   // HalfVector de la Lumière 1 en espace Tangent
varying vec3 Light2HV;   // HalfVector de la Lumière 2 en espace Tangent

// Accumulateurs pour les facteurs d'éclairage
vec4 Ambient;
vec4 Diffuse;


// Calcul pour une lumière ponctuelle
void pointLight(in int i, in vec3 normal, in vec3 eye, in vec3 ecPosition3)
{
   float nDotVP;       // normal . light direction
   float attenuation;  // computed attenuation factor
   float d;            // distance from surface to light source
   vec3  VP;           // direction from surface to light position

   // Compute vector from surface to light position
   VP = vec3 (gl_LightSource[i].position) - ecPosition3;

   // Compute distance between surface and light position
   d = length(VP);

   // Normalize the vector from surface to light position
   VP = normalize(VP);

   // Compute attenuation
   attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
      gl_LightSource[i].linearAttenuation * d +
      gl_LightSource[i].quadraticAttenuation * d * d);

   nDotVP = max(0.0, dot(normal, VP));

   Ambient += gl_LightSource[i].ambient * attenuation;
   Diffuse += gl_LightSource[i].diffuse * nDotVP * attenuation;
}


// Calcul pour une lumière "spot"
void spotLight(in int i, in vec3 normal, in vec3 eye, in vec3 ecPosition3)
{
   float nDotVP;            // normal . light direction
   float spotDot;           // cosine of angle between spotlight
   float spotAttenuation;   // spotlight attenuation factor
   float attenuation;       // computed attenuation factor
   float d;                 // distance from surface to light source
   vec3  VP;                // direction from surface to light position

   // Compute vector from surface to light position
   VP = vec3 (gl_LightSource[i].position) - ecPosition3;

   // Compute distance between surface and light position
   d = length(VP);

   // Normalize the vector from surface to light position
   VP = normalize(VP);

   // Compute attenuation
   attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
      gl_LightSource[i].linearAttenuation * d +
      gl_LightSource[i].quadraticAttenuation * d * d);

   // See if point on surface is inside cone of illumination
   spotDot = dot(-VP, normalize(gl_LightSource[i].spotDirection));

   if (spotDot < gl_LightSource[i].spotCosCutoff)
      spotAttenuation = 0.0; // light adds no contribution
   else
      spotAttenuation = pow(spotDot, gl_LightSource[i].spotExponent);

   // Combine the spotlight and distance attenuation.
   attenuation *= spotAttenuation;

   nDotVP = max(0.0, dot(normal, VP));

   Ambient  += gl_LightSource[i].ambient * attenuation;
   Diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation;
}


// Calcul pour une lumière directionnelle
void directionalLight(in int i, in vec3 normal)
{
   float nDotVP;         // normal . light direction

   nDotVP = max(0.0, dot(normal, normalize(vec3 (gl_LightSource[i].position))));

   Ambient  += gl_LightSource[i].ambient;
   Diffuse  += gl_LightSource[i].diffuse * nDotVP;
}


// éclairage pour la surface du dessus
void frontLighting(in vec3 normal, in vec3 ecPosition)
{
   vec4 color;
   const vec3 eye = vec3 (0.0, 0.0, 1.0);

   // Clear the light intensity accumulators
   Ambient  = vec4 (0.0);
   Diffuse  = vec4 (0.0);

   // Calcul des 3 lumières
   if (pointLightOn == 1) {
      pointLight(0, normal, eye, ecPosition);
   }
   
   if (dirLightOn == 1) {
      directionalLight(1, normal);
   }

   if (spotLightOn == 1) {
      spotLight(2, normal, eye, ecPosition);
   }

   color = gl_FrontLightModelProduct.sceneColor +
      Ambient  * gl_FrontMaterial.ambient +
      Diffuse  * gl_FrontMaterial.diffuse;
   color = clamp( color, 0.0, 1.0 );
   gl_FrontColor = color;
}


// à‰clairage pour la surface du dessous
void backLighting(in vec3 invNormal, in vec3 ecPosition)
{
   vec4 color;
   const vec3 eye = vec3 (0.0, 0.0, 1.0);

   // Clear the light intensity accumulators
   Ambient  = vec4 (0.0);
   Diffuse  = vec4 (0.0);

   // Calcul des 3 lumières
   if (pointLightOn == 1) {
      pointLight(0, invNormal, eye, ecPosition);
   }

   if (dirLightOn == 1) {
      directionalLight(1, invNormal);
   }

   if (spotLightOn == 1) {
      spotLight(2, invNormal, eye, ecPosition);
   }

   color = gl_BackLightModelProduct.sceneColor +
      Ambient  * gl_BackMaterial.ambient +
      Diffuse  * gl_BackMaterial.diffuse;
   color = clamp( color, 0.0, 1.0 );
   gl_BackColor = color;
}


// Transformation des coordonnées d'espace tangent
void tsTransform(in vec3 ecNormal, vec3 ecTangent, vec3 ecPosition)
{
   // Calcul de la binormale
   vec3 EyeDir;
   vec3 ecBinormal = cross(ecTangent, ecNormal); 

   // Calcul de la matrice de transformation pour passer en espace tangent
   mat3 tsMatrix = mat3(
      ecTangent.x, ecBinormal.x, ecNormal.x,
      ecTangent.y, ecBinormal.y, ecNormal.y,
      ecTangent.z, ecBinormal.z, ecNormal.z);

   // Calcul de la direction des lumières (pas besoin pour une lum directionnelle)
   vec3 light0Dir = normalize(gl_LightSource[0].position.xyz - ecPosition);
   vec3 light2Dir = normalize(gl_LightSource[2].position.xyz - ecPosition);

   // Calcul des vecteurs pertinants
   EyeDir    = vec3(0.0, 0.0, 1.0); // nous sommes en coordonnées de visualisation
                                    // on regarde toujours devant !
   Light0HV  = normalize(light0Dir + EyeDir);
   Light1HV  = gl_LightSource[1].halfVector.xyz;  // lumière à l'infini...
   Light2HV  = normalize(light2Dir + EyeDir);

   // Transformation dans l'espace tangent
   EyeDir    = tsMatrix * EyeDir;
   Light0HV  = tsMatrix * Light0HV;
   Light1HV  = tsMatrix * Light1HV;
   Light2HV  = tsMatrix * Light2HV;
}


// Retourne la matrice d'une rotation de theta radians autour de Y
mat3 rotMatX(float theta)
{
   return mat3(1.0, 0.0,        0.0,
      0.0, cos(theta), -sin(theta),
      0.0, sin(theta), cos(theta));
}


// Retourne la matrice d'une rotation de theta radians autour de Y
mat3 rotMatY(float theta)
{
   return mat3(cos(theta), 0.0, -sin(theta),
      0.0,        1.0, 0.0,
      sin(theta), 0.0, cos(theta));
}

// Retourne la matrice d'une rotation de theta radians autour de Y
mat3 rotMatZ(float theta)
{
   return mat3(cos(theta),  sin(theta), 0.0,
      -sin(theta), cos(theta), 0.0,
      0.0,         0.0,        1.0);
}


float ffog(in float ecDistance)
{
   return(abs(ecDistance));
}


// Animation de flexion de la carte (displacement mapping)
// Cette fonction transforme les variables en entrée
void animate(inout vec4 position, inout vec3 normal, inout vec3 tangent)
{
   float amplitude; // Amplitude selon le temps et l'espace (animation) 
   float deltaY;    // Variation de la hauteur du vertex
   mat3 rotMat;     // Matrice de tranformation pour la normale et tangente

   // Variation de l'amplitude de la déformation selon le temps
   amplitude = 2.0 * sin(time);

   // Déformation selon l'amplitude.
   if(amplitude < 0.0) {
      // Déformation sur l'axe des X
      deltaY = amplitude * sin(gl_MultiTexCoord0.x * PI);
      float theta = 0.5 * (gl_MultiTexCoord0.x - 0.5) * PI * sin(-amplitude);
      rotMat = rotMatZ(-theta);
   } else {
      // Déformation sur l'axe des Y
      deltaY = amplitude * sin(gl_MultiTexCoord0.y * PI);
      float theta = 0.5 * (gl_MultiTexCoord0.y - 0.5) * PI * sin(amplitude);
      rotMat = rotMatX(-theta);
   }

   // Transformation de la position, normale et tangente
   position.y += deltaY;
   normal *= rotMat;
   tangent *= rotMat;
}


void main()
{
   // Variables qui seront transformées par l'animation
   vec4 position = gl_Vertex;
   vec3 normal = gl_Normal;
   vec3 tangent = Tangent;

   // Transformation du vertex selon le temps
   if (animModele == 1) {
      animate(position, normal, tangent);
   }

   // Eye-coordonate position of vertex, normal, and tangent
   vec3 ecPosition = vec3(gl_ModelViewMatrix * position);
   vec3 ecNormal   = gl_NormalMatrix * normal; 
   vec3 ecTangent  = gl_NormalMatrix * tangent; 
  
   // le calcul du fog doit prendre en compte la nouvelle coordonnée
   gl_FogFragCoord = ffog((gl_ModelViewMatrix * position).z);

   // Transformation du vertex dans l'espace de clipping
   gl_Position = gl_ModelViewProjectionMatrix * position;

   // Calcul de la lumière ambiente et diffuse si en mode lumières
   frontLighting(ecNormal, ecPosition); 
   backLighting(-ecNormal, ecPosition);

   // Pasage des coordonées des textures
   gl_TexCoord[0] = gl_MultiTexCoord0;
   //gl_TexCoord[1] = gl_MultiTexCoord1;
   //gl_TexCoord[2] = gl_MultiTexCoord2;

   // Transformation des coordonées en espace tangent
   tsTransform(ecNormal, ecTangent, ecPosition);
}

