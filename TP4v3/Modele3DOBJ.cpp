///////////////////////////////////////////////////////////////////////////////
///  @file Modèle3D-OBJ.cpp
///  @author  Frédéric Plourde
///  @brief   Définit la classe CModele3DOBJ qui encapsule un modele3D 
///           chargé à partir d'un fichier OBJ
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
///  constructeur d'un modèle 3D avec 1 seule texture (utile seulement pour 
///  faire des tests du côté openGL... pas utile pour le labo en tant que tel...
///
///  @param [in, out]  nomsFichierOBJ char *	    La chaîne de caractères contenant le nom de fichier OBJ du modèle 3D
///  @param [in, out]  nomFichierTexture char *     l'unique nom de fichier de l'unique texture
///  @param [in]       echelle float [=1.000000]    mise à l'échelle qui sera appliquée automatique (glScale)
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-10-27
///
///////////////////////////////////////////////////////////////////////////////
CModele3DOBJ::CModele3DOBJ(char* nomFichierOBJ, char * nomFichierTexture, float echelle): 
CModele3DAbstrait(echelle, nomFichierTexture, TYPE_TEXTURE2D, true)
{
    // charger le modèle 3D en mémoire
    chargerModeleOBJ(nomFichierOBJ);

    // il est nécessaire d'appeler "construire" ici pour créer la liste d'affichage
    // et les textures associées
    construire(&dessiner, &appliquerTextures, false);
}


///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CModele3DOBJ \n
///  constructeur d'un modèle 3D avec plusieurs textures. Cette version du constructeur
///  est particulièrement utile lorsqu'on veut associer plusieurs textures aux différentes
///  unitées de texture du nuanceur.
///
///  @param [in, out]  nomsFichierOBJ char *        La chaîne de caractères contenant le nom de fichier OBJ du modèle 3D
///  @param [in, out]  nomsFichiersTextures std::vector<char*> *    Le vecteur contenant la liste des noms de fichiers. Cette liste est copiée, alors l'appelant peut en disposer.
///  @param [in]       echelle float [=1.000000]    mise à l'échelle qui sera appliquée automatique (glScale)
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-10-27
///
///////////////////////////////////////////////////////////////////////////////
CModele3DOBJ::CModele3DOBJ(char* nomFichierOBJ, std::vector<char*> *nomsFichiersTextures, float echelle): 
CModele3DAbstrait(echelle, nomsFichiersTextures, TYPE_TEXTURE2D, true)
{
    // charger le modèle 3D en mémoire
    chargerModeleOBJ(nomFichierOBJ);

    // il est nécessaire d'appeler "construire" ici pour créer la liste d'affichage
    // et les textures associées
    construire(&dessiner, &appliquerTextures, false);
}



///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CModele3DOBJ \n
///  Destructeur par défaut.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
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
///  Fonction qui charge en mémoire les sommets et les normales d'un modèle 
///  définit dans un fichier .OBJ.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
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
///  Fonction à implémenter si l'utilisateur désire remplacer (override) l'application
///  des textures. Ceci sera surtout utile du côté openGL si un texturage autre que 
///  modulate est désiré... Pour le lab, purement en GLSL, cette fonctionnalité
///  ne sera pas très utile, car on applique la texture DANS les shaders.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-10-27
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DOBJ::appliquerTextures(void*)
{
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
///  @date   2008-10-27
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DOBJ::dessiner()
{
    afficherModele3D();
}


///////////////////////////////////////////////////////////////////////////////
///  public static  afficherModele3D \n
///  Fonction qui affiche le modèle 3D à partir des tableaux de sommets lus 
///  dans le fichier source OBJ.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2008-10-27
///
///////////////////////////////////////////////////////////////////////////////
void CModele3DOBJ::afficherModele3D()
{
    // afficher le modèle 3D (OBJ)
    glRotated(-90.0, 1.0, 0.0, 0.0);
    glTranslated(0.0, -10.0, 0.0);
    if( !g_Triangles_.empty() )
	glDrawElements( GL_TRIANGLES, ( int )g_Triangles_.size() * 3, GL_UNSIGNED_INT, &g_Triangles_[ 0 ] );
    if( !g_Quads_.empty() )
	glDrawElements( GL_QUADS, ( int )g_Quads_.size() * 3, GL_UNSIGNED_INT, &g_Quads_[ 0 ] );
}