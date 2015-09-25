#version 120
uniform sampler2D colorMap;

void main (void)
{
   // effectuons la base : la transformation du vertex
   gl_Position = ftransform();

   // TODO:
   // passer correctement les coordonnées de texture
   gl_TexCoord[0] = gl_Vertex;
}
