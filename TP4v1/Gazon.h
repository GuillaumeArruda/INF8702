///////////////////////////////////////////////////////////////////////////////
///  @file Gazon.h
///  @author  Frédéric Plourde 
///  @brief   Déclare la classe CGazon qui encapsule une patch BSplinaire openGL
///           avec antialiasing
///  @date    2007-12-11
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "modele3dabstrait.h"
#include "SurfaceBSplinaire.h"
#include "Cst.h"

///////////////////////////////////////////////////////////////////////////////
///  @class CGazon
///  @brief Cette classe encapsule une patch bsplinaire (gazon)
///
///  @author Frédéric Plourde 
///  @date   2007-12-11
///
///////////////////////////////////////////////////////////////////////////////
class CGazon :
   public CModele3DAbstrait, CSurfaceBSplinaire
{
public:
   /// constructeur créant une patch bsplinaire (gazon) avec texture spécifiée unique (obligatoire)
   CGazon(char * nomFichierTexture, float largeurGazon = 1.0, float longueurGazon = 1.0);
   /// destructeur par défaut
   ~CGazon(void);

   /// fonction à implémenter pour définir la géométrie openGL du gazon (est statique, car est référée par un pointeur de fonction)
   static void dessiner();
   /// fonction à implémenter pour définir un texturage personnalisé
   static void appliquerTextures(void* ptr = NULL);

private:
};
