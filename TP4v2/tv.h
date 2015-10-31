///////////////////////////////////////////////////////////////////////////////
///  @file TV.h
///  @author  Fr�d�ric Plourde 
///  @brief   D�clare la classe CTV qui encapsule un simple billboard 2D
///  @date    2008-10-19
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "modele3dabstrait.h"
#include "Cst.h"

///////////////////////////////////////////////////////////////////////////////
///  @class CTV
///  @brief Cette classe un objet graphique (t�l�viseur) en OVERLAY.
///
///  @author Fr�d�ric Plourde
///  @date   2008-10-19
///
///////////////////////////////////////////////////////////////////////////////
class CTV :
   public CModele3DAbstrait
{
public:
   /// constructeur cr�ant un billboard (gl_quad) avec texture sp�cifi�e unique (obligatoire)
   CTV(std::vector<char*> *nomsFichiersTextures, float largeur = 1.0, float longueur = 1.0);
   /// destructeur par d�faut
   ~CTV(void);

   /// fonction � impl�menter pour d�finir la g�om�trie openGL de la TV (est statique, car est r�f�r�e par un pointeur de fonction)
   static void dessiner();
   /// fonction � impl�menter pour d�finir un texturage personnalis�
   static void appliquerTextures(void* ptr = NULL);

private:
};
