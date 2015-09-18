///////////////////////////////////////////////////////////////////////////////
///  @file Carte.h
///  @author  Frédéric Plourde
///  @brief   Déclare la classe CCarte qui encapsule un modele3D spécifique 
///           de carte d'identité de polytechnique
///  @date    2008-02-01
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "modele3dabstrait.h"
#include "Cst.h"
#include "Texture2D.h"
#include "noise.h"

///////////////////////////////////////////////////////////////////////////////
///  CCarte
///  Cette classe encapsule l'objet grpahique principal du TP : une carte d'identité de poly
///
///  Base class CModele3DAbstrait La classe de base encapsule tout modèle 3D openGL
///
///
///  @author Frédéric Plourde 
///  @date   2008-03-11 
///
///////////////////////////////////////////////////////////////////////////////
class CCarte :
   public CModele3DAbstrait
{
public:
   /// constructeur spécifique pour création d'une carte ne comportant qu'une seule texture
   CCarte(char * nomFichierTexture, float largeur, float hauteur, int nbColonnes, int nbRangees, float echelle = 1.0);
   /// constructeur spécifique pour création d'une carte comportant plusieurs textures
   CCarte(std::vector<char*> *nomsFichiersTextures, float largeur, float hauteur, int nbColonnes, int nbRangees, float echelle = 1.0);
   /// destructeur par défaut
   ~CCarte(void);

   /// fonction à implémenter pour définir la géométrie openGL de la carte d'identité de poly (est statique, car est référée par un pointeur de fonction)
   static void dessiner();
   /// fonction à implémenter pour personnaliser l'application de texture (à faire aussi : dans le constructeur, appeler "construire(&dessiner, &appliquerTextures)"
   static void appliquerTextures(void*);

private:
   /// fonction personnalisée qui applique le bruit de perlin
   static void appliquerBruitPerlin();

   /// la largeur de la carte, en unités d'objet
   static float largeur_;
   /// la hauteur de la carte, en unités d'objet
   static float hauteur_;
   /// le nombre de divisions verticales du maillage de la carte
   static float nbRangees_;
   /// le nombre de divisions horizontales du maillage de la carte
   static float nbColonnes_;

   /// fonction privée de création du maillage de la carte
   static void dessinerGrilleDeQuads(float largeur, float hauteur, int nbRangees, int nbColonnes, bool normaleVersExt);
};
