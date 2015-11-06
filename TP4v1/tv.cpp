///////////////////////////////////////////////////////////////////////////////
///  @file TV.cpp
///  @author  Fr�d�ric Plourde 
///  @brief   D�finit la classe CTV qui encapsule une patch bsplinaire antialias�e
///  @date    2008-10-19
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////

#include "Tv.h"

///////////////////////////////////////////////////////////////////////////////
///  public constructor  CTV \n
///  Construit un billboard au moyen d'une seule texture
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CTV::CTV(std::vector<char*> *nomsFichiersTextures, float largeur, float longueur):
CModele3DAbstrait(1.0, nomsFichiersTextures, TYPE_TEXTURE2D, true)
{
   // il est n�cessaire d'appeler "construire" ici pour cr�er la liste d'affichage
   // et les textures associ�es
   construire(&dessiner);
}



///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CTV \n
///  Destructeur par d�faut
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CTV::~CTV(void)
{
}


///////////////////////////////////////////////////////////////////////////////
///  public static  dessiner \n
///  Fonction � impl�menter absolument si on veut voir appara�tre quelque chose !
///  cette fonction sera appel�e par la classe de base (CModele3DAbstrait) par 
///  l'entremise d'une pointeur de fonction pass� � la construction. On mettra ici
///  uniquement le graphisme associ� � l'objet (i.e.  glBegin()...glVertex(), etc...)
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
void CTV::dessiner()
{
   glBegin(GL_QUADS); {
      glTexCoord2d(0.0, 0.0); glVertex2d(0,    0);
      glTexCoord2d(1.0, 0.0); glVertex2d(26.9, 0);
      glTexCoord2d(1.0, 1.0); glVertex2d(26.9, 26.6);
      glTexCoord2d(0.0, 1.0); glVertex2d(0,    26.6);
   } glEnd();
}


void CTV::appliquerTextures(void* ptr)
{
}


