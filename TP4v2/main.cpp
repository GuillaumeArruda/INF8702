///////////////////////////////////////////////////////////////////////////////
///  @file main.cpp
///  @brief   le main du programme ProjetNuanceur pour le cours INF8702 de Polytechnique
///  @author  Frédéric Plourde 
///  @date    2007-12-11
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include "textfile.h"
#include "Skybox.h"
#include "Gazon.h"
#include "TV.h"
#include "SurfaceBSplinaire.h"
#include "Lumiere.h"
#include "Cst.h"
#include "Var.h"
#include "NuanceurProg.h"
#include "Materiau.h"
#include "Texture2D.h"
#include "FBO.h"
#include "TextureCubemap.h"
#include "Objparser/OBJParser.h"

///////////////////////////////////////////////
// Énumérations locales                      //
///////////////////////////////////////////////

/// énumération des lumières de la scènes (à utiliser comme INDEX de lumieres[])
enum ENUM_LUM {LumPonctuelle, LumSpot, LumDirectionnelle};



///////////////////////////////////////////////
// LES OBJETS                                //
///////////////////////////////////////////////

// la liste des lumieres openGL de la scène (maximum de 8)
std::vector<CLumiere*> lumieres(CCst::nbLumieresOpenGL);

// les différents matériaux utilisés dans le programme
CMateriau mat_ambiant_model(0.0, 0.0, 0.0, 1.0,  1.0, 1.0, 1.0, 1.0,  1.0, 1.0, 1.0, 1.0,  0.0, 0.0, 0.0, 1.0,  30.0,  GL_FRONT_AND_BACK);
CMateriau nurbs_mat_ambiant_model(0.0, 0.0, 0.0, 1.0,  1.0, 1.0, 1.0, 1.0,  0.0, 0.0, 0.0, 1.0,  0.0, 0.0, 0.0, 1.0,  0.0,    GL_FRONT_AND_BACK);

// les programmes de nuanceurs
CNuanceurProg progNuanceurOpenGL;
CNuanceurProg progNuanceurSkybox("Nuanceurs/skyBoxSommets.glsl", "Nuanceurs/skyBoxFragments.glsl", false); 
CNuanceurProg progNuanceurGazon("Nuanceurs/gazonSommets.glsl", "Nuanceurs/gazonFragments.glsl", false); 
CNuanceurProg progNuanceurTV("Nuanceurs/TVSommets.glsl", "Nuanceurs/TVFragments.glsl", false); 

// Les objets 3D graphiques (à instancier plus tard parce que les textures demandent le contexte graphique)
CSkybox *skybox;
CGazon  *gazon;
CTV  *TV;
CFBO *fbo = NULL;

// objets utilisés pour stocker le modèle 3D OBJ
vector<Vertex>   g_Vertices;
vector<Triangle> g_Triangles;
vector<Quad>     g_Quads;
CVecteur3        g_ObjCentroid;

///////////////////////////////////////////////
// PROTOTYPES DES FONCTIONS DU MAIN          //
///////////////////////////////////////////////
void initialisation (void);
void dessinerScene(bool dessinerObjets = true);
void dessinerOverlay();
void affichage(void);
void attente(void);
void clavier(unsigned char touche, int x, int y);
void clavierSpecial(int touche, int x, int y);
void mouvementSouris(int x, int y);
void sourisClickee(int button, int state, int x, int y);
void redimensionnement(GLsizei w, GLsizei h);
void rafraichirCamera();
void compilerListeAffmur(GLuint &listeID, float largeur, float hauteur, int nbRangees, int nbColonnes, bool normaleVersExt);
void dessinerGrilleDeQuads(float largeur, float hauteur, int nbRangees, int nbColonnes, bool normaleVersExt);
void chargerModeleOBJ();
void rafraichirEclairage();
void rafraichirProjection();
void afficherShaderInfoLog(GLuint obj);
void afficherProgramInfoLog(GLuint obj);
void compilerNuanceurs();


// le main
int main(int argc,char *argv[])
{
   // initialisation de GLUT
   glutInit(&argc, argv);
   // parametrage de l'affichage
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   // taille de la fenetre
   glutInitWindowSize(800, 800);
   // creation de la fenetre
   CVar::g_nWindowID = glutCreateWindow("Projet nuanceur");

   // referencement de la fonction d'affichage
   glutDisplayFunc(affichage);
   // referencement de la fonction d'attente
   glutIdleFunc(attente);
   // referencement de la fonction de gestion des touches
   glutKeyboardFunc(clavier);
   // referencement de la fonction de gestion spéciale des touches extended
   glutSpecialFunc(clavierSpecial);
   // referencement de la fonction de redimensionnement
   glutReshapeFunc (redimensionnement);
   // référencement de la fonction de mouvement de la souris
   glutMotionFunc (mouvementSouris);
   // référencement de la fonction de click de la souris
   glutMouseFunc (sourisClickee);

   // vérification de la version 2.0 d'openGL
   glewInit();
   if (glewIsSupported("GL_VERSION_2_0"))
      printf("Pret pour OpenGL 2.0\n");
   else {
      printf("OpenGL 2.0 n'est pas supporte! \n");
      exit(1);
   }

   // recueillir des informations sur le système de rendu
   GLint max;
   glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max);
   printf("GL_MAX_TEXTURE_UNITS = %d\n", max);

   glGetIntegerv(GL_MAX_VARYING_FLOATS, &max);
   printf("GL_MAX_VARYING_FLOATS = %d\n", max);

   if (!glutExtensionSupported ("GL_EXT_framebuffer_object") ){
      printf("Objets 'Frame Buffer' NON supportes! :-( ... Je quitte !\n");
      exit (1);
   } else {
      printf("Objets 'Frame Buffer' supportes ;-)\n");
   }

   // compiler et lier les nuanceurs
   compilerNuanceurs();

   // initialisation de variables d'état openGL et création des listes
   initialisation();

   // boucle principale de gestion des evenements
   glutMainLoop();

   // on doit faire le ménage... !
   delete lumieres[LumPonctuelle];
   delete lumieres[LumDirectionnelle];
   delete lumieres[LumSpot];
   delete skybox;
   delete gazon;
   if (TV)
      delete TV;
   if (fbo)
      delete fbo;

   // Clean up
   g_Vertices.clear();
   g_Triangles.clear();
   g_Quads.clear();

   // le programme n'arrivera jamais jusqu'ici
   return EXIT_SUCCESS;
}



// initialisation d'openGL
void initialisation (void) {

   ////////////////////////////////////////////////////
   // CONSTRUCTION DES LUMIÈRES OPENGL
   ////////////////////////////////////////////////////

   // LUMIÈRE PONCTUELLE ORKENTÉE (enum : LumPonctuelle)
   lumieres[LumPonctuelle]     = new CLumiere(GL_LIGHT0, 0.0,0.0,0.0, 1.0,0.6,1.0, 0.8,0.4,0.8, 1.0,1.0,1.0, 20.0,0.0,50.0,1.0, true);

   // LUMIÈRE DIRECTIONNELLE (enum : LumDirectionnelle)
   lumieres[LumDirectionnelle] = new CLumiere(GL_LIGHT1, 0.0,0.0,0.0, 0.86,0.69,0.04, 0.81,0.83,0.0, 1.0,1.0,1.0, -20.0,0.0,50.0,0.0, true );

   // LUMIÈRE SPOT (enum : LumSpot)
   lumieres[LumSpot]           = new CLumiere(GL_LIGHT2, 0.0,0.0,0.0, 1.0,1.0,1.0, 1.0,1.0,1.0, 1.0,1.0,1.0, 10.0,30.0,90.0,1.0, true, 0.0, 1.0, 0.0, 100.0, 20.0);

   // Définir les modèle de rendu à utiliser
   glShadeModel(GL_SMOOTH);	

   // fixer la couleur de fond
   glClearColor (0.0, 0.0, 0.0, 1.0);

   // activer les etats openGL
   glEnable(GL_NORMALIZE);
   glEnable(GL_LIGHTING);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_FOG);

   // fonction de profondeur
   glDepthFunc(GL_LEQUAL);

   // amélioration de la perspective
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

   // Définir le modèle d'illumination et autres paramètres
   glLightModelfv(GL_LIGHT_MODEL_COLOR_CONTROL, &CCst::gl_separate_specular_color);
   glLightModelf( GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0 );
   glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 1 );       // two-side mode in openGL
   glEnable(GL_VERTEX_PROGRAM_TWO_SIDE);              // two-side mode in GLSL

   // charger modele 3D
   chargerModeleOBJ();

   // construire le skybox avec les textures
   skybox = new CSkybox("textures/uffizi_cross_LDR.bmp", CCst::grandeurSkybox); 

   // construire la surface bsplinaire
   gazon = new CGazon("textures/gazon.bmp", 1.0, 1.0);

   // les noms de fichier des différentes textures de la TV.
   std::vector<char*> texturesTV;
   texturesTV.push_back("textures/tv.bmp");
   texturesTV.push_back("textures/tvAlpha.bmp");

   // NOUVEAU ! :
   // construire l'objet TV
   TV = new CTV(&texturesTV, 1.0, 1.0);

   // TODO:
   // création du frame buffer object
   // Attention à la taille de votre FBO !
   // ...
   // ...
}


void chargerModeleOBJ()
{
   COBJParser* Parser = new COBJParser();

   if( Parser->OpenFile( CCst::fichierOBJ ) )
   {
      cerr << "Lecture du fichier : " << CCst::fichierOBJ << endl;
      Parser->ParseFile();
      Parser->CloseFile();

      g_Vertices    = Parser->GetVertices();
      g_Triangles   = Parser->GetTriangles();
      g_Quads       = Parser->GetQuads();
      g_ObjCentroid = Parser->GetCentroid();

      cerr << "Nombre de faces : " << ( g_Triangles.size() + g_Quads.size() ) << endl;

      glEnableClientState( GL_VERTEX_ARRAY );
      glEnableClientState( GL_NORMAL_ARRAY );
      glInterleavedArrays( GL_T2F_N3F_V3F, 0, &g_Vertices[ 0 ] );

      cerr << "Chargement termine !!" << endl;
   }

   delete Parser;
}


void dessinerScene(bool dessinerObjets)
{
   //////////////////////////////////////
   // DESSINER LE SKYBOX
   progNuanceurSkybox.activer();
   glPushMatrix(); {
      // effectuer une translation pour placer le skybox toujours au centre
      glTranslated(CVar::x, CVar::y, CVar::z);

      // afficher le graphisme (liste d'affichage)
      skybox->afficher();
   }glPopMatrix();
   ///////////////////////////////////////

   ///////////////////////////////////////
   // DESSINER LA SURFACE NURBS
   progNuanceurGazon.activer();

   // ajouts d'autres uniforms
   if (lumieres[LumPonctuelle]->estAllumee()) {
      progNuanceurGazon.uniform1("pointLightOn", 1);
   } else {
      progNuanceurGazon.uniform1("pointLightOn", 0);
   }

   if (lumieres[LumDirectionnelle]->estAllumee()) {
      progNuanceurGazon.uniform1("dirLightOn", 1);
   } else {
      progNuanceurGazon.uniform1("dirLightOn", 0);
   }

   if (lumieres[LumSpot]->estAllumee()) {
      progNuanceurGazon.uniform1("spotLightOn", 1);
   } else {
      progNuanceurGazon.uniform1("spotLightOn", 0);
   }

   glPushMatrix(); {
      // appliquer les matériaux front and back du modèle
      nurbs_mat_ambiant_model.appliquer();

      glTranslated(CVar::x, CVar::y, CVar::z);

      glTranslatef(0.0, 0.0, 0.0 - 55.0f);
      glScalef(CCst::largeurGazon, CCst::longueurGazon, CCst::hauteurGazon);
      glTranslatef(-3.5f, -1.5f , 0.0);

      // texturer le gazon
      (*gazon->obtenirTextures())[0]->appliquer(WRAP_S | WRAP_T | MIN_FILTER | MAG_FILTER,
         GL_REPEAT,   GL_REPEAT  , 0 ,
         CCst::mapFilters[CVar::minFilter] ,  CCst::mapFilters[CVar::magFilter],
         0         , 0         ,
         0         , 0         ,
         0         , 0         ,
         true     , GL_TEXTURE0);

      // ajouts d'autres uniforms
      if (lumieres[LumPonctuelle]->estAllumee()) {
         progNuanceurGazon.uniform1("pointLightOn", 1);
      } else {
         progNuanceurGazon.uniform1("pointLightOn", 0);
      }

      if (lumieres[LumDirectionnelle]->estAllumee()) {
         progNuanceurGazon.uniform1("dirLightOn", 1);
      } else {
         progNuanceurGazon.uniform1("dirLightOn", 0);
      }

      if (lumieres[LumSpot]->estAllumee()) {
         progNuanceurGazon.uniform1("spotLightOn", 1);
      } else {
         progNuanceurGazon.uniform1("spotLightOn", 0);
      }

      gazon->afficher();
   }glPopMatrix();
   ///////////////////////////////////////

if (dessinerObjets)
{
   ///////////////////////////////////////
   // DESSINER LE MODÈLE 3D en OBJ de la caméra
   
   // on n'utilise PAS de nuanceurs pour cet objet
   progNuanceurOpenGL.activer();

   // ajuster le fog dynamiquement selon la distance à l'observateur
   if (CVar::fogOn) {
      glFogi (GL_FOG_MODE,    GL_LINEAR);
      glFogf (GL_FOG_START,   CVar::rho - (CCst::largeurModele/2.0f));
      glFogf (GL_FOG_END,     CVar::rho + (CCst::hauteurModele/1.0f));
      glFogfv(GL_FOG_COLOR,   CCst::fogColor);
      glFogf (GL_FOG_DENSITY, 1.0);
      glEnable(GL_FOG);
   } else {
      glDisable(GL_FOG);
   }

   glPushMatrix(); {
      // appliquer les matériaux du modèle
      mat_ambiant_model.appliquer();

      // faire tourner le modèle, si désiré
      if (CVar::rotAutoOn) {
         CVar::angleRotX += 360.0 * (CCst::freqRotX / CCst::frameRate);
         CVar::angleRotY += 360.0 * (CCst::freqRotY / CCst::frameRate);
         CVar::angleRotZ += 360.0 * (CCst::freqRotZ / CCst::frameRate);

         if (CVar::angleRotX >= 360.0)
            CVar::angleRotX -= 360.0;
         if (CVar::angleRotY >= 360.0)
            CVar::angleRotY -= 360.0;
         if (CVar::angleRotZ >= 360.0)
            CVar::angleRotZ -= 360.0;
      }	

      glRotated(180.0, 1.0, 0.0, 0.0);
      glRotated(CVar::angleRotX, 1.0, 0.0, 0.0);
      glRotated(CVar::angleRotY, 0.0, 1.0, 0.0);
      glRotated(CVar::angleRotZ, 0.0, 0.0, 1.0);

      // afficher le modèle 3D (OBJ)
      glRotated(-90.0, 1.0, 0.0, 0.0);
      glScaled(10.0, 10.0, 10.0);
      if( !g_Triangles.empty() )
         glDrawElements( GL_TRIANGLES, ( int )g_Triangles.size() * 3, GL_UNSIGNED_INT, &g_Triangles[ 0 ] );
      if( !g_Quads.empty() )
         glDrawElements( GL_QUADS, ( int )g_Quads.size() * 3, GL_UNSIGNED_INT, &g_Quads[ 0 ] );

   }glPopMatrix();
   //////////////////////////////////////


   ///////////////////////////////////////////////////
   // Dessiner les axes pour se retrouver, si désiré
   if (CVar::axesOn) {
      progNuanceurOpenGL.activer(); 

      glPushMatrix(); {
         glDisable(GL_TEXTURE_2D);
         glDisable(GL_LIGHTING);
         glDisable(GL_FOG); 

         glLineWidth(3.0);

         glBegin(GL_LINES);{
            // l'axe des x en Rouge
            glColor4f(1.0, 0.0, 0.0, 1.0);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(1.0*CCst::axeScaleFact, 0.0, 0.0);

            // l'axe des y en Vert
            glColor4f(0.0, 1.0, 0.0, 1.0);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(0.0, 1.0*CCst::axeScaleFact, 0.0);

            // l'axe des z en Bleu
            glColor4f(0.0, 0.0, 1.0, 1.0);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(0.0, 0.0, 1.0*CCst::axeScaleFact);
         }
         glEnd();

         glEnable(GL_FOG);
         glEnable(GL_LIGHTING);
         glEnable(GL_TEXTURE_2D);
      }
      glPopMatrix();
   }
}
   ///////////////////////////////////////////////////
   // flush les derniers vertex du pipeline graphique
   glFlush();
}

void dessinerOverlay()
{
   //////////////////////////////////////
   // DESSINER LA TV
   progNuanceurTV.activer();
   glPushMatrix(); {
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_LIGHTING);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(0.0, 0.0, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

      glTranslated(25.0, -55.0, 0.0);

      // inscrire dans le nuanceur la texture FBO
      fbo->Bind(GL_TEXTURE2);

      TV->afficher();

      glEnable(GL_LIGHTING);
      glEnable(GL_DEPTH_TEST);
   }glPopMatrix();
   ///////////////////////////////////////
}


// fonction d'affichage
void affichage(void)
{
   // TODO :
   // Effectuer deux rendus openGL de la scène.
   // un depuis le point de vue du modèle 3D de la caméra XL1 et 
   // un autre de façon habituelle (déjà fait).
   // ATTENTION!! - n'oubliez pas d'ajuster glViewport pour chacun
   // des rendus afin de produire votre rendu caméraXL1 en plus
   // petit afin d'accélérer l'affichage !

   // effacement de l'ecran et du tampon de profondeur
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // charger matrice de modélisation
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // mettre à jour la caméra openGL
   rafraichirCamera();

   // mettre à jour l'éclairage
   rafraichirEclairage();

   // dessiner la scène
   dessinerScene(); 

   // charger la matrice de projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   // mettre à jour la projection
   rafraichirProjection();

   // NOUVEAU!!
   // dessiner l'overlay du téléviseur.
   dessinerOverlay();

   // utiliser le "double-buffering"
   glutSwapBuffers();
}





///////////////////////////////////////////////////////////////////////////////
///  global public  attente \n
///
///  fonction de rappel appelee lorsque l'application ne fait rien.
///  Elle est utilisée pour l'animation.
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2007-12-14
///
///////////////////////////////////////////////////////////////////////////////
void attente(void)
{
   // une variable pour memoriser le temps a attendre
   static int nWaitUntil = glutGet(GLUT_ELAPSED_TIME);

   // on recupere le temps présent
   int nTimer = glutGet(GLUT_ELAPSED_TIME);
   // et on le compare a l'instant qu'il faut attendre
   if(nTimer >= nWaitUntil)
   {
      // pour rafraichir l'affichage
      glutPostRedisplay();

      // framerate
      nWaitUntil = nTimer + (int)(1000.0f / CCst::frameRate);
   }
}




///////////////////////////////////////////////////////////////////////////////
///  global public  clavier \n
///
///  fonction de rappel pour la gestion du clavier
///
///  @param [in]       touche unsigned char  le caractère ASCII de la touche enfoncée
///  @param [in]       x int    la position en x de la souris au moment du rappel
///  @param [in]       y int    la position en y de la souris au moment du rappel
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2007-12-14
///
///////////////////////////////////////////////////////////////////////////////
void clavier(unsigned char touche, int x, int y)
{
   // traitement des touches q et echap
   switch(touche) {

      // quitter le programme
      case 'q' :
         {
            // destruction de la fenetre GLUT
            glutDestroyWindow(CVar::g_nWindowID);
            // on quitte notre programme
            exit(EXIT_SUCCESS);
            break;
         }


         // permutation de la projection (perspective / ortho)
      case 'p' :
         {
            if (CVar::isPerspective)
               CVar::isPerspective = false;
            else
               CVar::isPerspective = true;
            glutPostRedisplay();
            break;
         }

         // permutation de la lumière "spot"
      case 's' :
         {
            if (lumieres[LumSpot]->estAllumee())
               lumieres[LumSpot]->eteindre();
            else
               lumieres[LumSpot]->allumer();
            glutPostRedisplay();
            break;
         }

         // permutation de la lumière directionnelle
      case 'd' :
         {
            if (lumieres[LumDirectionnelle]->estAllumee())
               lumieres[LumDirectionnelle]->eteindre();
            else
               lumieres[LumDirectionnelle]->allumer();
            glutPostRedisplay();
            break;
         }

         // permutation de la lumière ponctuelle orientée
      case 'l' :
         {
            if (lumieres[LumPonctuelle]->estAllumee())
               lumieres[LumPonctuelle]->eteindre();
            else
               lumieres[LumPonctuelle]->allumer();
            glutPostRedisplay();
            break;
         }

         // permutation du brouillard
      case 'f' :
         {
            if (CVar::fogOn)
               CVar::fogOn = false;
            else
               CVar::fogOn = true;
            glutPostRedisplay();
            break;
         }

         // permutation des axes
      case 'a' :
         {
            if (CVar::axesOn)
               CVar::axesOn = false;
            else
               CVar::axesOn = true;
            glutPostRedisplay();
            break;
         }

         // rotation automatique du modèle
      case 'r' :
         {
            if (CVar::rotAutoOn)
               CVar::rotAutoOn = false;
            else
               CVar::rotAutoOn = true;
            glutPostRedisplay();
            break;
         }

         // animation du modèle
      case ' ' :
         {
            if (CVar::animModeleOn)
               CVar::animModeleOn = false;
            else
               CVar::animModeleOn = true;
            glutPostRedisplay();
            break;
         }

         // faire reculer la caméra (zoom-out)
      case '-' :
         {
            CVar::rho += 2.0f;
            glutPostRedisplay();
            break;
         }

         // faire avancer la caméra (zoom-in)
      case '=' :
         {
            CVar::rho -= 2.0f;
            if (CVar::rho <= 1.0)
               CVar::rho = 1.0f;
            glutPostRedisplay();
            break;
         }

         // permuter le minFilter
      case 'n' :
      case 'N' :
         {
            if (CVar::minFilter >= 4)
               CVar::minFilter = 0;
            else
               CVar::minFilter++;
            glutPostRedisplay();
            break;
         }

         // permuter le magFilter
      case 'm' :
      case 'M' :
         {
            if (CVar::magFilter >= 4)
               CVar::magFilter = 0;
            else
               CVar::magFilter++;
            glutPostRedisplay();
            break;
         }

         // permuter le parallax mapping
      case 'x' :
      case 'X' :
         {
            if (CVar::parallaxOn)
               CVar::parallaxOn = false;
            else
               CVar::parallaxOn = true;
            glutPostRedisplay();
            break;
         }
   }
}




///////////////////////////////////////////////////////////////////////////////
///  global public  clavierSpecial \n
///
///  fonction de rappel pour la gestion du clavier spécial
///
///  @param [in]       touche int    le caractère étendu de la touche enfoncée
///  @param [in]       x int    la position en x de la souris au moment du rappel
///  @param [in]       y int    la position en y de la souris au moment du rappel
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2007-12-14
///
///////////////////////////////////////////////////////////////////////////////
void clavierSpecial(int touche, int x, int y)
{
   // traitement des touches q et echap
   switch(touche) {
      case GLUT_KEY_RIGHT :
         // effectuer une rotation vers la droite autour du cube
         CVar::theta += DEG2RAD(2.0f);
         affichage();
         break;

      case GLUT_KEY_LEFT :
         // effectuer une rotation vers la droite autour du cube
         CVar::theta -= DEG2RAD(2.0f);
         affichage();
         break;

      case GLUT_KEY_UP :
         // effectuer une rotation vers la droite autour du cube
         CVar::phi -= DEG2RAD(2.0f);
         affichage();
         break;

      case GLUT_KEY_DOWN :
         // effectuer une rotation vers la droite autour du cube
         CVar::phi += DEG2RAD(2.0f);
         affichage();
         break;
   }
}




///////////////////////////////////////////////////////////////////////////////
///  global public  redimensionnement \n
///
///  fonction de rappel pour le redimensionnement de la fenêtre graphique
///
///  @param [in]       w GLsizei    nouvelle largeur "w" en pixels
///  @param [in]       h GLsizei    nouvelle hauteur "h" en pixels
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2007-12-14
///
///////////////////////////////////////////////////////////////////////////////
void redimensionnement(GLsizei w, GLsizei h)
{
   CVar::currentW = w;
   CVar::currentH = h;
   glViewport(0, 0, w, h);

   if (fbo)
      delete fbo;

   fbo = new CFBO();
   fbo->Init(CVar::currentW/4, CVar::currentH/4, GL_RGB);

   affichage();
}




///////////////////////////////////////////////////////////////////////////////
///  global public  sourisClickee \n
///
///  fonction de rappel pour la gestion d'un click de la souris
///
///  @param [in]       button int   l'identificateur du bouton de souris qui fut clické
///  @param [in]       state int    l'état du bouton (pressé ou relâché)
///  @param [in]       x int        la position en x de la souris au moment du click
///  @param [in]       y int        la position en y de la souris au moment du click
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2007-12-14
///
///////////////////////////////////////////////////////////////////////////////
void sourisClickee(int button, int state, int x, int y)
{
   if (state == GLUT_DOWN) {
      // on vient de presser la souris
      CVar::dernierX = x;
      CVar::dernierY = y;
   } else {
      // on vient de relacher la souris

   }
}




///////////////////////////////////////////////////////////////////////////////
///  global public  mouvementSouris \n
///
///  fonction de rappel pour la gestion du mouvement de la souris
///
///  @param [in]       x int    nouvelle position en x de la souris
///  @param [in]       y int    nouvelle position en y de la souris
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2007-12-14
///
///////////////////////////////////////////////////////////////////////////////
void mouvementSouris(int x, int y)
{
   CVar::angleRotX += y - CVar::dernierY;
   CVar::angleRotZ -= x - CVar::dernierX;

   CVar::dernierX = x;
   CVar::dernierY = y;
}




//////////////////////////////////////////////////////////
////////////  FONCTIONS POUR LA CAMÉRA ///////////////////
//////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///  global public  rafraichirCamera \n
///
///  Fonction de gestion de la position de la caméra en coordonnées sphériques.
///  Elle s'occuper de trouver les coordonnées x et y de la caméra à partir 
///  des theta et phi courants, puis fixe dans openGL la position de la caméra
///  à l'aide de gluLookAt().
///
///  @return Aucune
///
///  @author Frédéric Plourde 
///  @date   2007-12-14
///
///////////////////////////////////////////////////////////////////////////////
void rafraichirCamera()
{
   CVar::x = CVar::rho*cos(CVar::theta)*sin(CVar::phi);
   CVar::y = CVar::rho*sin(CVar::theta)*sin(CVar::phi);
   CVar::z = CVar::rho*cos(CVar::phi);

   gluLookAt(CVar::x, CVar::y, CVar::z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}



//////////////////////////////////////////////////////////
///////////  FONCTIONS POUR L'ÉCLAIRAGE //////////////////
//////////////////////////////////////////////////////////
void rafraichirEclairage()
{
   // déplacer la lumière "lampe de poche" à l'endroit du glulookat
   lumieres[LumSpot]->modifierPos(CVar::x, CVar::y, CVar::z);

   // déplacer la direction de la lampe de poche vers le centre (0,0,0)
   lumieres[LumSpot]->modifierSpotDir(-CVar::x, -CVar::y, -CVar::z);

   // Mise à jour des différentes lumières de la scène
   for (unsigned int i = 0; i < lumieres.size(); i++) {
      lumieres[i]->appliquer();
   }
}




//////////////////////////////////////////////////////////
//////////  FONCTIONS POUR LA PROJECTION /////////////////
//////////////////////////////////////////////////////////
void rafraichirProjection()
{
   if (CVar::isPerspective) {
      gluPerspective(60.0, (GLfloat)CVar::currentW / (GLfloat) CVar::currentH, 0.1, 2000.0);
   } else {
      if (CVar::currentW <= CVar::currentH) 
         glOrtho (-10.0, 10.0, -10.0*(GLfloat)CVar::currentH/(GLfloat)CVar::currentW, 
         10.0*(GLfloat)CVar::currentH/(GLfloat)CVar::currentW, 0.01, 2000.0);
      else 
         glOrtho (-10.0*(GLfloat)CVar::currentW/(GLfloat)CVar::currentH, 
         10.0*(GLfloat)CVar::currentW/(GLfloat)CVar::currentH, -10.0, 10.0, 0.01, 2000.0);
   }
}


//////////////////////////////////////////////////////////
//////////  FONCTIONS POUR LES NUANCEURS /////////////////
//////////////////////////////////////////////////////////
void compilerNuanceurs () 
{
   // on compiler ici les programmes de nuanceurs qui furent prédéfinis
   progNuanceurSkybox.compilerEtLier();
   progNuanceurGazon.compilerEtLier();
   progNuanceurTV.compilerEtLier();

   // on enregistre les différents uniforms que requerront les nuanceurs 
   progNuanceurSkybox.enregistrerUniformInteger("colorMap",  CCst::texUnit_0);
   progNuanceurGazon.enregistrerUniformInteger("colorMap",   CCst::texUnit_0);
   progNuanceurTV.enregistrerUniformInteger("colorMap",      CCst::texUnit_0);
   progNuanceurTV.enregistrerUniformInteger("alphaMap",      CCst::texUnit_1);
   progNuanceurTV.enregistrerUniformInteger("cameraMap",      CCst::texUnit_2);
}