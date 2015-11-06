///////////////////////////////////////////////////////////////////////////////
///  @file Mod�le3D-OBJ.cpp
///  @author  Fr�d�ric Plourde
///  @brief   D�finit la classe CModele3DOBJ qui encapsule un modele3D 
///           charg� � partir d'un fichier OBJ
///  @date    2008-10-27
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////

#include "Modele3DOBJ.h"

vector<Vertex>    CModele3DOBJ::g_Vertices_;
vector<Triangle>  CModele3DOBJ::g_Triangles_;
vector<Quad>      CModele3DOBJ::g_Quads_;
CVecteur3         CModele3DOBJ::g_ObjCentroid_;

///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CModele3DOBJ \n
///  constructeur d'un mod�le 3D avec 1 seule texture (utile seulement pour 
///  faire des tests du c�t� openGL... pas utile pour le labo en tant que tel...
///
///  @param [in, out]  nomsFichierOBJ char *	    La cha�ne de caract�res contenant le nom de fichier OBJ du mod�le 3D
///  @param [in, out]  nomFichierTexture char *     l'unique nom de fichier de l'unique texture
///  @param [in]       echelle float [=1.000000]    mise � l'�chelle qui sera appliqu�e automatique (glScale)
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-10-27
///
///////////////////////////////////////////////////////////////////////////////
CModele3DOBJ::CModele3DOBJ(char* nomFichierOBJ, char * nomFichierTexture, float echelle): 
CModele3DAbstrait(echelle, nomFichierTexture, TYPE_TEXTURE2D, true)
{
    // charger le mod�le 3D en m�moire
    chargerModeleOBJ(nomFichierOBJ);

    // il est n�cessaire d'appeler "construire" ici pour cr�er la liste d'affichage
    // et les textures associ�es
    construire(&dessiner, &appliquerTextures, false);
}


///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CModele3DOBJ \n
///  constructeur d'un mod�le 3D avec plusieurs textures. Cette version du constructeur
///  est particuli�rement utile lorsqu'on veut associer plusieurs textures aux diff�rentes
///  unit�es de texture du nuanceur.
///
///  @param [in, out]  nomsFichierOBJ char *        La cha�ne de caract�res contenant le nom de fichier OBJ du mod�le 3D
///  @param [in, out]  nomsFichiersTextures std::vector<char*> *    Le vecteur contenant la liste des noms de fichiers. Cette liste est copi�e, alors l'appelant peut en disposer.
///  @param [in]       echelle float [=1.000000]    mise � l'�chelle qui sera appliqu�e automatique (glScale)
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-10-27
///
///////////////////////////////////////////////////////////////////////////////
CModele3DOBJ::CModele3DOBJ(char* nomFichierOBJ, std::vector<char*> *nomsFichiersTextures, float echelle): 
CModele3DAbstrait(echelle, nomsFichiersTextures, TYPE_TEXTURE2D, true)
{
    // charger le mod�le 3D en m�moire
    chargerModeleOBJ(nomFichierOBJ);

    // il est n�cessaire d'appeler "construire" ici pour cr�er la liste d'affichage
    // et les textures associ�es
    construire(&dessiner, &appliquerTextures, false);
}



///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CModele3DOBJ \n
///  Destructeur par d�faut.
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-10-27
///
///////////////////////////////////////////////////////////////////////////////
CModele3DOBJ::~CModele3DOBJ(void)
{
    // Clean up
    g_Vertices_.clear();
    g_Triangles_.clear();
    g_Quads_.clear();
}


///////////////////////////////////////////////////////////////////////////////
///  public static  chargerModeleOBJ \n
///  Fonction qui charge en m�moire les sommets et les normales d'un mod�le 
///  d�finit dans un fichier .OBJ.
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-10-27
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DOBJ::chargerModeleOBJ(char* fichierOBJ)
{
   COBJParser* Parser = new COBJParser();

   if( Parser->OpenFile( fichierOBJ ) )
   {
      cerr << "Lecture du fichier : " << fichierOBJ << endl;
      Parser->ParseFile();
      Parser->CloseFile();

      g_Vertices_    = Parser->GetVertices();
      g_Triangles_   = Parser->GetTriangles();
      g_Quads_       = Parser->GetQuads();
      g_ObjCentroid_ = Parser->GetCentroid();

      cerr << "Nombre de faces : " << ( g_Triangles_.size() + g_Quads_.size() ) << endl;

      glEnableClientState( GL_VERTEX_ARRAY );
      glEnableClientState( GL_NORMAL_ARRAY );
      glInterleavedArrays( GL_T2F_N3F_V3F, 0, &g_Vertices_[ 0 ] );

      cerr << "Chargement termine !!" << endl;
   }

   delete Parser;
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
///  @date   2008-10-27
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DOBJ::appliquerTextures(void*)
{
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
///  @date   2008-10-27
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DOBJ::dessiner()
{
    afficherModele3D();
}


///////////////////////////////////////////////////////////////////////////////
///  public static  afficherModele3D \n
///  Fonction qui affiche le mod�le 3D � partir des tableaux de sommets lus 
///  dans le fichier source OBJ.
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-10-27
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DOBJ::afficherModele3D()
{
    // afficher le mod�le 3D (OBJ)
    glRotated(-90.0, 1.0, 0.0, 0.0);
    glTranslated(0.0, -10.0, 0.0);
    if( !g_Triangles_.empty() )
	glDrawElements( GL_TRIANGLES, ( int )g_Triangles_.size() * 3, GL_UNSIGNED_INT, &g_Triangles_[ 0 ] );
    if( !g_Quads_.empty() )
	glDrawElements( GL_QUADS, ( int )g_Quads_.size() * 3, GL_UNSIGNED_INT, &g_Quads_[ 0 ] );
}