uniform sampler2D colorMap;
uniform sampler2D alphaMap;
uniform sampler2D cameraMap;

void main (void) 
{
   vec2 scaledTexCoord;
   // les fronti�res de l'�cran � l'int�rieur de la texture du t�l�viseur
   float left   = 0.104;
   float right  = 0.885;
   float bottom = 0.278;
   float top    = 0.868;

   vec4 color;
   vec4 alpha;
   vec4 camColor;

   color    = texture2D(colorMap, gl_TexCoord[0].st);
   alpha    = texture2D(alphaMap, gl_TexCoord[0].st);

   // on utilise le "discard" pour impl�menter la "transparence".
   // tous les texels mis � noir [0 0 0] dans l'image alphaMap
   // seront tout simplement �limin�s du pipeline... donc parfaite 
   // transparence des c�t�s arrondis de l'image du t�l�viseur !
   // note : � cause de l'interpolation qui se fait lors du r��chantillonnage
   // de la texture � la bonne taille, on doit proc�der � la v�rification de 
   // la valeur alpha avec une certaine flexibilit� sur les valeurs lues.
   // le choix arbitraire < 0.5 est ici tr�s satisfaisant.
   if (alpha.r < 0.5)
      discard;

   if ((gl_TexCoord[0].s <= right) && (gl_TexCoord[0].s >= left) && (gl_TexCoord[0].t >= bottom) && (gl_TexCoord[0].t <= top) ) {
      // on se situe � l'int�rieur de l'�cran du t�l�viseur.
      // Mise � l'�chelle des coordonn�es de texture � l'int�rieur de l'�cran
      scaledTexCoord = (gl_TexCoord[0].st - vec2(left, bottom)) / vec2(right-left,top-bottom);
      camColor = texture2D(cameraMap, scaledTexCoord);

      // EFFET DE POST-TRAITEMENT ! :-)
      // ajout d'un effet "�cran cathodique" sur la texture de l'�cran
      if (mod(floor(scaledTexCoord.t * 50.0), 2.0) == 0.0) {
         gl_FragColor = (color) * (camColor) * 0.85;
      } else {
         gl_FragColor = (color) * (camColor);
      }
      gl_FragColor = (color) * (camColor);
   } else {
      // on se situe � l'ext�rieur de l'�cran et
      // on ne tient pas compte de la texture de cam�ra
      gl_FragColor = color;
   }
}
