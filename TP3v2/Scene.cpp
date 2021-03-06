#include "Scene.h"
#include <fstream>
#include <math.h>
#include "StringUtils.h"
#include "Triangle.h"
#include "Plan.h"
#include "Quadrique.h"

using namespace std;
using namespace Scene;

#define STRING_CHECKFIND( Buffer, String ) ( Buffer.find( String ) != string::npos )

const REAL CScene::DIM_FILM_CAM = 0.024;

///////////////////////////////////////////////////////////////////////////////
///  private overloaded constructor  CScene \n
///  Description : Constructeur par d�faut
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CScene::CScene( void )
    : m_ResLargeur           ( 0                   )
    , m_ResHauteur           ( 0                   )
    , m_InfoPixel            ( NULL                )
    , m_TextureScene         ( 0                   )
    , m_CouleurArrierePlan   ( CCouleur::NOIR      )
    , m_NbRebondsMax         ( 20                  )
    , m_EnergieMinRayon      ( RENDRE_REEL( 0.01 ) )
    , m_IndiceRefractionScene( RENDRE_REEL( 1.0 )  )
{
    m_Camera.Position    = CVecteur3::ZERO;
    m_Camera.PointVise   = CVecteur3::ZERO;
    m_Camera.Up          = CVecteur3::UNIT_Y;
    m_Camera.Orientation = CMatrice4::IDENTITE;
    m_Camera.Focale      = RENDRE_REEL( 50.0 );
    m_Camera.Angle       = 0;
}

///////////////////////////////////////////////////////////////////////////////
///  private destructor  ~CScene \n
///  Description : Destructeur
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CScene::~CScene( void )
{
    Liberer();
}

///////////////////////////////////////////////////////////////////////////////
///  public  AjouterSurface \n
///  Description : Ajoute une surface � la sc�ne
///
///  @param [in]       Surface Scene::ISurface *const Un pointeur constant � la surface
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::AjouterSurface( ISurface* const Surface )
{
    m_Surfaces.push_back( Surface );
}

///////////////////////////////////////////////////////////////////////////////
///  public  AjouterLumiere \n
///  Description : Ajoute une lumi�re � la sc�ne
///
///  @param [in]       Lumiere Scene::CLumiere *const    Pointeur constant vers une lumi�re
///  
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008   
///
///////////////////////////////////////////////////////////////////////////////
void CScene::AjouterLumiere( CLumiere* const Lumiere )
{
    m_Lumieres.push_back( Lumiere );
}

///////////////////////////////////////////////////////////////////////////////
///  private  Initialize \n
///  Description : Initialise la cam�ra de sc�ne et les surfaces avant de commencer
///                le lancer de rayons
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::Initialiser( void )
{
    InitialiserCamera();
    for( SurfaceIterator it = m_Surfaces.begin(); it != m_Surfaces.end(); it++ )
        ( *it )->Pretraitement();

    m_InfoPixel = new GLfloat[ m_ResLargeur * m_ResHauteur * 3 ];
}

///////////////////////////////////////////////////////////////////////////////
///  public  AjusterResolution \n
///  Description : Ajuster la r�solution de la texture
///
///  @param [in]       ResX const int Resolution en largeut
///  @param [in]       ResY const int Resolution en hauteur
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::AjusterResolution( const int ResLargeur, const int ResHauteur )
{
    m_ResLargeur = ResLargeur;
    m_ResHauteur = ResHauteur;
}

///////////////////////////////////////////////////////////////////////////////
///  public  AjusterNbRebondsMax \n
///  Description : Set maximum ray bounce
///
///  @param [in]       NbRebondsMax const int      Le nombre maximal de rebonds
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::AjusterNbRebondsMax( const int NbRebondsMax )
{
    m_NbRebondsMax = NbRebondsMax;
}

///////////////////////////////////////////////////////////////////////////////
///  public  AjusterEnergieMinimale \n
///  Description : Ajuste l'�nergie minimale du rayon
///
///  @param [in]       EnergieMin const Math3D::REAL    L'�nergie minimale du rayon
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::AjusterEnergieMinimale( const REAL EnergieMin )
{
    m_EnergieMinRayon = EnergieMin;
}

///////////////////////////////////////////////////////////////////////////////
///  public  AjusterIndiceRefraction \n
///  Description : Ajuste l'indice de r�fraction de la sc�ne
///
///  @param [in]       IndiceRefraction const Math3D::REAL   Indice de r�fraction
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::AjusterIndiceRefraction( const REAL IndiceRefraction )
{
    m_IndiceRefractionScene = IndiceRefraction;
}

///////////////////////////////////////////////////////////////////////////////
///  public  TraiterFichierDeScene \n
///  Description : Traite le fichier de donn�es de la sc�ne
///
///  @param [in]       Fichier const char *    Fichier � traiter
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::TraiterFichierDeScene( const char* Fichier )
{
    fstream FichierScene( Fichier, ios::in );

    if( FichierScene.is_open() )
    {
        EtatTraitementScene EtatCourant = TRAITEMENT_SCENE;
        EtatTraitementScene EtatNouveau = TRAITEMENT_SCENE;

        char Line[ NB_MAX_CAR_PAR_LIGNE ];
        string Buffer;
        int count = 0;

        CLumiere* Lumiere = NULL;
        ISurface* Surface = NULL;

        float Val0, Val1, Val2;
        int  R, G, B;

        while( !FichierScene.eof() )
        {
            FichierScene.getline( Line, NB_MAX_CAR_PAR_LIGNE );
            Buffer = Line;
            CStringUtils::Trim( Buffer, " " );
            
            // Passer les lignes vides et les commentaires
            if( Buffer.empty() || Buffer[ 0 ] == '*' )
                continue;
            else
            {
                // V�rifier l'arriv�e d'un nouvel �tat de traitement
                bool EstNouvelObjetScene = true;
                if     ( STRING_CHECKFIND( Buffer, "Lumiere:" ) ) EtatNouveau = TRAITEMENT_LUMIERE;
                else if( STRING_CHECKFIND( Buffer, "Poly:"    ) ) EtatNouveau = TRAITEMENT_TRIANGLE;
                else if( STRING_CHECKFIND( Buffer, "Plane:"   ) ) EtatNouveau = TRAITEMENT_PLAN;
                else if( STRING_CHECKFIND( Buffer, "Quad:"    ) ) EtatNouveau = TRAITEMENT_QUADRIQUE;
                else
                    EstNouvelObjetScene = false;

                if( EstNouvelObjetScene )
                {
                    // Ajouter objet nouvellement trait� � la sc�ne
                    if( EtatCourant != TRAITEMENT_SCENE )
                    {
                        if( EtatCourant == TRAITEMENT_LUMIERE ) 
                            AjouterLumiere( Lumiere );
                        else        
                            AjouterSurface( Surface );

                        Surface = NULL;
                        Lumiere = NULL;
                    }

                    // Substituer le nouvel �tat pour l'ancien
                    EtatCourant = EtatNouveau;
                    
                    // cr�er un nouvel objet de sc�ne puis le remplir
                    switch( EtatCourant )
                    {
                        case TRAITEMENT_LUMIERE:
                            Lumiere = new CLumiere();
                            cout << "[ACTION]: Creation d'une lumiere..." << endl;
                            break;
                        case TRAITEMENT_TRIANGLE:
                            Surface = new CTriangle();
                            cout << "[ACTION]: Creation d'un triangle..." << endl;
                            break;
                        case TRAITEMENT_PLAN:
                            Surface = new CPlan();
                            cout << "[ACTION]: Creation d'un plan..." << endl;
                            break;
                        case TRAITEMENT_QUADRIQUE:
                            Surface = new CQuadrique();
                            cout << "[ACTION]: Creation d'une quadrique..." << endl;
                            break;
                    }
                }
                else
                {
                    // Remplir les informations g�n�riques de l'objet courant
                    if( Surface != NULL )
                    {
                        bool IsGenericSurfaceInfo = true;

                        if( STRING_CHECKFIND( Buffer, "color:" ) )
                        { 
                            sscanf( Buffer.c_str(), "%s %i %i %i", Line, &R, &G, &B );
                            Surface->AjusterCouleur( CCouleur( R, G, B ) );
                        }
                        else if( STRING_CHECKFIND( Buffer, "ambient:" ) )
                        {
                            sscanf( Buffer.c_str(), "%s %f", Line, &Val0 );
                            Surface->AjusterCoeffAmbiant( Val0 );
                        }
                        else if( STRING_CHECKFIND( Buffer, "diffus:" ) )
                        {
                            sscanf( Buffer.c_str(), "%s %f", Line, &Val0 );
                            Surface->AjusterCoeffDiffus( Val0 );
                        }
                        else if( STRING_CHECKFIND( Buffer, "specular:" ) )
                        {
                            sscanf( Buffer.c_str(), "%s %f %f", Line, &Val0, &Val1 );
                            Surface->AjusterCoeffSpeculaire( Val0 );
                            Surface->AjusterCoeffBrillance( Val1 );
                        }
                        else if( STRING_CHECKFIND( Buffer, "reflect:" ) )
                        {
                            sscanf( Buffer.c_str(), "%s %f", Line, &Val0 );
                            Surface->AjusterCoeffReflexion( Val0 );
                        }
                        else if( STRING_CHECKFIND( Buffer, "refract:" ) )
                        {
                            sscanf( Buffer.c_str(), "%s %f %f", Line, &Val0, &Val1 );
                            Surface->AjusterCoeffRefraction( Val0 );
                            Surface->AjusterIndiceRefraction( Val1 );
                        }
                        else if( STRING_CHECKFIND( Buffer, "rotate:" ) )
                        {
                            sscanf( Buffer.c_str(), "%s %f %f %f", Line, &Val0, &Val1, &Val2 );
                            
                            CMatrice4 Transform = Surface->ObtenirTransformation();
                            Transform.RotationAutourDesX( Deg2Rad<REAL>( Val0 ) );
                            Transform.RotationAutourDesY( Deg2Rad<REAL>( Val1 ) );
                            Transform.RotationAutourDesZ( Deg2Rad<REAL>( Val2 ) );
                            Surface->AjusterTransformation( Transform );
                        }
                        else if( STRING_CHECKFIND( Buffer, "translate:" ) )
                        {
                            sscanf( Buffer.c_str(), "%s %f %f %f", Line, &Val0, &Val1, &Val2 );
                            CMatrice4 Transform = Surface->ObtenirTransformation();
                            Transform.Translation( Val0, Val1, Val2 );
                            Surface->AjusterTransformation( Transform );
                        }
                        else if( STRING_CHECKFIND( Buffer, "scale:" ) )
                        {
                            sscanf( Buffer.c_str(), "%s %f %f %f", Line, &Val0, &Val1, &Val2 );
                            CMatrice4 Transform = Surface->ObtenirTransformation();
                            Transform.MiseAEchelle( Val0, Val1, Val2 );
                            Surface->AjusterTransformation( Transform );
                        }
                        else
                            IsGenericSurfaceInfo = false;

                        if( IsGenericSurfaceInfo )
                            continue;
                    }

                    // Remplir les infos sp�cifiques � l'objet
                    switch( EtatCourant )
                    {
                        case TRAITEMENT_SCENE:

                            if( STRING_CHECKFIND( Buffer, "background:" ) )
                            {
                                sscanf( Buffer.c_str(), "%s %i %i %i", Line, &R, &G, &B );
                                AjusterCouleurArrierePlan( CCouleur( R, G, B ) );
                            }
                            else if( STRING_CHECKFIND( Buffer, "origin:" ) )
                            {
                                sscanf( Buffer.c_str(), "%s %f %f %f", Line, &Val0, &Val1, &Val2 );
                                AjusterPositionCamera( CVecteur3( Val0, Val1, Val2 ) );
                            }
                            else if( STRING_CHECKFIND( Buffer, "eye:" ) )
                            {
                                sscanf( Buffer.c_str(), "%s %f %f %f", Line, &Val0, &Val1, &Val2 );
                                AjusterPointViseCamera( CVecteur3( Val0, Val1, Val2 ) );
                            }
                            else if( STRING_CHECKFIND( Buffer, "up:" ) )
                            {
                                sscanf( Buffer.c_str(), "%s %f %f %f", Line, &Val0, &Val1, &Val2 );
                                AjusterVecteurUpCamera( CVecteur3( Val0, Val1, Val2 ) );
                            }
                            
                            
                            break;

                        case TRAITEMENT_LUMIERE:

                            if( STRING_CHECKFIND( Buffer, "position:" ) )
                            {
                                sscanf( Buffer.c_str(), "%s %f %f %f", Line, &Val0, &Val1, &Val2 );
                                Lumiere->SetPosition( CVecteur3( Val0, Val1, Val2 ) );
                            }
                            else if( STRING_CHECKFIND( Buffer, "intens:" ) )
                            {
                                sscanf( Buffer.c_str(), "%s %f", Line, &Val0 );
                                Lumiere->SetIntensity( Val0 );
                            }
                            else if( STRING_CHECKFIND( Buffer, "color:" ) )
                            {
                                sscanf( Buffer.c_str(), "%s %i %i %i", Line, &R, &G, &B );
                                Lumiere->AjusterCouleur( CCouleur( R, G, B ) );    
                            }

                            break;

                        case TRAITEMENT_TRIANGLE:

                            if( STRING_CHECKFIND( Buffer, "point:" ) )
                            {
                                int PtIdx;
                                sscanf( Buffer.c_str(), "%s %i %f %f %f", Line, &PtIdx, &Val0, &Val1, &Val2 );
                                ( ( CTriangle* )Surface )->AjusterPoint( PtIdx, CVecteur3( Val0, Val1, Val2 ) );
                            }

                            break;

                        case TRAITEMENT_PLAN:

                            if( STRING_CHECKFIND( Buffer, "v_linear:" ) )
                            {
                                sscanf( Buffer.c_str(), "%s %f %f %f", Line, &Val0, &Val1, &Val2 );
                                ( ( CPlan* )Surface )->AjusterNormale( CVecteur3( Val0, Val1, Val2 ) );
                            }
                            else if( STRING_CHECKFIND( Buffer, "v_const:" ) )
                            {
                                sscanf( Buffer.c_str(), "%s %f", Line, &Val0 );
                                ( ( CPlan* )Surface )->AjusterConstante( Val0 );
                            }

                            break;

                        case TRAITEMENT_QUADRIQUE:

                            if( STRING_CHECKFIND( Buffer, "v_quad:" ) )
                            {
                                sscanf( Buffer.c_str(), "%s %f %f %f", Line, &Val0, &Val1, &Val2 );
                                ( ( CQuadrique* )Surface )->AjusterQuadratique( CVecteur3( Val0, Val1, Val2 ) );
                            }
                            else if( STRING_CHECKFIND( Buffer, "v_mixte:" ) )
                            {
                                sscanf( Buffer.c_str(), "%s %f %f %f", Line, &Val0, &Val1, &Val2 );
                                ( ( CQuadrique* )Surface )->AjusterMixte( CVecteur3( Val0, Val1, Val2 ) );
                            }
                            else if( STRING_CHECKFIND( Buffer, "v_linear:" ) )
                            {
                                sscanf( Buffer.c_str(), "%s %f %f %f", Line, &Val0, &Val1, &Val2 );
                                ( ( CQuadrique* )Surface )->AjusterLineaire( CVecteur3( Val0, Val1, Val2 ) );
                            }
                            else if( STRING_CHECKFIND( Buffer, "v_const:" ) )
                            {
                                sscanf( Buffer.c_str(), "%s %f", Line, &Val0 );
                                ( ( CQuadrique* )Surface )->AjusterConstante( Val0 );
                            }

                            break;
                    }
                }
            }
        }

        // Fermer le fichier de sc�ne
        FichierScene.close();

        // Ajouter le dernier objet trait�
        if( Surface != NULL ) AjouterSurface( Surface );
        if( Lumiere != NULL ) AjouterLumiere( Lumiere );
    }
    else
        cerr << "[CScene::TraiterFichierDeScene()] : Incapable d'ouvrir " << Fichier << "." << endl;

}

///////////////////////////////////////////////////////////////////////////////
///  public  Liberer \n
///  Description : D�sallouer toutes les donn�es de sc�ne
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::Liberer( void )
{
    // Supprimer les surfaces
    for( SurfaceIterator it = m_Surfaces.begin(); it != m_Surfaces.end(); it++ )
        delete ( *it );
    m_Surfaces.clear();

    // Supprimer les lumi�res
    for( LumiereIterator it = m_Lumieres.begin(); it != m_Lumieres.end(); it++ )
        delete ( *it );
    m_Lumieres.clear();

    // Supprimer le rendu de sc�ne
    glDeleteTextures( 1, &m_TextureScene );
    delete m_InfoPixel;
}

///////////////////////////////////////////////////////////////////////////////
///  public  AjusterPositionCamera \n
///  Description : Ajuster la position de la cam�ra
///
///  @param [in]       Position const Math3D::CVecteur3 & Position
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::AjusterPositionCamera( const CVecteur3& Position )
{
    m_Camera.Position = Position;
}

///////////////////////////////////////////////////////////////////////////////
///  public  AjusterPointViseCamera \n
///  Description : Ajuster le point cis� par la cam�ra
///
///  @param [in]       PointVise const Math3D::CVecteur3 &  PointVise
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::AjusterPointViseCamera( const CVecteur3& PointVise )
{
    m_Camera.PointVise = PointVise;
}

///////////////////////////////////////////////////////////////////////////////
///  public  AjusterVecteurUpCamera \n
///  Description : Ajuster le vecteur up de la cam�ra
///
///  @param [in]       Up const Math3D::CVecteur3 &  Up
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::AjusterVecteurUpCamera( const CVecteur3& Up )
{
    m_Camera.Up = Up;
}

///////////////////////////////////////////////////////////////////////////////
///  public  AjusterCouleurArrierePlan \n
///  Description : Ajuster la couleur de l'arri�re-plan de la sc�ne
///
///  @param [in]       Color const CCouleur &     Couleur ArrierePlan
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::AjusterCouleurArrierePlan( const CCouleur& Couleur )
{
    m_CouleurArrierePlan = Couleur;
}

///////////////////////////////////////////////////////////////////////////////
///  public  ObtenirTextureGL \n
///  Description : Obtenir le rendu de la sc�ne dans une texture openGL
///
///  @return GLuint Texture Id
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
GLuint CScene::ObtenirTextureGL( void )
{
    return m_TextureScene;
}

///////////////////////////////////////////////////////////////////////////////
///  private  InitialiserCamera \n
///  Description : Initialiser l'angle et la matrice "modelView" de la cam�ra
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::InitialiserCamera( void )
{
    // Calculer l'angle de la cam�ra
    //
    //
    //                  lentille                  |
    //                    |                       |
    // �cran/             |                       y2
    // film               |                       |
    // -+---------+-------+-------+---------------+
    //  |         |<- f ->|<- f ->|
    //  y1                |
    //  |                 |
    //  |
    //
    //  <------ d1 ------>|<--------- d2 --------->
    //
    // From: 1 / f   = 1 / d1 + 1 / d2
    //  and: y1 / y2 = d1 / d2
    //  and: d1      = DIM_FILM
    //       y2      = ( d2 / f - 1 ) * DIM_FILM
    //
    // Nous avons besoin de trouver atan( y2 / d2 ) pour l'ouverture du champ
    REAL d2        = CVecteur3::Distance( m_Camera.Position, m_Camera.PointVise );
    REAL y2        = ( d2 / ( m_Camera.Focale * RENDRE_REEL( 0.001 ) ) - 1 ) * DIM_FILM_CAM;
    m_Camera.Angle = ( 360 * atan2( y2 * RENDRE_REEL( 0.5 ) , d2 ) ) / RENDRE_REEL( PI );

    // Calculer l'orientation de la cam�ra
    CVecteur3 N = CVecteur3::Normaliser( m_Camera.Position - m_Camera.PointVise );
    CVecteur3 V = CVecteur3::Normaliser( m_Camera.Up - N * CVecteur3::ProdScal( m_Camera.Up, N ) );
    CVecteur3 U = CVecteur3::ProdVect( V, N );
    
    m_Camera.Orientation = CMatrice4(  U.x,  U.y,  U.z, 0.0f,
                                       V.x,  V.y,  V.z, 0.0f,
                                       N.x,  N.y,  N.z, 0.0f,
                                      0.0f, 0.0f, 0.0f, 1.0f );
}

///////////////////////////////////////////////////////////////////////////////
///  public  LancerRayons \n
///  Description : Lancement des rayons (raytrace) dans la sc�ne courante
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CScene::LancerRayons( void )
{
    Initialiser();

    REAL HalfH        = tan( Deg2Rad<REAL>( m_Camera.Angle * RENDRE_REEL( 0.5 ) ) );
    REAL HalfW        = ( RENDRE_REEL( m_ResLargeur ) / m_ResHauteur ) * HalfH;
    REAL InvResWidth  = RENDRE_REEL( 1.0 ) / m_ResLargeur;
    REAL InvResHeight = RENDRE_REEL( 1.0 ) / m_ResHauteur;

    CRayon   Rayon;
    CCouleur PixelColor;
    int      PixelIdx = 0;

    #pragma omp parallel for private(Rayon, PixelColor, PixelIdx)
    for( int PixY = 0; PixY < m_ResHauteur; PixY++ )
    {
      for( int PixX = 0; PixX < m_ResLargeur; PixX++ )
      {

         // Pr�pare le rayon
         Rayon.AjusterOrigine( m_Camera.Position );

         Rayon.AjusterDirection( CVecteur3( ( 2 * PixX * InvResWidth  - 1 ) * HalfW,
                                            ( 2 * PixY * InvResHeight - 1 ) * HalfH,
                                            -1 ) );

         Rayon.AjusterDirection( CVecteur3::Normaliser( Rayon.ObtenirDirection() * m_Camera.Orientation ) );
         Rayon.AjusterEnergie( 1 );
         Rayon.AjusterNbRebonds( 0 );
         Rayon.AjusterIndiceRefraction( 1 );

         PixelColor = ObtenirCouleur( Rayon );

         PixelIdx = ( PixX + ( PixY * m_ResLargeur ) ) * 3;
         m_InfoPixel[ PixelIdx + 0 ] = PixelColor.r; 
         m_InfoPixel[ PixelIdx + 1 ] = PixelColor.g;
         m_InfoPixel[ PixelIdx + 2 ] = PixelColor.b;
      }
    }

    // Cr�er une texture openGL
    glGenTextures  ( 1, &m_TextureScene );
    glBindTexture  ( GL_TEXTURE_2D, m_TextureScene );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexImage2D   ( GL_TEXTURE_2D, 0, GL_RGBA8, m_ResLargeur, m_ResHauteur, 0, GL_RGB, GL_FLOAT, m_InfoPixel );
}

///////////////////////////////////////////////////////////////////////////////
///  private constant  ObtenirCouleur \n
///  Description : Obtenir la couleur du pixel pour un rayon donn�
///
///  @param [in]       Rayon const CRayon &    Le rayon � tester
///
///  @return const CCouleur La couleur du pixel
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
const CCouleur CScene::ObtenirCouleur( const CRayon& Rayon ) const
{
    CIntersection Result;
    CIntersection Tmp;
    
    for( SurfaceIterator aSurface = m_Surfaces.begin(); aSurface != m_Surfaces.end(); aSurface++ )
    {
        Tmp = ( *aSurface )->Intersection( Rayon );
        if( Tmp.ObtenirDistance() > EPSILON && ( Tmp.ObtenirDistance() < Result.ObtenirDistance() || Result.ObtenirDistance() < 0 ) )
            Result = Tmp;
    }
    
    // S'il n'y aucune intersection, retourner la couleur de l'arri�re-plan
    // Sinon, retourner la couleur � l'intersection
    return ( Result.ObtenirDistance() < 0 ) ? m_CouleurArrierePlan : ObtenirCouleurSurIntersection( Rayon, Result );
}

///////////////////////////////////////////////////////////////////////////////
///  private constant  ObtenirCouleurSurIntersection \n
///  Description : Obtient la couleur � un point d'intersection en particulier
///                Calcule les contributions color�es de toutes les lumi�res, avec
///                les mod�les de Phong et de Gouraud. Aussi, d�pendemment des
///                propri�t�s de la surface en intersection, on r�fl�chi ou in 
///                r�fracte le rayon courant.
///                current ray.
///
///  @param [in]       Rayon const CRayon &    Le rayon � tester
///  @param [in]       Intersection const Scene::CIntersection &    L'ntersection sp�cifi�e
///
///  @return const CCouleur La couleur � l'intersection donn�e
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
const CCouleur CScene::ObtenirCouleurSurIntersection( const CRayon& Rayon, const CIntersection& Intersection ) const
{
    CCouleur  Result = Intersection.ObtenirSurface()->ObtenirCouleur() * Intersection.ObtenirSurface()->ObtenirCoeffAmbiant();
    CVecteur3 IntersectionPoint = Rayon.ObtenirOrigine()+ Intersection.ObtenirDistance() * Rayon.ObtenirDirection();
    
    // Calculer les contribution color�es des toutes les lumi�res dans la sc�ne
    CCouleur LumiereContributions = CCouleur::NOIR;
    CRayon   LumiereRayon;
    for( LumiereIterator uneLumiere = m_Lumieres.begin(); uneLumiere != m_Lumieres.end(); uneLumiere++ )
    {
        // Initialise le rayon de lumi�re (ou rayon d'ombre)
        LumiereRayon.AjusterOrigine( IntersectionPoint );
        LumiereRayon.AjusterDirection( ( *uneLumiere )->GetPosition() - IntersectionPoint );
        LumiereRayon.AjusterEnergie( 1 );
        LumiereRayon.AjusterIndiceRefraction( 1 );

        if( CVecteur3::ProdScal( LumiereRayon.ObtenirDirection(), Intersection.ObtenirNormale() ) > 0.0 )
        {
            // Obtenir la couleur � partir de la lumi�re
            CCouleur Filter         = ObtenirFiltreDeSurface( LumiereRayon );
            CCouleur LumiereCouleur = ( *uneLumiere )->ObtenirCouleur() * Filter;

            // Ajouter la contribution de Gouraud
            REAL GouraudFactor = ( *uneLumiere )->GetIntensity() * Intersection.ObtenirSurface()->ObtenirCoeffDiffus() *
                CVecteur3::ProdScal( Intersection.ObtenirNormale(), LumiereRayon.ObtenirDirection() );
            Result += Intersection.ObtenirSurface()->ObtenirCouleur() * GouraudFactor * LumiereCouleur;

            // Calcul speculaire
            CVecteur3 E = CVecteur3::Normaliser(-Rayon.ObtenirDirection());
            CVecteur3 R = CVecteur3::Reflect(-LumiereRayon.ObtenirDirection(), Intersection.ObtenirNormale());

            REAL brillance = Intersection.ObtenirSurface()->ObtenirCoeffBrillance() == 0 ? 1.0 : Intersection.ObtenirSurface()->ObtenirCoeffBrillance();
            REAL specular = std::pow(std::fmax(CVecteur3::ProdScal(R, E),0.0), brillance);

            Result += LumiereCouleur * specular * Intersection.ObtenirSurface()->ObtenirCoeffSpeculaire();
        }
    }
    return Result;
}

///////////////////////////////////////////////////////////////////////////////
///  private constant  ObtenirFiltreDeSurface \n
///  Description : Obtenir le filtre du mat�riau de la surface. Le coefficient de
///                r�fraction nous indique du m�me coup s'il y a transparence de la 
///                surface.
///
///  @param [in, out]  LumiereRayon CRayon &     Le rayon de lumi�re (ou d'ombre) � tester
///
///  @return const CCouleur Le filtre de couleur
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
const CCouleur CScene::ObtenirFiltreDeSurface( CRayon& LumiereRayon ) const
{
    CCouleur Filter = CCouleur::BLANC;
    CIntersection LumiereIntersection;
   
    REAL Distance = CVecteur3::Norme( LumiereRayon.ObtenirDirection() );
    LumiereRayon.AjusterDirection( LumiereRayon.ObtenirDirection() / Distance );

    // TODO : � COMPL�TER LORS DU VOLET 2...


    // Tester le rayon de lumi�re avec chaque surface de la sc�ne
    // pour v�rifier s'il y a intersection
    CIntersection Result;
    CIntersection Tmp;

    for (SurfaceIterator aSurface = m_Surfaces.begin(); aSurface != m_Surfaces.end(); aSurface++)
    {
        Tmp = (*aSurface)->Intersection(LumiereRayon);
        if (Tmp.ObtenirDistance() > EPSILON)
        {
            Filter *= Tmp.ObtenirSurface()->ObtenirCoeffRefraction() * Tmp.ObtenirSurface()->ObtenirCouleur();
        }
    }

    // S'il y a une intersection appliquer la translucidit� de la surface
    // intersect�e sur le filtre

    return Filter;
}
