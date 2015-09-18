///////////////////////////////////////////////////////////////////////////////
///  @file Modele3DAbstrait.cpp
///  @author  Fr�d�ric Plourde
///  @brief   D�finit la classe CModele3DAbstrait qui encapsule un mod�le 3D
///           openGL dont le graphisme est sp�cifi� par l'entremise d'un 
///           pointeur de fonction
///  @date    2007-12-13
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////

#include "Modele3DAbstrait.h"
#include "Texture2D.h"
#include <cstring>
#include "Cst.h"


///////////////////////////////////////////////////////////////////////////////
///  CModele3DAbstrait::CModele3DAbstrait(float echelle, char * nomFichierTexture, TEXTypeEnum texType = TYPE_TEXTURE2D, bool compilerTex = true)
///  Construit un mod�le 3D openGL g�n�rique au et ne comportant qu'une seule texture interne
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CModele3DAbstrait::CModele3DAbstrait(float echelle, char * nomFichierTexture, TEXTypeEnum texType = TYPE_TEXTURE2D, bool compilerTex, bool mipmap) :
echelle_(echelle), dessinerFunc_(NULL), texturerFunc_(NULL), texType_(texType), compilerTex_(compilerTex), remplacerTexturage_(false), mipmap_(mipmap)
{
   // m�moriser le nom de fichier
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
///  Construit un mod�le 3D openGL g�n�rique et comportant plusieurs textures sp�cifiques dans un std::vecteur.
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CModele3DAbstrait::CModele3DAbstrait(float echelle, std::vector<char*> *nomsFichiersTextures, TEXTypeEnum texType = TYPE_TEXTURE2D, bool compilerTex) :
echelle_(echelle), dessinerFunc_(NULL), texturerFunc_(NULL), texType_(texType), compilerTex_(compilerTex), remplacerTexturage_(false)
{
   // m�moriser les diff�rents noms de fichiers
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
///  Destructeur par d�faut
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CModele3DAbstrait::~CModele3DAbstrait(void)
{
   // lib�rer la m�moire de texture
   nomsFichiersTextures_.clear();
   textures_.clear();
}



///////////////////////////////////////////////////////////////////////////////
///  protected virtual  construire \n
///  Fonction appel�e par le constructeur d'un mod�le3D sp�cialis� (d�riv�) pour construire les 
///  textures associ�es et lancer la compilation de la liste d'affichage.
///
///  @param [in]       dessinerFunc void (*)(void)       le pointeur de fonction pointant sur dessiner.
///  @param [in]       texturerFunc void (*)(void) [=0]  le pointeur de fonction pointant sur appliquerTextures 
///  @param [in]       remplacerTexturage                indique si la fonction de texturage fournie doit remplacer
///                                                      le texturage par d�faut. Sinon, il sera appel� ensuite.
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DAbstrait::construire(void (*dessinerFunc)(void), void (*texturerFunc)(void*), bool remplacerTexturage)
{
   remplacerTexturage_ = remplacerTexturage;

   // m�moriser les pointeurs de fonction de dessin et de texturage
   dessinerFunc_ = dessinerFunc;
   texturerFunc_ = texturerFunc;

   // cr�er la texture si sp�cifi�e
   if (nomsFichiersTextures_.size() >= 1) {
      for (uint i = 0; i < nomsFichiersTextures_.size(); i++) {
         switch (texType_) {
            case TYPE_TEXTURE2D : {
                  textures_.push_back(new CTexture2D(nomsFichiersTextures_[i], mipmap_));
                  break;
            }
         }
      }
   } else {
      // signifie qu'il n'y aura pas de texture d'appliqu�e.
   }

   // compiler la liste d'affichage d�finie dans la fonction prot�g�e ::dessiner()
   // d�clar�e dans l'instance sp�cialis�e
   compilerListeAffichage();
}




///////////////////////////////////////////////////////////////////////////////
///  public  afficher \n
///  Fonction publique appel�e par l'utilisateur d'un mod�le 3D sp�cifique pour 
///  afficher l'objet. Appelle simplement la liste d'affichage r�sidente.
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
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
///  Ajoute une texture sp�cifique (pr�construite) a l'ensemble des textures
///  du mod�le.
///
///  @param [in, out]  texture CTextureAbstraite *   le pointeur sur la texture abstraite.
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-03-09
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DAbstrait::ajouterTexture(CTextureAbstraite* texture)
{
   textures_.push_back(texture);
}



///////////////////////////////////////////////////////////////////////////////
///  protected virtual  compilerListeAffichage \n
///  compile la liste d'affichage r�sidente. � noter que les textures sp�cifi�es
///  � l'interne ne sont pas forc�ment incluses dans la liste. (voir "compilerTex_"
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-02-20n
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DAbstrait::compilerListeAffichage()
{
   // dessiner la pi�ce
   listeAffichage_ = glGenLists(1);
   glNewList(listeAffichage_ , GL_COMPILE);{

      // Activer les textures si sp�cifi�es et d�sir�es
      if (textures_.size() >= 1) {
         if (compilerTex_) {
            if (!texturerFunc_ || (texturerFunc_ && remplacerTexturage_ == false)) {
               // sinon, texturer de fa�on g�n�rique pour chaque texture
               for (uint i = 0; i < textures_.size(); i++) {

                  textures_[i]->appliquer(WRAP_S | WRAP_T | WRAP_R | MIN_FILTER | MAG_FILTER,
                     GL_CLAMP,   GL_CLAMP,    GL_CLAMP,
                     GL_LINEAR , GL_LINEAR ,
                     0         , 0         ,
                     0         , 0         ,
                     0         , 0         ,
                     mipmap_     , textures_.size() > 1 ? CCst::unitesTextures[i] : GL_NONE ); 

                  // s'il y a plusieurs textures de sp�cifi�es en couche, appliquer
                  // un modulate g�n�rique
                  if (textures_.size() > 1) {
                     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                  }                 
               }   
            }
            // si fonction de texturage sp�cif�e, l'appeler...
            if (texturerFunc_) {
                // ensuite, lier la texture avec la cible de classe GL_TEXTURE_2D
               (texturerFunc_)(this);
            }
         }
      }

      // G�n�rer le graphisme � compiler dans la liste d'affichage
      glPushMatrix(); {

         // appliquer la mise � l'�chelle g�n�rale
         glScalef(echelle_, echelle_, echelle_);

         // g�n�rer le graphisme associ� (si sp�cifi�)
         if (dessinerFunc_) {
            (dessinerFunc_)();
         }

      } glPopMatrix();

      // d�sactiver le texturage pour les cibles utilis�es
      for (uint i = 0; i < textures_.size(); i++) {
         textures_[i]->desactiver();
      }
      glActiveTexture(GL_TEXTURE2);
      glDisable(GL_TEXTURE_2D);
   }glEndList();
}