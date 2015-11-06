uniform sampler2D colorMap;
uniform sampler2D alphaMap;
uniform sampler2D cameraMap;

void main (void) 
{
   vec2 scaledTexCoord;
   // les frontières de l'écran à l'intérieur de la texture du téléviseur
   float left   = 0.104;
   float right  = 0.885;
   float bottom = 0.278;
   float top    = 0.868;

   vec4 color;
   vec4 alpha;
   vec4 camColor;

   color    = texture2D(colorMap, gl_TexCoord[0].st);
   alpha    = texture2D(alphaMap, gl_TexCoord[0].st);

   // on utilise le "discard" pour implémenter la "transparence".
   // tous les texels mis à noir [0 0 0] dans l'image alphaMap
   // seront tout simplement éliminés du pipeline... donc parfaite 
   // transparence des côtés arrondis de l'image du téléviseur !
   // note : à cause de l'interpolation qui se fait lors du rééchantillonnage
   // de la texture à la bonne taille, on doit procéder à la vérification de 
   // la valeur alpha avec une certaine flexibilité sur les valeurs lues.
   // le choix arbitraire < 0.5 est ici très satisfaisant.
   if (alpha.r < 0.5)
      discard;

   if ((gl_TexCoord[0].s <= right) && (gl_TexCoord[0].s >= left) && (gl_TexCoord[0].t >= bottom) && (gl_TexCoord[0].t <= top) ) {
      // on se situe à l'intérieur de l'écran du téléviseur.
      // Mise à l'échelle des coordonnées de texture à l'intérieur de l'écran
      scaledTexCoord = (gl_TexCoord[0].st - vec2(left, bottom)) / vec2(right-left,top-bottom);
      camColor = texture2D(cameraMap, scaledTexCoord);

      // EFFET DE POST-TRAITEMENT ! :-)
      // ajout d'un effet "écran cathodique" sur la texture de l'écran
      if (mod(floor(scaledTexCoord.t * 50.0), 2.0) == 0.0) {
         gl_FragColor = (color) * (camColor) * 0.85;
      } else {
         gl_FragColor = (color) * (camColor);
      }
      gl_FragColor = (color) * (camColor);
   } else {
      // on se situe à l'extérieur de l'écran et
      // on ne tient pas compte de la texture de caméra
      gl_FragColor = color;
   }
}
