uniform int pointLightOn;
uniform int spotLightOn;
uniform int dirLightOn;

varying vec3 ecPosition;   // la position du sommet oen coordonnées de visualisation
varying vec3 ecNormal;	   // la normale au sommet, en coordonnées e visualisation

// Accumulateurs pour les facteurs d'éclairage
vec4 Ambient;
vec4 Diffuse;
vec4 Specular;

vec4 test;


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
    HV = normalize(eyeDir + VP);

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
    HV = normalize(eyeDir + VP);

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
    HV = normalize(eyeDir + normalize(lightDir));

    nDotVP = max(0.0, dot(normal, normalize(vec3 (gl_LightSource[i].position))));
    specCos = max(0.0, dot(normal, HV));

    Ambient  += gl_LightSource[i].ambient;
    Diffuse  += gl_LightSource[i].diffuse * nDotVP;
    Specular += gl_LightSource[i].specular * pow(specCos, shininess);
}

// fonction d'éclairage pour l'ambiant et le diffus
vec4 lighting(in vec3 normal, in vec3 eyeDir, in vec3 light1Dir, in vec3 light2Dir, in vec3 light3Dir, in float shini, in vec4 texColor)
{
    vec4 color;

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
    color *= texColor;
    color += Specular * matSpecular;
    color =  clamp(color, 0.0, 1.0);

    return color;
}


void main (void) 
{

    vec4 color = lighting(normalize(ecNormal), 
		     normalize(-ecPosition), 
		     gl_LightSource[0].position.xyz - ecPosition, 
		     gl_LightSource[1].position.xyz - ecPosition,
		     gl_LightSource[2].position.xyz - ecPosition, 
		     gl_FrontMaterial.shininess, 
		     vec4(1.0, 1.0, 1.0, 1.0)
		     ); 

    gl_FragColor = color;
}