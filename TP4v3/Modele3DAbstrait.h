///////////////////////////////////////////////////////////////////////////////
///  @file Modele3DAbstrait.h
///  @author  Frédéric Plourde
///  @brief   Déclare la classe CModele3DAbstrait qui encapsule un modèle 3D
///           openGL dont le graphisme est spécifié par l'entremise d'un 
///           pointeur de fonction
///  @date    2007-12-13
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TextureAbstraite.h"
#include <GL/glut.h> 
#include <vector>


///////////////////////////////////////////////////////////////////////////////
///  @class CModele3DAbstrait
///  @brief Cette classe encapsule tout objet graphique openGL.
///
///
///  @remarks Pour créer un objet graphique (utilisant une liste d'affichage openGL), 
///           on devrait DÉRIVER une classe personnalisé de CModele3DAbstrait. la
///           fonction protégée "construire" est disponible à toute classe dérivée
///           et doit impérativement être appelée à la construction de la classe
///           dérivée afin de fournir à la classe de base abstraite les pointeurs
///           de fonction sur la fonction d'affichage (au moins) et la fonction
///           de d'application des textures (faccult.).  Le graphisme "glBegin...glEnd"
///           devrait donc toujours faire partie de la classe dérivée et être pointée
///           par le pointeur de fonction "dessinerFunc" passé à "construire()".
///
///  @author Frédéric Plourde 
///  @date   2008-03-11   <TODO: insérer initiales ici> <TODO: insérer le texte ici> 
///
///////////////////////////////////////////////////////////////////////////////
class CModele3DAbstrait
{
public:
   /// constructeur créant un modèle 3D générique comportant une seule texture
   CModele3DAbstrait(float echelle, char * nomFichierTexture, TEXTypeEnum texType, bool compilerTex = true, bool mipmap = false);
   /// constructeur créant un modèle 3D générique comportant plusieurs textures
   CModele3DAbstrait(float echelle, std::vector<char*> *nomsFichiersTextures, TEXTypeEnum texType, bool compilerTex = true, bool mipmap = false);
   /// destructeur par défaut
   ~CModele3DAbstrait(void);

   /// fonction à appeler pour commander l'affichage de l'objet (appel de la liste d'affichage)
   void afficher();
   /// ajoute une texture spécifique (préconstruite)au modèle
   void ajouterTexture(CTextureAbstraite* texture);

   //////////////////////////////////////////
   // Les Accesseurs...                     /
   //////////////////////////////////////////
   /// obtient l'échelle du modèle 3D
   inline float obtenirEchelle() const;
   /// modifie l'échelle du modèle 3D
   inline void modifierEchelle(float echelle);
   /// obtient une référence sur la texture
   inline std::vector<CTextureAbstraite*>* obtenirTextures();

protected:
   /// Compile la liste d'affichage en fonction de plusieurs conditions de création
   virtual void compilerListeAffichage();
   /// Construit les différentes textures spécifiées et lance la compilation de la liste d'Affichage
   virtual void construire(void (*dessinerFunc)(void), void (*texturerFunc)(void*) = NULL, bool remplacerTexturage = false);

   /// la liste des textures openGL du modèle 3D
   std::vector<CTextureAbstraite*> textures_;

private:
   /// indique si le texturage personnalisé doit remplacer le texturage par défaut
   bool remplacerTexturage_;
   /// le type de textures associées à l'objet (GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, etc...)
   TEXTypeEnum texType_;
   /// l'identificateur openGL de la liste d'affichage associée au modèle 3D
   GLuint listeAffichage_;
   /// facteur d'échelle général appliqué dans la liste d'affichage (glScale) lors de la compilation
   float  echelle_;
   /// doit-on mipmapper ?
   bool mipmap_;
   /// flag disant si oui ou non les textures seront inclues dans la liste d'Affichage ou non (laissées à la discrétion du l'usager)
   bool   compilerTex_;
   /// la liste des noms de fichiers des textures
   std::vector<char*> nomsFichiersTextures_;

   void (*dessinerFunc_) (void);
   void (*texturerFunc_) (void*);
};



////////////////////////////////
// Définitions inline
////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///  inline public constant  obtenirEchelle \n
///
///  obtient l'échelle du modèle 3D
///
///  @param [in]       Aucun
///  @author Frédéric Plourde 
///  @date   2008-02-18
///
///////////////////////////////////////////////////////////////////////////////
inline float CModele3DAbstrait::obtenirEchelle() const
{
   return echelle_;
}


///////////////////////////////////////////////////////////////////////////////
///  inline public  modifierEchelle \n
///
///  // modifie l'échelle du modèle 3D
///
///  @param [in]       echelle  float  la nouvelle échelle du modèle 3D
///
///  @author Frédéric Plourde 
///  @date   2008-02-18
///
///////////////////////////////////////////////////////////////////////////////
inline void CModele3DAbstrait::modifierEchelle(float echelle)
{
   echelle_ = echelle;
}



///////////////////////////////////////////////////////////////////////////////
///  inline public constant  obtenirTextures \n
///
///  obtient une référence sur la te du modèle
///
///  @param [in]       Aucun
///  @author Frédéric Plourde 
///  @date   2008-02-18
///
///////////////////////////////////////////////////////////////////////////////
inline std::vector<CTextureAbstraite*>* CModele3DAbstrait::obtenirTextures()
{
   return &textures_;
}

