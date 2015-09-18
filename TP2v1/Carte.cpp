///////////////////////////////////////////////////////////////////////////////
///  @file Carte.cpp
///  @author  Frédéric Plourde
///  @brief   Définit la classe CCarte qui encapsule un modele3D spécifique 
///           de carte d'identité de polytechnique
///  @date    2008-02-01
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////

#include "Carte.h"

// initialisation des membres statiques
float CCarte::largeur_;
float CCarte::hauteur_;
float CCarte::nbRangees_;
float CCarte::nbColonnes_;


///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CCarte \n
///  constructeur d'une carte d'identité poly avec 1 seule texture (utile seulement pour 
///  faire des tests du côté openGL... pas utile pour le labo en tant que tel...
///
///  @param [in, out]  nomFichierTexture char *    l'unique nom de fichier de l'unique texture
///  @param [in]       largeur float    largeur de la carte
///  @param [in]       hauteur float    hauteur de la carte
///  @param [in]       nbColonnes int   nombre de colonnes du maillage
///  @param [in]       nbRangees int    nombre de rangées du maillage
///  @param [in]       echelle float [=1.000000]    mise à l'échelle qui sera appliquée automatique (glScale)
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CCarte::CCarte(char * nomFichierTexture, float largeur, float hauteur, int nbColonnes, int nbRangees, float echelle): 
CModele3DAbstrait(echelle, nomFichierTexture, TYPE_TEXTURE2D, true)
{
   // initialisations...
   largeur_ = largeur;
   hauteur_ = hauteur;
   nbColonnes_ = nbColonnes;
   nbRangees_ = nbRangees;

   // il est nécessaire d'appeler "construire" ici pour créer la liste d'affichage
   // et les textures associées
   construire(&dessiner, &appliquerTextures, false);
}


///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CCarte \n
///  constructeur d'une carte d'identité poly avec plusieurs textures. Cette version
///  est utile pour le laboratoire, car on peut spécifier en première texture la texture du Recto, 
///  en deuxième le Verso... pour la troisième, la normal map de perlin, on ne pourra pas la spécifier
///  ici, car elle ne sera pas lue à partir du disque, mais bien construite de façon procédurale
///  dans le programme... donc on devra la construire manuellement.
///
///  @param [in, out]  nomsFichiersTextures std::vector<char*> *    Le vecteur contenant la liste des noms de fichiers. Cette liste est copiée, alors l'appelant peut en disposer.
///  @param [in]       largeur float    largeur de la carte
///  @param [in]       hauteur float    hauteur de la carte
///  @param [in]       nbColonnes int   nombre de colonnes du maillage
///  @param [in]       nbRangees int    nombre de rangées du maillage
///  @param [in]       echelle float [=1.000000]    mise à l'échelle qui sera appliquée automatique (glScale)
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CCarte::CCarte(std::vector<char*> *nomsFichiersTextures, float largeur, float hauteur, int nbColonnes, int nbRangees, float echelle): 
CModele3DAbstrait(echelle, nomsFichiersTextures, TYPE_TEXTURE2D, true)
{
   // initialisations...
   largeur_ = largeur;
   hauteur_ = hauteur;
   nbColonnes_ = nbColonnes;
   nbRangees_ = nbRangees;

   // il est nécessaire d'appeler "construire" ici pour créer la liste d'affichage
   // et les textures associées
   construire(&dessiner, &appliquerTextures, false);
}



///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CCarte \n
///  Destructeur par défaut.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CCarte::~CCarte(void)
{
}


///////////////////////////////////////////////////////////////////////////////
///  public static  appliquerTextures \n
///  Fonction à implémenter si l'utilisateur désire remplacer (override) l'application
///  des textures. Ceci sera surtout utile du côté openGL si un texturage autre que 
///  modulate est désiré... Pour le lab, purement en GLSL, cette fonctionnalité
///  ne sera pas très utile, car on applique la texture DANS les shaders.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
void CCarte::appliquerTextures(void*)
{
   appliquerBruitPerlin();
}


///////////////////////////////////////////////////////////////////////////////
///  public static  dessiner \n
///  Fonction à implémenter absolument si on veut voir apparaître quelque chose !
///  cette fonction sera appelée par la classe de base (CModele3DAbstrait) par 
///  l'entremise d'une pointeur de fonction passé à la construction. On mettra ici
///  uniquement le graphisme associé à l'objet (i.e.  glBegin()...glVertex(), etc...)
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
void CCarte::dessiner()
{
   dessinerGrilleDeQuads(largeur_, hauteur_, nbRangees_, nbColonnes_, true);
}



///////////////////////////////////////////////////////////////////////////////
///  private static  dessinerGrilleDeQuads \n
///  Fonction privée utilitaire appelée à l'interne pour générer la grille de quads 
///  de la carte d'identité.
///
///  @param [in]       largeur float
///  @param [in]       hauteur float
///  @param [in]       nbRangees int
///  @param [in]       nbColonnes in
///  @param [in]       normaleVersExt bool  décrit si les normales seront vers nous ou pas.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
void CCarte::dessinerGrilleDeQuads(float largeur, float hauteur, int nbRangees, int nbColonnes, bool normaleVersExt) 
{
   
   int normFact = normaleVersExt ? 1 : -1;
   
   float startX = -largeur/2.0f;
   float startZ =  hauteur/2.0f;
   float s1, t1, s2, t2;

   float incrementX = largeur / (float)nbColonnes;
   float incrementZ = hauteur / (float)nbRangees;

   glBegin(GL_QUADS);
   glNormal3f(0.0, normFact, 0.0);
   glVertexAttrib3f(CCst::indexTangente, 1.0, 0.0, 0.0);

   for (int i=0;i<nbColonnes;i++) {
      for (int k=0;k<nbRangees;k++) {

         s1 = (i*incrementX)/largeur;
         s2 = ((i+1)*incrementX)/largeur;
         t1 = (k*incrementZ)/hauteur;
         t2 = ((k+1)*incrementZ)/hauteur;

         glMultiTexCoord2f(GL_TEXTURE0, s1, t1); 
         glMultiTexCoord2f(GL_TEXTURE1, s1, t1);       
         glMultiTexCoord2f(GL_TEXTURE2, s1, t1);       
         glVertex3f(startX + i*incrementX, 0.0f, startZ - k*incrementZ);

         glMultiTexCoord2f(GL_TEXTURE0, s2, t1); 
         glMultiTexCoord2f(GL_TEXTURE1, s2, t1);
         glMultiTexCoord2f(GL_TEXTURE2, s2, t1);
         glVertex3f((startX + i*incrementX) + incrementX, 0.0f, startZ - k*incrementZ);

         glMultiTexCoord2f(GL_TEXTURE0, s2, t2); 
         glMultiTexCoord2f(GL_TEXTURE1, s2, t2); 
         glMultiTexCoord2f(GL_TEXTURE2, s2, t2); 
         glVertex3f((startX + i*incrementX) + incrementX, 0.0f, (startZ - k*incrementZ) - incrementZ);

         glMultiTexCoord2f(GL_TEXTURE0, s1, t2); 
         glMultiTexCoord2f(GL_TEXTURE1, s1, t2); 
         glMultiTexCoord2f(GL_TEXTURE2, s1, t2);   
         glVertex3f(startX + i*incrementX, 0.0f, (startZ - k*incrementZ) - incrementZ);
      }
   }
   glEnd();
}


void CCarte::appliquerBruitPerlin()
{
   glActiveTexture(GL_TEXTURE2);
   glBindTexture(GL_TEXTURE_2D, CVar::perlinTex);
   glEnable(GL_TEXTURE_2D);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}