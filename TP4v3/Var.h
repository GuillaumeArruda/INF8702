///////////////////////////////////////////////////////////////////////////////
///  @file Var.h
///  @author  Frédéric Plourde 
///  @brief   Déclare les VARIABLES GLOBALES du programme
///  @date    2007-12-11
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Singleton.h"
#include <GL/glut.h>


///////////////////////////////////////////////
// Structure de données globales             //
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
/// @author Frédéric Plourde
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

   /// indique si on désire activer l'animation du modèle
   static int animModeleOn;

   /// indique si on désire activer le Shadow Mapping
   static int shadowMapOn;

   /// indique si on désire afficher le modele 3D
   static int modeleOn;

   /// indique si on désire activer la rotation des lumières openGL (lum 0 et 1 seulement)
   static int rotLumOn;

   /// indique si on désire activer la rotation automatique du modèle
   static int rotAutoOn;

   /// Indique le filtre à utiliser pour le minFilter (utilisé comme index à CCst::mapFilters[])
   static int minFilter;

   /// Indique le filtre à utiliser pour le minFilter (utilisé comme index à CCst::mapFilters[])
   static int magFilter;


   /// le ID de la fenêtre graphique GLUT
   static int g_nWindowID; 

   /// Largeur et hauteur courantes de la fenêtre graphique (viewport)
   static int currentW, currentH;  

   /// indique si on est en mode Perspective (true) ou glOrtho (false) 
   static bool isPerspective;



   /// angle de rotation de la caméra en theta (coord. sphériques)
   static double theta;

   /// angle de rotation de la caméra en phi (coord. sphériques)
   static double phi;

   /// angle de rotation de la caméra en rho (coord. sphériques)
   static double rho;    

   /// la position de la camera en X
   static float x;

   /// la position de la camera en Y
   static float y;

   /// la position de la camera en Z
   static float z;	


   /// angle courant de rotation en X du modèle
   static float angleRotX;   

   /// angle courant de rotation en Y du modèle
   static float angleRotY;

   /// angle courant de rotation en Z du modèle
   static float angleRotZ;


   /// angle courant de rotation de la lumière 0 et 1 
   static float angleRotLum[];


   /// indique si la souris fut bougée déjà
   static bool premierMouvement;

   /// la dernière valeur en X de position lue de la souris
   static int dernierX;      

   /// la derniere valeur en Y de position lus de la souris
   static int dernierY;     


   /// déclaration de la liste d'affichage du modèle  
   static GLuint listeAffModele;  

};
