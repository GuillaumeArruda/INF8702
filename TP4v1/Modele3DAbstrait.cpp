///////////////////////////////////////////////////////////////////////////////
///  @file Modele3DAbstrait.cpp
///  @author  Frédéric Plourde
///  @brief   Définit la classe CModele3DAbstrait qui encapsule un modèle 3D
///           openGL dont le graphisme est spécifié par l'entremise d'un 
///           pointeur de fonction
///  @date    2007-12-13
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////

#include "Modele3DAbstrait.h"
#include "Texture2D.h"
#include "TextureCubemap.h"
#include "Skybox.h"
#include <cstring>
#include "Cst.h"


///////////////////////////////////////////////////////////////////////////////
///  CModele3DAbstrait::CModele3DAbstrait(float echelle, char * nomFichierTexture, TEXTypeEnum texType = TYPE_TEXTURE2D, bool compilerTex = true)
///  Construit un modèle 3D openGL générique au et ne comportant qu'une seule texture interne
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CModele3DAbstrait::CModele3DAbstrait(float echelle, char * nomFichierTexture, TEXTypeEnum texType = TYPE_TEXTURE2D, bool compilerTex, bool mipmap) :
echelle_(echelle), dessinerFunc_(NULL), texturerFunc_(NULL), texType_(texType), compilerTex_(compilerTex), remplacerTexturage_(false), mipmap_(mipmap)
{
   // mémoriser le nom de fichier
   if (nomFichierTexture) {
      if (strlen(nomFichierTexture) >= 1) {
         char * temp;
         temp = new char[strlen(nomFichierTexture)];
         strcpy(temp, nomFichierTexture);
         nomsFichiersTextures_.push_back(temp);
      }
   }
}



///////////////////////////////////////////////////////////////////////////////
///  CModele3DAbstrait::CModele3DAbstrait(float echelle, std::vector<char*> *nomsFichiersTextures, TEXTypeEnum texType = TYPE_TEXTURE2D, bool compilerTex = true)
///  Construit un modèle 3D openGL générique et comportant plusieurs textures spécifiques dans un std::vecteur.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CModele3DAbstrait::CModele3DAbstrait(float echelle, std::vector<char*> *nomsFichiersTextures, TEXTypeEnum texType = TYPE_TEXTURE2D, bool compilerTex, bool mipmap) :
echelle_(echelle), dessinerFunc_(NULL), texturerFunc_(NULL), texType_(texType), compilerTex_(compilerTex), remplacerTexturage_(false), mipmap_(mipmap)
{
   // mémoriser les différents noms de fichiers
   if (nomsFichiersTextures) {
      if (nomsFichiersTextures->size() >= 1) {
         for (uint i = 0; i < nomsFichiersTextures->size(); i++) {
            if (strlen((*nomsFichiersTextures)[i]) >= 1) {
               char * temp;
               temp = new char[strlen((*nomsFichiersTextures)[i])];
               strcpy(temp, (*nomsFichiersTextures)[i]);
               nomsFichiersTextures_.push_back(temp);
            }
         }
      }
   }
}



///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CModele3DAbstrait \n
///  Destructeur par défaut
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CModele3DAbstrait::~CModele3DAbstrait(void)
{
   // libérer la mémoire de texture
   nomsFichiersTextures_.clear();
   textures_.clear();
}



///////////////////////////////////////////////////////////////////////////////
///  protected virtual  construire \n
///  Fonction appelée par le constructeur d'un modèle3D spécialisé (dérivé) pour construire les 
///  textures associées et lancer la compilation de la liste d'affichage.
///
///  @param [in]       dessinerFunc void (*)(void)       le pointeur de fonction pointant sur dessiner.
///  @param [in]       texturerFunc void (*)(void) [=0]  le pointeur de fonction pointant sur appliquerTextures 
///  @param [in]       remplacerTexturage                indique si la fonction de texturage fournie doit remplacer
///                                                      le texturage par défaut. Sinon, il sera appelé ensuite.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DAbstrait::construire(void (*dessinerFunc)(void), void (*texturerFunc)(void*), bool remplacerTexturage)
{
   remplacerTexturage_ = remplacerTexturage;

   // mémoriser les pointeurs de fonction de dessin et de texturage
   dessinerFunc_ = dessinerFunc;
   texturerFunc_ = texturerFunc;

   // créer la texture si spécifiée
   if (nomsFichiersTextures_.size() >= 1) {
      for (uint i = 0; i < nomsFichiersTextures_.size(); i++) {
         switch (texType_) {
            case TYPE_TEXTURE2D : 
               {
               textures_.push_back(new CTexture2D(nomsFichiersTextures_[i], mipmap_));
               break;
               }
            case TYPE_TEXTURE_CUBEMAP : 
               {
               textures_.push_back(new CTextureCubemap(nomsFichiersTextures_[i]));
               break;
               }
         }
      }
   } else {
      // signifie qu'il n'y aura pas de texture d'appliquée.
   }

   // compiler la liste d'affichage définie dans la fonction protégée ::dessiner()
   // déclarée dans l'instance spécialisée
   compilerListeAffichage();
}




///////////////////////////////////////////////////////////////////////////////
///  public  afficher \n
///  Fonction publique appelée par l'utilisateur d'un modèle 3D spécifique pour 
///  afficher l'objet. Appelle simplement la liste d'affichage résidente.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DAbstrait::afficher()
{
   // on appelle simplement la liste d'affichage pour afficher ;-)
   glCallList(listeAffichage_);
}



///////////////////////////////////////////////////////////////////////////////
///  public  ajouterTexture \n
///  Ajoute une texture spécifique (préconstruite) a l'ensemble des textures
///  du modèle.
///
///  @param [in, out]  texture CTextureAbstraite *   le pointeur sur la texture abstraite.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-03-09
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DAbstrait::ajouterTexture(CTextureAbstraite* texture)
{
   textures_.push_back(texture);
}



///////////////////////////////////////////////////////////////////////////////
///  protected virtual  compilerListeAffichage \n
///  compile la liste d'affichage résidente. À noter que les textures spécifiées
///  à l'interne ne sont pas forcément incluses dans la liste. (voir "compilerTex_"
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20n
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DAbstrait::compilerListeAffichage()
{
   // dessiner la pièce
   listeAffichage_ = glGenLists(1);
   glNewList(listeAffichage_ , GL_COMPILE);{

      // Activer les textures si spécifiées et désirées
      if (textures_.size() >= 1) {
         if (compilerTex_) {
            if (!texturerFunc_ || (texturerFunc_ && remplacerTexturage_ == false)) {
               // sinon, texturer de façon générique pour chaque texture
               for (uint i = 0; i < textures_.size(); i++) {

                  textures_[i]->appliquer(WRAP_S | WRAP_T | WRAP_R | MIN_FILTER | MAG_FILTER,
                     GL_CLAMP,   GL_CLAMP,    GL_CLAMP,
                     GL_LINEAR , GL_LINEAR ,
                     0         , 0         ,
                     0         , 0         ,
                     0         , 0         ,
                     mipmap_     , CCst::unitesTextures[i]); 

                  // s'il y a plusieurs textures de spécifiées en couche, appliquer
                  // un modulate générique
                  if (textures_.size() > 1) {
                     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                  }                 
               }   
            }
            // si fonction de texturage spécifée, l'appeler...
            if (texturerFunc_) {
                // ensuite, lier la texture avec la cible de classe GL_TEXTURE_2D
               (texturerFunc_)(this);
            }
         }
      }

      // Générer le graphisme à compiler dans la liste d'affichage
      glPushMatrix(); {

         // appliquer la mise à l'échelle générale
         glScalef(echelle_, echelle_, echelle_);

         // générer le graphisme associé (si spécifié)
         if (dessinerFunc_) {
            (dessinerFunc_)();
         }

      } glPopMatrix();

      // désactiver le texturage pour les cibles utilisées
      for (uint i = 0; i < textures_.size(); i++) {
         textures_[i]->desactiver();
      }
      glActiveTexture(GL_TEXTURE2);
      glDisable(GL_TEXTURE_2D);
   }glEndList();
}