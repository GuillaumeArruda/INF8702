///////////////////////////////////////////////////////////////////////////////
///  @file Lumiere.cpp
///  @author  Frédéric Plourde 
///  @brief   Définit la classe CLumiere qui encapsule une glLight
///  @date    2007-12-11
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#include "Lumiere.h"



///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CLumiere \n
///
///  Crée une lumière openGL ponctuelle orientée (w = 1.0),
///  centrée à (0,0,0) et d'intensité totale I = 1.0 pour
///  toutes les composantes.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2007-12-11
///
///////////////////////////////////////////////////////////////////////////////
CLumiere::CLumiere(void)
{
   lumKD_ = GL_LIGHT0; 
   KA_[0] = 1.0;
   KA_[1] = 1.0;
   KA_[2] = 1.0;
   KA_[3] = 1.0;

   KD_[0] = 1.0;
   KD_[1] = 1.0;
   KD_[2] = 1.0;
   KD_[3] = 1.0;

   KS_[0] = 1.0;
   KS_[1] = 1.0;
   KS_[2] = 1.0;
   KS_[3] = 1.0;

   KE_[0] = 1.0;
   KE_[1] = 1.0;
   KE_[2] = 1.0;
   KE_[3] = 1.0;

   pos_[0] = 0.0;
   pos_[1] = 0.0;
   pos_[2] = 0.0;
   pos_[3] = 1.0;
   estAllumee_ = true;

   spotDir_[0] =  0.0;
   spotDir_[1] =  0.0;
   spotDir_[2] = -1.0;

   spotExp_    = 0.0;
   spotCutoff_ = 180.0;

   constAtt_ = 0.0;
   linAtt_   = 0.0;
   quadAtt_  = 0.0;
}



///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CLumiere \n
///
///  Contructeur d'une lumière spécifique.
///
///  @param [in]       lumKD GLenum   l'Identifiant openGL de la lumière associée.
///  @param [in]       KAr     composante ambiante (rouge)
///  @param [in]       KAg     composante ambiante (verte)
///  @param [in]       KAb     composante ambiante (bleue)
///  @param [in]       KDr     composante diffuse (rouge)
///  @param [in]       KDg     composante diffuse (verte)
///  @param [in]       KDb     composante diffuse (bleue)
///  @param [in]       KSr     composante spéculaire (rouge)
///  @param [in]       KSg     composante spéculaire (verte)
///  @param [in]       KSb     composante spéculaire (bleue)
///  @param [in]       KEr     composante émettrice (rouge)
///  @param [in]       KEg     composante émettrice (verte)
///  @param [in]       KEb     composante émettrice (bleue)
///  @param [in]       posX    position de la lumière en X
///  @param [in]       posY    position de la lumière en Y
///  @param [in]       posZ    position de la lumière en Z
///  @param [in]       posW = 1,000000  [=1,000000]  position de la lumière en W (directionnelle = 0.0, orientée = 1.0)
///  @param [in]       estActivee indique si la lumière s'active (s'allume) dès la construction
///  @param [in]       spotDirX   [0.0]     la direction en X du "spot" le cas échéant
///  @param [in]       spotDirY   [0.0]     la direction en Y du "spot" le cas échéant
///  @param [in]       spotDirZ   [-1.0]    la direction en Z du "spot" le cas échéant
///  @param [in]       spotExp    [0.0]     l'exposant du "spot" le cas échéant
///  @param [in]       spotCutoff [180.0]  l'angle de coupure du "spot" le cas échéant
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2007-12-11
///
///////////////////////////////////////////////////////////////////////////////
CLumiere::CLumiere(GLenum lumKD, GLfloat KAr, GLfloat KAg, GLfloat KAb, GLfloat KDr, GLfloat KDg, GLfloat KDb, GLfloat KSr, GLfloat KSg, GLfloat KSb, GLfloat KEr, GLfloat KEg, GLfloat KEb, GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat posW, bool estActivee, GLfloat spotDirX, GLfloat spotDirY, GLfloat spotDirZ, GLfloat spotExp, GLfloat spotCutoff)
{
   lumKD_ = lumKD; 
   KA_[0] = KAr;
   KA_[1] = KAg;
   KA_[2] = KAb;
   KA_[3] = 1.0;

   KD_[0] = KDr;
   KD_[1] = KDg;
   KD_[2] = KDb;
   KD_[3] = 1.0;

   KS_[0] = KSr;
   KS_[1] = KSg;
   KS_[2] = KSb;
   KS_[3] = 1.0;

   KE_[0] = KEr;
   KE_[1] = KEg;
   KE_[2] = KEb;
   KE_[3] = 1.0;

   pos_[0] = posX;
   pos_[1] = posY;
   pos_[2] = posZ;
   pos_[3] = posW;
   estAllumee_ = estActivee;

   spotDir_[0] =  spotDirX;
   spotDir_[1] =  spotDirY;
   spotDir_[2] =  spotDirZ;

   spotExp_    = spotExp;
   spotCutoff_  = spotCutoff;

   constAtt_ = 1.0;
   linAtt_   = 0.0;
   quadAtt_  = 0.0;
}



CLumiere::~CLumiere(void)
{
}



///////////////////////////////////////////////////////////////////////////////
///  public  appliquer \n
///
///  Cette méthode active et désactive l'illumination openGL pour cette lumière
///  en regard du membre privé estActive_.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2007-12-11
///
///////////////////////////////////////////////////////////////////////////////
void CLumiere::appliquer()
{
   if (estAllumee_) {
      glLightfv(lumKD_, GL_AMBIENT,  KA_);
      glLightfv(lumKD_, GL_DIFFUSE,  KD_);
      glLightfv(lumKD_, GL_SPECULAR, KS_);
      glLightfv(lumKD_, GL_EMISSION, KE_);
      glLightfv(lumKD_, GL_POSITION, pos_);

      glLightfv(lumKD_, GL_SPOT_DIRECTION, spotDir_);
      glLightf (lumKD_, GL_SPOT_EXPONENT,  spotExp_);
      glLightf (lumKD_, GL_SPOT_CUTOFF,    spotCutoff_);

      glLightf (lumKD_, GL_CONSTANT_ATTENUATION,  constAtt_);
      glLightf (lumKD_, GL_LINEAR_ATTENUATION,  linAtt_);
      glLightf (lumKD_, GL_QUADRATIC_ATTENUATION,  quadAtt_);

      glEnable(lumKD_);
   } else {
      glDisable(lumKD_);
   }
}
