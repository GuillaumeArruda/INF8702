///////////////////////////////////////////////////////////////////////////////
///  @file Cst.cpp
///  @author  Frédéric Plourde 
///  @brief Définit les CONSTANTES GLOBALES du programme
///  @date    2007-12-11
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#include "Cst.h"

SINGLETON_DECLARATION_CPP(CCst);

const GLfloat  CCst::gl_separate_specular_color = GL_SEPARATE_SPECULAR_COLOR;

const int      CCst::g_nWidth        = 640;
const int      CCst::g_nHeight       = 480;

const float    CCst::freqRotX        = 0.15f;             
const float    CCst::freqRotY        = 0.1f;              
const float    CCst::freqRotZ        = 0.2f;              

const float    CCst::freqRotLum0     = 0.1f;              
const float    CCst::freqRotLum1     = 0.2f; 

const float    CCst::frameRate       = 50.0f;         

const int      CCst::nbLumieresOpenGL= 3;

const float    CCst::largeurModele   = 20.0f;         
const float    CCst::hauteurModele   = 15.0f;    
const int      CCst::largeurShadowMap= 512;
const int      CCst::hauteurShadowMap= 512;
const float    CCst::grandeurSkybox  = 2000.0f;
const float    CCst::largeurGazon    = 360.0f;
const float    CCst::longueurGazon   = 360.0f;
const float    CCst::hauteurGazon    = 50.0f;             
const int      CCst::nbColonnes      = 96;               
const int      CCst::nbRangees       = 60;               
const float    CCst::axeScaleFact    = 15.0f;        
const GLfloat  CCst::fogColor[]      = {0.0, 0.0, 0.0};   

const int      CCst::texUnit_0       = 0;
const int      CCst::texUnit_1       = 1;
const int      CCst::texUnit_2       = 2;
const int      CCst::texUnit_3       = 3;
const int      CCst::texUnit_4       = 4;
    
const GLuint   CCst::mapFilters[5]   = {GL_NEAREST,
                                        GL_LINEAR ,
                                        GL_LINEAR_MIPMAP_NEAREST,
                                        GL_NEAREST_MIPMAP_LINEAR,
                                        GL_LINEAR_MIPMAP_LINEAR};

const GLenum   CCst::unitesTextures[32]  = {GL_TEXTURE0 ,
                                            GL_TEXTURE1 ,
                                            GL_TEXTURE2 ,
                                            GL_TEXTURE3 ,
                                            GL_TEXTURE4 ,
                                            GL_TEXTURE5 ,
                                            GL_TEXTURE6 ,
                                            GL_TEXTURE7 ,
                                            GL_TEXTURE8 ,
                                            GL_TEXTURE9 ,
                                            GL_TEXTURE10 ,
                                            GL_TEXTURE11 ,
                                            GL_TEXTURE12 ,
                                            GL_TEXTURE13 ,
                                            GL_TEXTURE14 ,
                                            GL_TEXTURE15 ,
                                            GL_TEXTURE16 ,
                                            GL_TEXTURE17 ,
                                            GL_TEXTURE18 ,
                                            GL_TEXTURE19 ,
                                            GL_TEXTURE20 ,
                                            GL_TEXTURE21 ,
                                            GL_TEXTURE22 ,
                                            GL_TEXTURE23 ,
                                            GL_TEXTURE24 ,
                                            GL_TEXTURE25 ,
                                            GL_TEXTURE26 ,
                                            GL_TEXTURE27 ,
                                            GL_TEXTURE28 ,
                                            GL_TEXTURE29 ,
                                            GL_TEXTURE30 ,
                                            GL_TEXTURE31 };

const GLuint CCst::indexTangente = 1;

const Point CCst::Points[32] = { 
   // V0...++y
   { 0,3,0 },      // U0... -x
   { 1,3,0 },      // U1
   { 2,3,-2 },     // U2
   { 3,3,-0.1f },   // U3
   { 4,3,-2.5f },   // U4
   { 5,3,-1.5f },   // U5
   { 6,3,-1.2f },   // U6
   { 7,3,0 }       // U7... +x
   ,
   // V1...+y
   { 0,2,0 },   // U0... -x
   { 1,2,2 },   // U1
   { 2,2,0 },   // U2
   { 3,2,0 },   // U3
   { 4,2,0 },   // U4
   { 5,2,0 },   // U5
   { 6,2,1.5f }, // U6
   { 7,2,0 }    // U7... +x
   ,
   // V2...-y
   { 0,1,0 },   // U0... -x
   { 1,1,1.5f }, // U1
   { 2,1,0 },   // U2
   { 3,1,0 },   // U3
   { 4,1,0 },   // U4
   { 5,1,0 },   // U5
   { 6,1,2 },   // U6
   { 7,1,0 }    // U7... +x
   ,
   // V3...--y
   { 0,0,0 },      // U0... -x
   { 1,0,-1.1f },   // U1
   { 2,0,-2.0f },   // U2
   { 3,0,-2.3f },   // U3
   { 4,0,-1.9f },   // U4
   { 5,0,-1.8f },   // U5
   { 6,0,-1.1f },   // U6
   { 7,0,0 }       // U7... +x
};

const float CCst::VNodalU[12]      = { 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f };
const float CCst::VNodalV[8]       = { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};