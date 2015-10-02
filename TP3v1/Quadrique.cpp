#include "Quadrique.h"
#include <algorithm>

using namespace Scene;

///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CQuadrique \n
///  Description : Constructeur par défaut
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CQuadrique::CQuadrique( void )
    : ISurface     (                  )
    , m_Quadratique( CVecteur3::ZERO  )
    , m_Lineaire   ( CVecteur3::ZERO  )
    , m_Mixte      ( CVecteur3::ZERO  )
    , m_Cst        ( RENDRE_REEL( 0 ) )
{}

///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CQuadrique \n
///  Description : Constructeur par défaut
///
///  @param [in]       Quadric const Scene::CQuadrique &   la quadrique à copier
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CQuadrique::CQuadrique( const CQuadrique& Quadric )
    : ISurface     ( Quadric               )
    , m_Quadratique( Quadric.m_Quadratique )
    , m_Lineaire   ( Quadric.m_Lineaire    )
    , m_Mixte      ( Quadric.m_Mixte       )
    , m_Cst        ( Quadric.m_Cst         )
{}

///////////////////////////////////////////////////////////////////////////////
///  public virtual destructor  ~CQuadrique \n
///  Description : Destructeur
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CQuadrique::~CQuadrique( void )
{
}

///////////////////////////////////////////////////////////////////////////////
///  public  operator = \n
///  Description : Opérateur de copie
///
///  @param [in]       Quadric const Scene::CQuadrique &    La quadrique à copier
///
///  @return Scene::CQuadrique & La quadrique modifiée
///
///  @author Olivier Dionne 
///  @date   14/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CQuadrique& CQuadrique::operator = ( const CQuadrique& Quadric )
{
    ISurface::operator =( Quadric );
    m_Quadratique = Quadric.m_Quadratique;
    m_Lineaire    = Quadric.m_Lineaire;
    m_Mixte       = Quadric.m_Mixte;
    m_Cst         = Quadric.m_Cst;
    return ( *this );
}

///////////////////////////////////////////////////////////////////////////////
///  protected virtual constant  AfficherInfoDebug \n
///  Description : Implémente le déboguage polymorphique par flux de sortie
///
///  @param [in, out]  Out std::ostream &    Le flux de sortie
///
///  @return std::ostream & Le flux de sortie modifié
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
ostream& CQuadrique::AfficherInfoDebug( ostream& Out ) const
{
    Out << "[DEBUG]: Quadric.Quadratique       = " << m_Quadratique << endl;
    Out << "[DEBUG]: Quadric.Lineaire          = " << m_Lineaire    << endl;
    Out << "[DEBUG]: Quadric.Mixte             = " << m_Mixte       << endl;
    Out << "[DEBUG]: Quadric.Constante         = " << m_Cst;
    return Out;
}

///////////////////////////////////////////////////////////////////////////////
///  public virtual  Pretraitement \n
///  Description : Pretraitement des données de la quadrique( appelé AVANT le lancer)
///
///  @return None
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
void CQuadrique::Pretraitement( void )
{
   // Algorithme tiré de ... 
   // R. Goldman, "Two Approach to a Computer Model for Quadric Surfaces",
   // IEEE CG&A, Sept 1983, pp.21
   
    REAL A = m_Quadratique.x;
	REAL B = m_Quadratique.y;
    REAL C = m_Quadratique.z;
    REAL D = m_Mixte.z    * RENDRE_REEL( 0.5 );
	REAL E = m_Mixte.x    * RENDRE_REEL( 0.5 );
	REAL F = m_Mixte.y    * RENDRE_REEL( 0.5 );
	REAL G = m_Lineaire.x * RENDRE_REEL( 0.5 );
	REAL H = m_Lineaire.y * RENDRE_REEL( 0.5 );
	REAL J = m_Lineaire.z * RENDRE_REEL( 0.5 );
	REAL K = m_Cst;

	CMatrice4 Q( A, D, F, G,
			     D, B, E, H,
			     F, E, C, J,
			     G, H, J, K );

    CMatrice4 Inverse = m_Transformation.Inverse();

    Q = Inverse * Q * Inverse.Transpose();

    m_Quadratique.x = Q[ 0 ][ 0 ];
    m_Quadratique.y = Q[ 1 ][ 1 ];
	m_Quadratique.z = Q[ 2 ][ 2 ];
	m_Cst           = Q[ 3 ][ 3 ];
	m_Mixte.x       = Q[ 1 ][ 2 ] * RENDRE_REEL( 2.0 );
	m_Mixte.y       = Q[ 0 ][ 2 ] * RENDRE_REEL( 2.0 );
	m_Mixte.z       = Q[ 0 ][ 1 ] * RENDRE_REEL( 2.0 );
	m_Lineaire.x    = Q[ 0 ][ 3 ] * RENDRE_REEL( 2.0 );
	m_Lineaire.y    = Q[ 1 ][ 3 ] * RENDRE_REEL( 2.0 );
    m_Lineaire.z    = Q[ 2 ][ 3 ] * RENDRE_REEL( 2.0 );
}

///////////////////////////////////////////////////////////////////////////////
///  public virtual  Intersection \n
///  Description : Effectue l'intersection Rayon/Quadrique
///
///  @param [in]       Rayon const CRayon &    Le rayon à tester
///
///  @return Scene::CIntersection Le résultat de l'ntersection
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CIntersection CQuadrique::Intersection( const CRayon& Rayon )
{
    CIntersection Result;

	float a, b, c, d, e, f, g, h, i, j;
	float A, B, C;
	float t0, t1, tf;
	CVecteur3 rd = Rayon.ObtenirDirection();
	CVecteur3 r0 = Rayon.ObtenirOrigine();

	a = m_Quadratique[0];
	e = m_Quadratique[1];
	h = m_Quadratique[2];

	b = m_Mixte[0];
	c = m_Mixte[1];
	f = m_Mixte[2];

	d = m_Lineaire[0];
	g = m_Lineaire[1];
	i = m_Lineaire[2];

	j = m_Cst;

	A = a * rd.x * rd.x + 2 * b * rd.x * rd.y + 2 * c * rd.x * rd.z +
		e * rd.y * rd.y + 2 * f * rd.y * rd.z + h * rd.z * rd.z;

	B = 2 * (a * r0.x * rd.x + b * (r0.x * rd.y + rd.x * r0.y) + c * (r0.x * rd.z + rd.x * r0.z) +
		d * rd.x + e * r0.y * rd.y + f * (r0.y * rd.z + rd.y * r0.z) + g* rd.y +
		h * r0.z * rd.z + i * rd.z);

	C = a * r0.x * r0.x + 2 * b * r0.x * r0.y + 2 * c * r0.x * r0.z + 2 * d * r0.x +
		e * r0.y * r0.y + 2 * f * r0.y * r0.z + 2 * g * r0.y +
		h * r0.z * r0.z + 2 * i * r0.z + j;

	if (A != 0.f)
	{
		t0 = (-B + sqrt(B * B - 4 * A * C)) / (2 * A);
		t1 = (-B - sqrt(B * B - 4 * A * C)) / (2 * A);

		if (t0 > 0)
		{
			if(t1 > 0)
			{
				tf = min(t0, t1);
			}
			else
			{
				tf = t0;
			}
		}
		else 
		{
			if (t1 > 0)
			{
				tf = t1;
			}
			else 
			{
				// No intersection
				return Result;
			}
		}
	}
	else 
	{
		tf = -B / C;
	}

	//CVecteur3 intersection(r0[0] + rd[0] * tf, r0[1] + rd[1] * tf, r0[2] + rd[2] * tf);
	CVecteur3 intersection(r0 + rd * tf);
	Result.AjusterDistance(CVecteur3::Norme(intersection - r0));

	CVecteur3 normale;
	normale.x = 2 * (a * intersection.x + b * intersection.y + c * intersection.z + d);
	normale.y = 2 * (b * intersection.x + e * intersection.y + f * intersection.z + g);
	normale.x = 2 * (c * intersection.x + f * intersection.y + h * intersection.z + i);

	Result.AjusterNormale(CVecteur3::ProdScal(normale, rd) > 0 ? normale : -normale);

	Result.AjusterSurface(this);

    return Result;
}

///////////////////////////////////////////////////////////////////////////////
///  public virtual constant  Copier \n
///  Description : Alloue une copie de la quadrique courante
///
///  @return Scene::CQuadrique *la copie de la quadrique
///
///  @author Olivier Dionne 
///  @date   13/08/2008
///
///////////////////////////////////////////////////////////////////////////////
CQuadrique* CQuadrique::Copier( void ) const
{
    return new CQuadrique( *this );
}