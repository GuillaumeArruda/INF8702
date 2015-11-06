varying vec3 ecPosition;   // la position du sommet oen coordonnées de visualisation
varying vec3 ecNormal;	   // la normale au sommet, en coordonnées e visualisation

void main()
{
    vec4 position = gl_Vertex;
    vec3 normal   = gl_Normal;

    // Eye-coordonate position of vertex, normal
    ecPosition = vec3(gl_ModelViewMatrix * position);
    ecNormal   = gl_NormalMatrix * normal; 

    // Pasage des coordonées des textures
    gl_TexCoord[0] = gl_MultiTexCoord0;

    // Transformation du vertex dans l'espace de clipping
    gl_Position = gl_ModelViewProjectionMatrix * position;
}

