///////////////////////////////////////////////////////////////////////////////
///  @file Var.h
///  @author  Fr�d�ric Plourde 
///  @brief   D�clare les VARIABLES GLOBALES du programme
///  @date    2007-12-11
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Singleton.h"
#include <GL/glut.h>


///////////////////////////////////////////////
// Structure de donn�es globales             //
///////////////////////////////////////////////

struct Image {
   bool valide;
   int tailleX;
   int tailleY;
   GLubyte *data;
};


///////////////////////////////////////////////////////////////////////////
/// @class CVar
/// @brief Les VARIABLES globales du programme.
///        C'est une classe singleton.
///
/// @author Fr�d�ric Plourde
/// @date 2007-12-11
///////////////////////////////////////////////////////////////////////////
class CVar : public Singleton<CVar>
{
   SINGLETON_DECLARATION_CLASSE(CVar);

public:
   /// indique si l'on veut afficher les axes
   static int axesOn;    

   /// indique si l'on veut activer le brouillard
   static int fogOn;

   /// indique si on d�sire activer l'animation du mod�le
   static int animModeleOn;

   /// indique si on d�sire activer le Shadow Mapping
   static int shadowMapOn;

   /// indique si on d�sire afficher le modele 3D
   static int modeleOn;

   /// indique si on d�sire activer la rotation des lumi�res openGL (lum 0 et 1 seulement)
   static int rotLumOn;

   /// indique si on d�sire activer la rotation automatique du mod�le
   static int rotAutoOn;

   /// Indique le filtre � utiliser pour le minFilter (utilis� comme index � CCst::mapFilters[])
   static int minFilter;

   /// Indique le filtre � utiliser pour le minFilter (utilis� comme index � CCst::mapFilters[])
   static int magFilter;


   /// le ID de la fen�tre graphique GLUT
   static int g_nWindowID; 

   /// Largeur et hauteur courantes de la fen�tre graphique (viewport)
   static int currentW, currentH;  

   /// indique si on est en mode Perspective (true) ou glOrtho (false) 
   static bool isPerspective;



   /// angle de rotation de la cam�ra en theta (coord. sph�riques)
   static double theta;

   /// angle de rotation de la cam�ra en phi (coord. sph�riques)
   static double phi;

   /// angle de rotation de la cam�ra en rho (coord. sph�riques)
   static double rho;    

   /// la position de la camera en X
   static float x;

   /// la position de la camera en Y
   static float y;

   /// la position de la camera en Z
   static float z;	


   /// angle courant de rotation en X du mod�le
   static float angleRotX;   

   /// angle courant de rotation en Y du mod�le
   static float angleRotY;

   /// angle courant de rotation en Z du mod�le
   static float angleRotZ;


   /// angle courant de rotation de la lumi�re 0 et 1 
   static float angleRotLum[];


   /// indique si la souris fut boug�e d�j�
   static bool premierMouvement;

   /// la derni�re valeur en X de position lue de la souris
   static int dernierX;      

   /// la derniere valeur en Y de position lus de la souris
   static int dernierY;     


   /// d�claration de la liste d'affichage du mod�le  
   static GLuint listeAffModele;  

};
