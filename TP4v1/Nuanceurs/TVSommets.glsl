void main (void)
{
   // effectuons la base : la transformation du vertex
   gl_Position = ftransform();

   // Pasage des coordon�es des textures
   gl_TexCoord[0] = gl_MultiTexCoord0;
}
