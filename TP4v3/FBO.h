///////////////////////////////////////////////////////////////////////////////
///  @file FBO.h
///  @author  Olivier Dionne 
///  @brief   D�clare une classe impl�mentant un "frame buffer object"
///  @date    2008-10-19
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <GL/glew.h>

///////////////////////////////////////////////////////////////////////////////
///  @class CFBO
///  @brief Cette classe encapsule un FBO (Frame Buffer Object) openGL
///
///  @author Olivier Dionne
///  @date   2008-10-19
///
///////////////////////////////////////////////////////////////////////////////
class CFBO
{
public:
   /// Constructeur par d�faut
	CFBO();
   /// destructeur par d�faut
	~CFBO();

	/// Initialise le FBO et cr�e une texture de rendu
	void Init( int w, int h );

	/// Lib�re la m�moire openGL
	void Liberer();
	/// Charge le FBO comme une texture 2D openGL
	void Bind(GLenum texUnit);
   /// Commencement de la capture des instruction openGL r�alisant le contenu du FBO
   void CommencerCapture(); 
   /// fin de la description du contenu du FBO
   void TerminerCapture();
	/// Retourne la largeur de la texture
   inline int GetWidth()  const { return m_TextureW; }
   /// Retourne la hauteur de la texture
	inline int GetHeight() const { return m_TextureH; }

private:

    /// l'ID du FBO
    GLuint m_FBO;

    /// l'ID de la texture RGB interne
    GLuint m_Texture;

    /// l'ID de la texture interne
    GLuint m_Profondeur;

    /// le format interne (commun�ment GL_RGB)
    GLint  m_InternalFormat;

    /// la cible de la texture (commun�ment GL_TEXTURE2D)
    GLenum m_Target;

    /// la hauteur du FBO
    int m_TextureH;

    /// la largeur du FBO
    int m_TextureW;
};

