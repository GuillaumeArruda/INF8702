///////////////////////////////////////////////////////////////////////////////
///  @file FBO.cpp
///  @author  Olivier Dionne
///  @brief   Définit la classe CFBO implémentant un Frame Buffer Object simple
///           pour openGL
///  @date    2008-10-19
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#include "FBO.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CFBO \n
///
///  Crée un FBO très simple où tous les membres privés sont à 0.
///
///  @return Aucune
///
///  @author Olivier Dionne
///  @date   2008-10-19
///
///////////////////////////////////////////////////////////////////////////////
CFBO::CFBO() :
m_FBO( 0 ),
m_Texture( 0 ),
m_InternalFormat( 0 ),
m_Target( GL_TEXTURE_2D ),
m_TextureW( 0 ),
m_TextureH( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CFBO \n
///
///  Détruit un objet FBO
///
///  @return Aucune
///
///  @author Olivier Dionne
///  @date   2008-10-19
///
///////////////////////////////////////////////////////////////////////////////
CFBO::~CFBO()
{
   Liberer();
}


///////////////////////////////////////////////////////////////////////////////
///  public  Bind \n
///
///  Cette méthode lie la texture interne RGB à la cible openGL courante et active
///  du même coup l'unité de texture spécifiée. C'est la fonction a appeler juste
///  avant de dessiner les primitives sur lesquelles on veut appliquer le FBO
///  en tant que texture.
///  
///  @param [in]       texUnit GLenum   L'Identifiant openGL de l'unité de texture désirée
///
///  @return Aucune
///
///  @author Olivier Dionne, Frédéric Plourde
///  @date   2008-10-19
///
///////////////////////////////////////////////////////////////////////////////
void CFBO::Bind(GLenum texUnit)
{
   glActiveTexture(texUnit);
   glEnable( m_Target );
   glBindTexture( m_Target, m_Texture );
}

///////////////////////////////////////////////////////////////////////////////
///  public  Init \n
///
///  Cette méthode initialise le FBO en créant les noms de texture à l'interne.
///  Pour une mise à jour du FBO à chaque image, cette fonction NE DEVRAIT PAS
///  être appelée constamment, parce qu'elle consomme de la mémoire et est 
///  relativement lente. On conseille donc d'appeler Init() une seule fois au début
///  de l'application, puis simplement d'utiliser la paire CommencerCapture() et 
///  TerminerCapture() afin de mettre à jour le contenu interne du FBO en tout temps.
///  Évidemment, parce qu'il est important que la taille du FBO soit la même que le 
///  viewport courant, si la taille du viewport change en cours d'exécution, il est 
///  impératif d'appeler Init() à nouveau (avec les nouvelles valeurs de w et de h), mais
///  en ayant pris soin au préalable de LIBERER la mémoire du FBO.
///  
///  @param [in]       w int   La largeur du viewport / de la texture interne
///  @param [in]       h int   La hauteur du viewport / de la texture interne
///  @param [in]       format int   Le format interne de la texture du FBO (communément GL_RGB)
///
///  @return Aucune
///
///  @author Olivier Dionne
///  @date   2008-10-19
///
///////////////////////////////////////////////////////////////////////////////
void CFBO::Init( int w, int h, int format )
{
   // Dimensions
   m_TextureW = w;
   m_TextureH = h;
   m_InternalFormat = format;

   // Initialise le FBO
   glGenFramebuffersEXT( 1, &m_FBO );
   glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_FBO );

   // Initialise la partir RGB du FBO
   glGenTextures( 1, &m_Texture );
   glBindTexture( m_Target, m_Texture );
   glTexParameteri( m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexParameteri( m_Target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
   glTexParameteri( m_Target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
   glTexImage2D( m_Target, 0, m_InternalFormat, m_TextureW, m_TextureH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
   glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, m_Target, m_Texture, 0 );

   // Vérification des erreurs
   GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );

   // On vérifie les erreurs à la suite de la création du FBO
   switch( status )
   {
   case GL_FRAMEBUFFER_COMPLETE_EXT:
      break;
   case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
      cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT" << endl;
      break;
   case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
      cerr << "GL_FRAMEBUFFER_UNSUPPORTED_EXT" << endl;
      break;
   case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
      cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT" << endl;
      break;
   case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
      cerr << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT" << endl;
      break;
   case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
      cerr << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT" << endl;
      break;
   case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
      cerr << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT" << endl;
      break;
   case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
      cerr << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT" << endl;
      break;
   default:
      cerr << "ERREUR INCONNUE" << endl;
   }

   glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

///////////////////////////////////////////////////////////////////////////////
///  public  Liberer \n
///
///  Cette fonction libère l'espace mémoire interne du FBO
///
///  @return Aucune
///
///  @author Olivier Dionne
///  @date   2008-10-19
///
///////////////////////////////////////////////////////////////////////////////
void CFBO::Liberer()
{
   if ( m_Texture )
   {
      glDeleteTextures( 1, &m_Texture );
      m_Texture = 0;
   }

   if ( m_FBO )
   {
      glDeleteFramebuffersEXT( 1, &m_FBO );
      m_FBO = 0;
   }
}

///////////////////////////////////////////////////////////////////////////////
///  public  CommencerCapture \n
///
///  Cette fonction débute la définition du contenu du FBO. Son utilisation est
///  très simple et intuitive. Une fois le FBO construit et initialiser avec new ()
///  et Init(), on n'a qu'à insérer les commandes openGL produisant le rendu externe
///  voulu (qui sera enregistré dans le FBO) entre les commandes CommencerCapture() et
///  TerminerCapture();
///
///  @return Aucune
///
///  @author Olivier Dionne, Frédéric Plourde
///  @date   2008-10-19
///
///////////////////////////////////////////////////////////////////////////////
void CFBO::CommencerCapture()
{
   // active la définition du FBO
   glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_FBO );
}

///////////////////////////////////////////////////////////////////////////////
///  public  TerminerCapture \n
///
///  Cette fonction termine la définition du contenu du FBO. Son utilisation est
///  très simple et intuitive. Une fois le FBO construit et initialiser avec new ()
///  et Init(), on n'a qu'à insérer les commandes openGL produisant le rendu externe
///  voulu (qui sera enregistré dans le FBO) entre les commandes CommencerCapture() et
///  TerminerCapture();
///
///  @return Aucune
///
///  @author Olivier Dionne
///  @date   2008-10-19
///
///////////////////////////////////////////////////////////////////////////////
void CFBO::TerminerCapture()
{
   // désactive la définition du FBO
   glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}