///////////////////////////////////////////////////////////////////////////////
///  @file FBO.cpp
///  @author  Olivier Dionne
///  @brief   D�finit la classe CFBO impl�mentant un Frame Buffer Object simple
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
///  Cr�e un FBO tr�s simple o� tous les membres priv�s sont � 0.
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
///  D�truit un objet FBO
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
///  Cette m�thode lie la texture interne RGB � la cible openGL courante et active
///  du m�me coup l'unit� de texture sp�cifi�e. C'est la fonction a appeler juste
///  avant de dessiner les primitives sur lesquelles on veut appliquer le FBO
///  en tant que texture.
///  
///  @param [in]       texUnit GLenum   L'Identifiant openGL de l'unit� de texture d�sir�e
///
///  @return Aucune
///
///  @author Olivier Dionne, Fr�d�ric Plourde
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
///  Cette m�thode initialise le FBO en cr�ant les noms de texture � l'interne.
///  Pour une mise � jour du FBO � chaque image, cette fonction NE DEVRAIT PAS
///  �tre appel�e constamment, parce qu'elle consomme de la m�moire et est 
///  relativement lente. On conseille donc d'appeler Init() une seule fois au d�but
///  de l'application, puis simplement d'utiliser la paire CommencerCapture() et 
///  TerminerCapture() afin de mettre � jour le contenu interne du FBO en tout temps.
///  �videmment, parce qu'il est important que la taille du FBO soit la m�me que le 
///  viewport courant, si la taille du viewport change en cours d'ex�cution, il est 
///  imp�ratif d'appeler Init() � nouveau (avec les nouvelles valeurs de w et de h), mais
///  en ayant pris soin au pr�alable de LIBERER la m�moire du FBO.
///  
///  @param [in]       w int   La largeur du viewport / de la texture interne
///  @param [in]       h int   La hauteur du viewport / de la texture interne
///  @param [in]       format int   Le format interne de la texture du FBO (commun�ment GL_RGB)
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

   // TODO :
   // Initialiser le FBO et l'objet de rendu RGB
   // ...
   // ...
   // ...


   // V�rification des erreurs FBO
   // Nous vous fournissons cette v�rification d'erreurs
   // pour que vous arriviez plus ais�ment � d�boguer votre code.
   GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );

   // On v�rifie les erreurs � la suite de la cr�ation du FBO
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
}

///////////////////////////////////////////////////////////////////////////////
///  public  Liberer \n
///
///  Cette fonction lib�re l'espace m�moire interne du FBO
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
   if( m_Profondeur )
   {
      glDeleteRenderbuffersEXT( 1, &m_Profondeur );
      m_Profondeur = 0;
   }
}

///////////////////////////////////////////////////////////////////////////////
///  public  CommencerCapture \n
///
///  Cette fonction d�bute la d�finition du contenu du FBO. Son utilisation est
///  tr�s simple et intuitive. Une fois le FBO construit et initialiser avec new ()
///  et Init(), on n'a qu'� ins�rer les commandes openGL produisant le rendu externe
///  voulu (qui sera enregistr� dans le FBO) entre les commandes CommencerCapture() et
///  TerminerCapture();
///
///  @return Aucune
///
///  @author Olivier Dionne, Fr�d�ric Plourde
///  @date   2008-10-19
///
///////////////////////////////////////////////////////////////////////////////
void CFBO::CommencerCapture()
{
   // TODO :
   // active la d�finition du FBO
   // ...
}

///////////////////////////////////////////////////////////////////////////////
///  public  TerminerCapture \n
///
///  Cette fonction termine la d�finition du contenu du FBO. Son utilisation est
///  tr�s simple et intuitive. Une fois le FBO construit et initialiser avec new ()
///  et Init(), on n'a qu'� ins�rer les commandes openGL produisant le rendu externe
///  voulu (qui sera enregistr� dans le FBO) entre les commandes CommencerCapture() et
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
   // TODO :
   // d�sactive la d�finition du FBO
   // ...
}