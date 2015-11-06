///////////////////////////////////////////////////////////////////////////////
///  @file TextureCubemap.h
///  @author  Fr�d�ric Plourde 
///  @brief   D�clare la classe CTextureCubemap qui impl�mente une texture
///           cubemap dans openGL
///  @date    2008-01-18
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "textureAbstraite.h"

// d�finition d'une macro utile pour la copie des sous-images
// les parenth�ses autour de (y) et (x) sont hyper importantes
#define lirePixelRGB(x, y) (textureImage.data + ((textureImage.tailleX * (textureImage.tailleY - 1 - (y))) + (x)) * 3)

///////////////////////////////////////////////////////////////////////////////
///  @class CTextureCubemap
///  @brief Cette classe, d�riv�e de CTextureAbstraire, encapsule une texture de
///         cubemap, repr�sentant en fait, 6 textures. Les 6 textures ne SONT PAS
///         charg�es (ou demand�es) s�par�ment, mais doivent �tre en format CROIX
///         (cross format).
///
///  @author Fr�d�ric Plourde
///  @date   2008-10-19
///
///////////////////////////////////////////////////////////////////////////////
class CTextureCubemap :
   public CTextureAbstraite
{
public:
   /// constructeur d'une texture cubemap openGL (de type GL_TEXTURE_CUBE_MAP) � partir du nom de fichier de la texture
   CTextureCubemap(char* nomFichier);
   /// destructeur par d�faut
   ~CTextureCubemap(void);

private:
   /// charge la texture en m�moire avec la cible GL_TEXTURE_CUBE_MAP
   virtual void chargerTexture();
   /// membre priv� temporaire servant la conserver les texels lus du fichier
   Image textureImage;
};
