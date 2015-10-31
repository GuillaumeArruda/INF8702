///////////////////////////////////////////////////////////////////////////////
///  @file Gazon.cpp
///  @author  Fr�d�ric Plourde 
///  @brief   D�finit la classe CGazon qui encapsule une patch bsplinaire antialias�e
///  @date    2007-12-11
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////

#include "Gazon.h"

///////////////////////////////////////////////////////////////////////////////
///  public constructor  CGazon \n
///  Construit un skybox au moyen d'une seule texture (seule possibilit� pour un skybox)
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CGazon::CGazon(char * nomFichierTexture, float largeurGazon, float longueurGazon):
CModele3DAbstrait(1.0, nomFichierTexture, TYPE_TEXTURE2D, false, true),
CSurfaceBSplinaire(8, 4, CCst::Points, 12, 8, CCst::VNodalU, CCst::VNodalV, 40)
{
   // il est n�cessaire d'appeler "construire" ici pour cr�er la liste d'affichage
   // et les textures associ�es
   construire(&dessiner);
}



///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CGazon \n
///  Destructeur par d�faut d'un skybox
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CGazon::~CGazon(void)
{
   if(noeudsU_)
      delete [] noeudsU_;

   if(noeudsV_)
      delete [] noeudsV_;

   if(points_)
      delete [] points_;
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
void CGazon::dessiner()
{
   dessinerOpenGL();
}


void CGazon::appliquerTextures(void* ptr)
{
}


