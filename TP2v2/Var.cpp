///////////////////////////////////////////////////////////////////////////////
///  @file Var.cpp
///  @author  Fr�d�ric Plourde 
///  @brief   D�finit les VARIABLES GLOBALES du programme
///  @date    2007-12-11
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#include "Cst.h"
#include "Var.h"

SINGLETON_DECLARATION_CPP(CVar);

int     CVar::g_nWindowID;
int     CVar::currentW;
int     CVar::currentH;

int     CVar::axesOn                = true;  
int     CVar::fogOn                 = true;   
int     CVar::animModeleOn          = false; 
int     CVar::rotAutoOn             = false;       
int     CVar::perlinOn              = true;  

int     CVar::minFilter             = 0;
int     CVar::magFilter             = 0;

bool    CVar::isPerspective         = true;

double  CVar::theta                 = DEG2RAD(270.0f);
double  CVar::phi                   = DEG2RAD(90.0f);
double  CVar::rho                   = 30.0;  

float   CVar::x;
float   CVar::y;
float   CVar::z;

float   CVar::angleRotX             = 0.0f;
float   CVar::angleRotY             = 0.0f;
float   CVar::angleRotZ             = 0.0f;

bool    CVar::premierMouvement      = true;
int     CVar::dernierX              = 0; 
int     CVar::dernierY              = 0;

GLuint  CVar::listeAffModele;

GLuint  CVar::perlinTex             = 0;