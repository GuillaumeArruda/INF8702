uniform int pointLightOn;
uniform int spotLightOn;
uniform int dirLightOn;
uniform int fogOn;
uniform int perlinOn;
uniform int animModele;

uniform float time;           

uniform sampler2D frontColorMap;  // Unité de texture pour le dessus
uniform sampler2D backColorMap;   // Unité de texture pour le dessous
uniform sampler2D normalMap;      // Unité de texture pour le bruit

uniform float NoiseAmplitude;   // Facteur d'amplitude du bruit 

varying vec3 Light0HV;          // Lumière 0: Half vector en espace tangent
varying vec3 Light1HV;          // Lumière 1: Half vector en espace tangent
varying vec3 Light2HV;          // Lumière "spot" en esapce tangent


// Calcule la spécularité d'une source lumineuse
vec4 lightSpec(in int i, in vec3 normal, in vec3 halfVector, in float shininess)
{
    float specCos = max(0.0, dot(normal, halfVector));
    return gl_LightSource[i].specular * pow(specCos, shininess);
}


void main (void) 
{
    vec4 color = gl_Color;  // Couleur du fragment
    vec3 normal;            // Vecteur normal en espace tangent
    vec4 matSpecular;       // Couleur speculaire du matériau
    float matShininess;     // Shininess du matériau

    // Propriétés de la surface
    if(gl_FrontFacing)
    {	
        // Sampling de la texture
        color *= texture2D(frontColorMap, gl_TexCoord[0].xy);


        // Propriétés de la surface
        matSpecular  = gl_FrontMaterial.specular;
        matShininess = gl_FrontMaterial.shininess;

        // Échantillonnage du bruit de perlin
        vec3 noise = texture2D(normalMap, gl_TexCoord[0].xy).rgb;

        // Perturbation de la normale
        if (perlinOn == 1) {
            normal = normalize(noise);
        } else {
            normal = vec3(0.0, 0.0, 1.0);
        }
    }
    else
    {
        // Sampling de la texture
        vec2 texCoordInv = vec2(1.0 - gl_TexCoord[0].x, gl_TexCoord[0].y);
        color *= texture2D(backColorMap, texCoordInv);

        // Propriétés de la surface
        matSpecular  = gl_BackMaterial.specular;
        matShininess = gl_BackMaterial.shininess;

        // Normale inversée
        normal = vec3(0.0, 0.0, -1.0);

    }

    // Calcul du facteur spéculaire selon la lumière (allumée ou non)
    vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);

    if (pointLightOn == 1) {
        specular +=  lightSpec(0, normal, Light0HV, 400.0);
    }

    if (dirLightOn == 1) {
        specular +=  lightSpec(1, normal, Light1HV, 400.0);
    }

    if (spotLightOn == 1) {
        specular +=  lightSpec(2, normal, Light2HV, 100.0);
    }



    // Ajout de la contribution spéculaire au fragement
    color += specular * matSpecular;
    color = clamp(color, 0.0, 1.0);

    // modulation finale de la couleur avec du brouillard
    if (fogOn == 1) {
        float fog;

        fog = (gl_Fog.end - gl_FogFragCoord) * gl_Fog.scale;
        fog = clamp(fog, 0.0, 1.0);
        color = vec4(mix( vec3(gl_Fog.color), vec3(color), fog), color.a);
    }

    gl_FragColor = color;
}


