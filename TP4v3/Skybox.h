///////////////////////////////////////////////////////////////////////////////
///  @file Skybox.h
///  @author  Frédéric Plourde 
///  @brief   Déclare la classe CSkybox qui encapsule un skybox openGL avec
///           texture cubemap en format cross
///  @date    2007-12-11
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "modele3dabstrait.h"

///////////////////////////////////////////////////////////////////////////////
///  @class CSkybox
///  @brief Cette classe encapsule un Skybox openGL et gère les textures associées
///
///  @author Frédéric Plourde
///  @date   2008-10-19
///
///////////////////////////////////////////////////////////////////////////////
class CSkybox :
   public CModele3DAbstrait
{
public:
   /// constructeur créant un skybox avec texture spécifiée unique (obligatoire)
   CSkybox(char * nomFichierTexture, float echelle);
   /// destructeur par défaut
   ~CSkybox(void);

   /// fonction à implémenter pour définir la géométrie openGL du skybox (est statique, car est référée par un pointeur de fonction)
   static void dessiner();
   /// fonction à implémenter pour définir un texturage personnalisé
   static void appliquerTextures(void *);

private:
};
