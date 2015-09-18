///////////////////////////////////////////////////////////////////////////////
///  @file Carte.cpp
///  @author  Fr�d�ric Plourde
///  @brief   D�finit la classe CCarte qui encapsule un modele3D sp�cifique 
///           de carte d'identit� de polytechnique
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
///  constructeur d'une carte d'identit� poly avec 1 seule texture (utile seulement pour 
///  faire des tests du c�t� openGL... pas utile pour le labo en tant que tel...
///
///  @param [in, out]  nomFichierTexture char *    l'unique nom de fichier de l'unique texture
///  @param [in]       largeur float    largeur de la carte
///  @param [in]       hauteur float    hauteur de la carte
///  @param [in]       nbColonnes int   nombre de colonnes du maillage
///  @param [in]       nbRangees int    nombre de rang�es du maillage
///  @param [in]       echelle float [=1.000000]    mise � l'�chelle qui sera appliqu�e automatique (glScale)
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
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

   // il est n�cessaire d'appeler "construire" ici pour cr�er la liste d'affichage
   // et les textures associ�es
   construire(&dessiner, &appliquerTextures, false);
}


///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CCarte \n
///  constructeur d'une carte d'identit� poly avec plusieurs textures. Cette version
///  est utile pour le laboratoire, car on peut sp�cifier en premi�re texture la texture du Recto, 
///  en deuxi�me le Verso... pour la troisi�me, la normal map de perlin, on ne pourra pas la sp�cifier
///  ici, car elle ne sera pas lue � partir du disque, mais bien construite de fa�on proc�durale
///  dans le programme... donc on devra la construire manuellement.
///
///  @param [in, out]  nomsFichiersTextures std::vector<char*> *    Le vecteur contenant la liste des noms de fichiers. Cette liste est copi�e, alors l'appelant peut en disposer.
///  @param [in]       largeur float    largeur de la carte
///  @param [in]       hauteur float    hauteur de la carte
///  @param [in]       nbColonnes int   nombre de colonnes du maillage
///  @param [in]       nbRangees int    nombre de rang�es du maillage
///  @param [in]       echelle float [=1.000000]    mise � l'�chelle qui sera appliqu�e automatique (glScale)
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
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

   // il est n�cessaire d'appeler "construire" ici pour cr�er la liste d'affichage
   // et les textures associ�es
   construire(&dessiner, &appliquerTextures, false);
}



///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CCarte \n
///  Destructeur par d�faut.
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
CCarte::~CCarte(void)
{
}


///////////////////////////////////////////////////////////////////////////////
///  public static  appliquerTextures \n
///  Fonction � impl�menter si l'utilisateur d�sire remplacer (override) l'application
///  des textures. Ceci sera surtout utile du c�t� openGL si un texturage autre que 
///  modulate est d�sir�... Pour le lab, purement en GLSL, cette fonctionnalit�
///  ne sera pas tr�s utile, car on applique la texture DANS les shaders.
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
void CCarte::appliquerTextures(void*)
{
   appliquerBruitPerlin();
}


///////////////////////////////////////////////////////////////////////////////
///  public static  dessiner \n
///  Fonction � impl�menter absolument si on veut voir appara�tre quelque chose !
///  cette fonction sera appel�e par la classe de base (CModele3DAbstrait) par 
///  l'entremise d'une pointeur de fonction pass� � la construction. On mettra ici
///  uniquement le graphisme associ� � l'objet (i.e.  glBegin()...glVertex(), etc...)
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-02-20
///
///////////////////////////////////////////////////////////////////////////////
void CCarte::dessiner()
{
   dessinerGrilleDeQuads(largeur_, hauteur_, nbRangees_, nbColonnes_, true);
}



///////////////////////////////////////////////////////////////////////////////
///  private static  dessinerGrilleDeQuads \n
///  Fonction priv�e utilitaire appel�e � l'interne pour g�n�rer la grille de quads 
///  de la carte d'identit�.
///
///  @param [in]       largeur float
///  @param [in]       hauteur float
///  @param [in]       nbRangees int
///  @param [in]       nbColonnes in
///  @param [in]       normaleVersExt bool  d�crit si les normales seront vers nous ou pas.
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
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