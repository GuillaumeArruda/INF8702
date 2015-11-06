///////////////////////////////////////////////////////////////////////////////
///  @file Cst.h
///  @author  Frédéric Plourde 
///  @brief   Déclare les CONSTANTES GLOBALES du programme
///  @date    2007-12-11
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Singleton.h"
#include <GL/glew.h>


///////////////////////////////////////////////
//           AIDE POUR LE TP                 //
///////////////////////////////////////////////
// Ce travail pratique peut être particulièrement
// ardu, surtout si l'on s'enligne dans la 
// mauvaise direction dès le début.
// pour vous faciliter la tâche, on met à votre
// disposition cette définition système 
// "AIDE_POUR_TP" afin de vous permettre de 
// facilement visualiser le contenu RGB de vos
// trois FBO (shadowMaps[]). C'est une étape 
// préliminaire importante qu'il ne vous faut 
// pas sous-estimer. En effet, c'est en étant
// à même de visualiser le contenu RGB de vos FBO
// que vous serez en mesure d'en ajuster les propriétés
// de projection (ouverture, plans near, far, etc..)
// afin d'obtenir la meilleur résolution possible 
// par la suite en mode GL_DEPTH_COMPONENT.
// NOTE : Pour activer le mode AIDE_POUR_TP, vous n'avez
// qu'à décommenter la ligne suivante.
///////////////////////////////////////////////
//#define AIDE_POUR_TP


///////////////////////////////////////////////
// définitions de types globaux              //
///////////////////////////////////////////////
typedef unsigned int uint;

///////////////////////////////////////////////////////////////////////////////
///  colorRGB
///  Cette structure déclare une couleur RGB (r,g,b) sur des float (0..1)
///
///
///  @author Frédéric Plourde @date 2008-03-11
///////////////////////////////////////////////////////////////////////////////
struct colorRGB {
   float r;
   float g;
   float b;
} ;

///////////////////////////////////////////////////////////////////////////////
///  vect3D
///  Cette structure déclare un vecteur 3D (x,y,z)
///
///  @remarks <TODO: insérer une remarque ici>
///
///  @author Frédéric Plourde @date 2008-03-11
///////////////////////////////////////////////////////////////////////////////
struct vect3D {
   float x;
   float y;
   float z;
} ;



///////////////////////////////////////////////////////////////////////////////
///  Point
///  Structure encapsulant un point 3D dans l'espace
///
///
///  @author Frédéric Plourde @date 2008-03-11
///////////////////////////////////////////////////////////////////////////////
struct Point {
	float x;
	float y;
	float z;
};

///////////////////////////////////////////////
// Énumérations globales                     //
///////////////////////////////////////////////


///////////////////////////////////////////////
// déclaration des macros et defines globaux //
///////////////////////////////////////////////

#ifndef M_PI
#define M_PI 3.1415926535897932385
#endif

#define DEG2RAD(a) ((a / 180.0f) * (M_PI))

#define PRODVECT(normale, vect1, vect2) \
	normale.x = (vect1.y*vect2.z) - (vect1.z*vect2.y); \
	normale.y = (vect1.z*vect2.x) - (vect1.x*vect2.z); \
	normale.z = (vect1.x*vect2.y) - (vect1.y*vect2.x)

#define NORMALIZE(normale) \
	{float norm = sqrt((normale.x*normale.x) + \
			          (normale.y*normale.y) + \
					  (normale.z*normale.z)); \
	normale.x = normale.x / norm;  \
	normale.y = normale.y / norm;  \
	normale.z = normale.z / norm;}


#define SUBVECT(result, lhs, rhs) \
	result.x = lhs.x - rhs.x; \
	result.y = lhs.y - rhs.y; \
	result.z = lhs.z - rhs.z;

#define MIN(a,b) a <= b ? a : b
#define MAX(a,b) a >= b ? a : b

#define ASSIGNVECT(res, V1) res.x = V1.x; res.y = V1.y; res.z = V1.z;
#define SUBVECT(res, V1, V2) res.x = V1.x - V2.x; res.y = V1.y - V2.y; res.z = V1.z - V2.z;
#define ADDVECT(res, V1, V2) res.x = V1.x + V2.x; res.y = V1.y + V2.y; res.z = V1.z + V2.z;
#define DIVVECT(V1, a) V1.x /= a; V1.y /= a; V1.z /= a;
#define CITYBLOCKNORM(V1) (V1.x*V1.x) + (V1.y*V1.y) + (V1.z*V1.z);




///////////////////////////////////////////////////////////////////////////
/// @class CCst
/// @brief Les CONSTANTES globales du programme.
///        C'est une classe singleton.
///
/// @author Frédéric Plourde
/// @date 2007-12-11
///////////////////////////////////////////////////////////////////////////
class CCst : public Singleton<CCst>
{
   SINGLETON_DECLARATION_CLASSE(CCst);

public:
   /// spécificateur du mode de séparation de la couleur spéculaire
   static const GLfloat gl_separate_specular_color;

   /// la largeur initiale de la fenêtre GLUT
   static const int g_nWidth;
   /// la hauteur initiale de la fenêtre GLUT
   static const int g_nHeight;

   /// la fréquence de rotation en X du modèle
   static const float freqRotX;            
   /// la fréquence de rotation en Y du modèle
   static const float freqRotY;       
   /// la fréquence de rotation en Z du modèle
   static const float freqRotZ;   

   /// la fréquence de rotation de la lumière 0 (point)
   static const float freqRotLum0;       
   /// la fréquence de rotation de la lumière 1 (directionnelle)
   static const float freqRotLum1;

   /// le framerate cible de l'application graphique.          
   static const float frameRate;

   /// le nombre de lumières openGL de la scène
   static const int nbLumieresOpenGL;

   /// la largeur du modèle 3D
   static const float largeurModele;
   /// la hauteur du modèle 3D
   static const float hauteurModele; 
   /// le nombre de colonnes (quads) dans la grille de quads
   static const int   nbColonnes;
   /// le nombre de rangées (quads) dans la grille de quads
   static const int   nbRangees;
   /// la grandeur du skybox
   static const float grandeurSkybox;
   /// la largeur du gazon (patch Bspline)
   static const float largeurGazon;
   /// la longueur du gazon (patch Bspline)
   static const float longueurGazon;
   /// la hauteur du gazon (patch Bspline)
   static const float hauteurGazon;

   /// le facteur multiplicatif des axes unitaires dessinés dans la scène
   static const float axeScaleFact;

   /// la couleur du FOG dans l'application
   static const GLfloat fogColor[];


   /// la constante "0" pour l'unité de texture 0
   static const int texUnit_0;
   /// la constante "1" pour l'unité de texture 1
   static const int texUnit_1;
   /// la constante "2" pour l'unité de texture 2
   static const int texUnit_2;
   /// la constante "3" pour l'unité de texture 3
   static const int texUnit_3;
   /// la constante "4" pour l'unité de texture 4
   static const int texUnit_4;

   /// les différents paramètres pour le filtrage mipmaping
   static const GLuint mapFilters[5];

   /// le vecteur des GLenum d'unités de textures
   static const GLenum unitesTextures[32];

   /// l'index unique de l'attribut de tangente
   static const GLuint indexTangente;

   /// les points de contrôle de la patch BSpline du gazon
   static const Point Points[32]; 

   /// le vecteur nodal en U de la patch BSpline (gazon)
   static const float VNodalU[12];

   /// le vecteur nodal en V de la patch BSpline (gazon)
   static const float VNodalV[8];

   /// la largeur du shadow map (si == -1, alors c'est la taille du viewport qui en décide)
   static const int      largeurShadowMap;

   /// la hauteur du shadow map (si == -1, alors c'est la taille du viewport qui en décide)
   static const int	 hauteurShadowMap;

};
