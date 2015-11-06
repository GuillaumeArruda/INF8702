///////////////////////////////////////////////////////////////////////////////
///  @file Skybox.cpp
///  @author  Frédéric Plourde 
///  @brief   Définit la classe CSkybox qui encapsule un skybox openGL avec
///           texture cubemap en format cross
///  @date    2007-12-11
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////

#include "Skybox.h"

///////////////////////////////////////////////////////////////////////////////
///  public constructor  CSkybox \n
///  Construit un skybox au moyen d'une seule texture (seule possibilité pour un skybox)
///  attention : le format de la texture DOIT être en format "cross".
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CSkybox::CSkybox(char * nomFichierTexture, float echelle = 1.0):
CModele3DAbstrait(echelle, nomFichierTexture, TYPE_TEXTURE_CUBEMAP, true)
{
   // il est nécessaire d'appeler "construire" ici pour créer la liste d'affichage
   // et les textures associées
   construire(&dessiner, &appliquerTextures, true);
}



///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CSkybox \n
///  Destructeur par défaut d'un skybox
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CSkybox::~CSkybox(void)
{
}


///////////////////////////////////////////////////////////////////////////////
///  public static  dessiner \n
///  Fonction à implémenter absolument si on veut voir apparaître quelque chose !
///  cette fonction sera appelée par la classe de base (CModele3DAbstrait) par 
///  l'entremise d'une pointeur de fonction passé à la construction. On mettra ici
///  uniquement le graphisme associé à l'objet (i.e.  glBegin()...glVertex(), etc...)
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
void CSkybox::dessiner()
{
   glBegin(GL_QUADS); {

      // FACE DE DEVANT (Y POS)
      glNormal3f(0.0, -1.0, 0.0);
      glVertex3f(-0.5, 0.5,-0.5);
      glVertex3f(-0.5, 0.5, 0.5);
      glVertex3f( 0.5, 0.5, 0.5);
      glVertex3f( 0.5, 0.5,-0.5);

      // FACE DE DERRIÈRE (Y NEG)
      glNormal3f( 0.0, 1.0, 0.0);
      glVertex3f( 0.5,-0.5,-0.5);
      glVertex3f( 0.5,-0.5, 0.5);
      glVertex3f(-0.5,-0.5, 0.5);
      glVertex3f(-0.5,-0.5,-0.5);

      // FACE DE GAUCHE (X NEG)
      glNormal3f( 1.0, 0.0, 0.0);
      glVertex3f(-0.5,-0.5,-0.5);
      glVertex3f(-0.5,-0.5, 0.5);
      glVertex3f(-0.5, 0.5, 0.5);
      glVertex3f(-0.5, 0.5,-0.5);

      // FACE DE DROITE (X POS)
      glNormal3f(-1.0,0.0, 0.0);
      glVertex3f(0.5,-0.5,-0.5);
      glVertex3f(0.5,-0.5, 0.5);
      glVertex3f(0.5, 0.5, 0.5);
      glVertex3f(0.5, 0.5,-0.5);

      // FACE DU HAUT (Z POS)
      glNormal3f(0.0, 0.0, -1.0);
      glVertex3f(-0.5, 0.5, 0.5);
      glVertex3f(-0.5,-0.5, 0.5);
      glVertex3f( 0.5,-0.5, 0.5);
      glVertex3f( 0.5, 0.5, 0.5);        

      // FACE DU BAS (Z NEG)
      glNormal3f( 0.0, 0.0, 1.0);
      glVertex3f(-0.5,-0.5,-0.5);
      glVertex3f(-0.5, 0.5,-0.5);
      glVertex3f( 0.5, 0.5,-0.5);
      glVertex3f( 0.5,-0.5,-0.5);  

   }glEnd();
}


void CSkybox::appliquerTextures(void * ptr)
{
   CSkybox* skybox = (CSkybox*)ptr;

   // appliquer la texture de façon personnalisée (mode CLAMP_TO_EDGE)
   (*(skybox->obtenirTextures()))[0]->appliquer(
      WRAP_S|WRAP_T|MIN_FILTER|MAG_FILTER, 
      GL_CLAMP_TO_EDGE,        GL_CLAMP_TO_EDGE,    0,
      GL_LINEAR, GL_LINEAR,
      0,                       0,
      0,                       0,
      0,                       0,
      false,                   GL_TEXTURE0);
}