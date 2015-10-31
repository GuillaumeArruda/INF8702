///////////////////////////////////////////////////////////////////////////////
///  @file TV.h
///  @author  Frédéric Plourde 
///  @brief   Déclare la classe CTV qui encapsule un simple billboard 2D
///  @date    2008-10-19
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "modele3dabstrait.h"
#include "Cst.h"

///////////////////////////////////////////////////////////////////////////////
///  @class CTV
///  @brief Cette classe un objet graphique (téléviseur) en OVERLAY.
///
///  @author Frédéric Plourde
///  @date   2008-10-19
///
///////////////////////////////////////////////////////////////////////////////
class CTV :
   public CModele3DAbstrait
{
public:
   /// constructeur créant un billboard (gl_quad) avec texture spécifiée unique (obligatoire)
   CTV(std::vector<char*> *nomsFichiersTextures, float largeur = 1.0, float longueur = 1.0);
   /// destructeur par défaut
   ~CTV(void);

   /// fonction à implémenter pour définir la géométrie openGL de la TV (est statique, car est référée par un pointeur de fonction)
   static void dessiner();
   /// fonction à implémenter pour définir un texturage personnalisé
   static void appliquerTextures(void* ptr = NULL);

private:
};
