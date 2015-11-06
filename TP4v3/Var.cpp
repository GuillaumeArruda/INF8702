///////////////////////////////////////////////////////////////////////////////
///  @file Var.cpp
///  @author  Frédéric Plourde 
///  @brief   Définit les VARIABLES GLOBALES du programme
///  @date    2007-12-11
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#include "Cst.h"
#include "Var.h"

SINGLETON_DECLARATION_CPP(CVar);

int     CVar::g_nWindowID;
int     CVar::currentW		    = 800;
int     CVar::currentH		    = 800;

int     CVar::axesOn                = true;  
int     CVar::fogOn                 = true;   
int     CVar::animModeleOn          = false; 
int     CVar::shadowMapOn           = true;
int	CVar::modeleOn		    = true;
int     CVar::rotAutoOn             = false;   
int     CVar::rotLumOn             = false;  

int     CVar::minFilter             = 0;
int     CVar::magFilter             = 0;

bool    CVar::isPerspective         = true;

double  CVar::theta                 = DEG2RAD(270.0f);
double  CVar::phi                   = DEG2RAD(80.0f);
double  CVar::rho                   = 30.0;  

float   CVar::x;
float   CVar::y;
float   CVar::z;

float   CVar::angleRotX             = 0.0f;
float   CVar::angleRotY             = 0.0f;
float   CVar::angleRotZ             = 0.0f;

float   CVar::angleRotLum[]        = {0.0f, 0.0f, 0.0f};

bool    CVar::premierMouvement      = true;
int     CVar::dernierX              = 0; 
int     CVar::dernierY              = 0;

GLuint  CVar::listeAffModele;