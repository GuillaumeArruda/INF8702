uniform sampler2D colorMap;

void main (void)
{
   // effectuons la base : la transformation du vertex
   gl_Position = ftransform();

   gl_TexCoord[0] = gl_Vertex;
}
