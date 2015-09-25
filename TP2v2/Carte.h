///////////////////////////////////////////////////////////////////////////////
///  @file Carte.h
///  @author  Fr�d�ric Plourde
///  @brief   D�clare la classe CCarte qui encapsule un modele3D sp�cifique 
///           de carte d'identit� de polytechnique
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
///  Cette classe encapsule l'objet grpahique principal du TP : une carte d'identit� de poly
///
///  Base class CModele3DAbstrait La classe de base encapsule tout mod�le 3D openGL
///
///
///  @author Fr�d�ric Plourde 
///  @date   2008-03-11 
///
///////////////////////////////////////////////////////////////////////////////
class CCarte :
   public CModele3DAbstrait
{
public:
   /// constructeur sp�cifique pour cr�ation d'une carte ne comportant qu'une seule texture
   CCarte(char * nomFichierTexture, float largeur, float hauteur, int nbColonnes, int nbRangees, float echelle = 1.0);
   /// constructeur sp�cifique pour cr�ation d'une carte comportant plusieurs textures
   CCarte(std::vector<char*> *nomsFichiersTextures, float largeur, float hauteur, int nbColonnes, int nbRangees, float echelle = 1.0);
   /// destructeur par d�faut
   ~CCarte(void);

   /// fonction � impl�menter pour d�finir la g�om�trie openGL de la carte d'identit� de poly (est statique, car est r�f�r�e par un pointeur de fonction)
   static void dessiner();
   /// fonction � impl�menter pour personnaliser l'application de texture (� faire aussi : dans le constructeur, appeler "construire(&dessiner, &appliquerTextures)"
   static void appliquerTextures(void*);

private:
   /// fonction personnalis�e qui applique le bruit de perlin
   static void appliquerBruitPerlin();

   /// la largeur de la carte, en unit�s d'objet
   static float largeur_;
   /// la hauteur de la carte, en unit�s d'objet
   static float hauteur_;
   /// le nombre de divisions verticales du maillage de la carte
   static float nbRangees_;
   /// le nombre de divisions horizontales du maillage de la carte
   static float nbColonnes_;

   /// fonction priv�e de cr�ation du maillage de la carte
   static void dessinerGrilleDeQuads(float largeur, float hauteur, int nbRangees, int nbColonnes, bool normaleVersExt);
};
