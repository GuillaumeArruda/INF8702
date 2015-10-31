uniform samplerCube colorMap;

void main (void) 
{
   vec4 color;

   // compl�ter le nuanceur ici pour qu'il affiche le bon texel en mode GL_REPLACE
   color = textureCube(colorMap, gl_TexCoord[0].xyz);

   gl_FragColor = color;
}
