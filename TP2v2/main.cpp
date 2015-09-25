///////////////////////////////////////////////////////////////////////////////
///  @file main.cpp
///  @brief   le main du programme ProjetNuanceur pour le cours INF8702 de Polytechnique
///  @author  Fr�d�ric Plourde 
///  @date    2007-12-11
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <GL/glew.h>
#include <freeglut.h>
#include "textfile.h"
#include "Skybox.h"
#include "Carte.h"
#include "Gazon.h"
#include "SurfaceBSplinaire.h"
#include "Lumiere.h"
#include "Cst.h"
#include "Var.h"
#include "NuanceurProg.h"
#include "Materiau.h"
#include "Texture2D.h"
#include "TextureCubemap.h"


///////////////////////////////////////////////
// �num�rations locales                      //
///////////////////////////////////////////////

/// �num�ration des lumi�res de la sc�nes (� utiliser comme INDEX de lumieres[])
enum ENUM_LUM {LumPonctuelle, LumSpot, LumDirectionnelle};



///////////////////////////////////////////////
// LES OBJETS                                //
///////////////////////////////////////////////

// la liste des lumieres openGL de la sc�ne (maximum de 8)
std::vector<CLumiere*> lumieres(CCst::nbLumieresOpenGL);

// les diff�rents mat�riaux utilis�s dans le programme
CMateriau front_mat_ambiant_model(0.0, 0.0, 0.0, 1.0,  0.9, 0.8, 1.0, 1.0,  0.5, 0.5, 0.5, 1.0,  0.0, 0.0, 0.0, 1.0,  100.0,  GL_FRONT);
CMateriau back_mat_ambiant_model (0.0, 0.0, 0.0, 1.0,  1.0, 1.0, 1.0, 1.0,  0.0, 0.0, 0.0, 1.0,  0.0, 0.0, 0.0, 1.0,  5.0,    GL_BACK);
CMateriau nurbs_mat_ambiant_model(0.0, 0.0, 0.0, 1.0,  1.0, 1.0, 1.0, 1.0,  0.0, 0.0, 0.0, 1.0,  0.0, 0.0, 0.0, 1.0,  0.0,    GL_FRONT_AND_BACK);

// les programmes de nuanceurs
CNuanceurProg progNuanceurOpenGL;
CNuanceurProg progNuanceurCartePoly("Nuanceurs/cartePolySommets.glsl", "Nuanceurs/cartePolyFragments.glsl", false); 
CNuanceurProg progNuanceurSkybox("Nuanceurs/skyBoxSommets.glsl", "Nuanceurs/skyBoxFragments.glsl", false); 
CNuanceurProg progNuanceurGazon("Nuanceurs/gazonSommets.glsl", "Nuanceurs/gazonFragments.glsl", false); 

// Les objets 3D graphiques (� instancier plus tard parce que les textures demandent le contexte graphique)
CSkybox *skybox;
CCarte  *cartePoly;
CGazon  *gazon;

///////////////////////////////////////////////
// PROTOTYPES DES FONCTIONS DU MAIN          //
///////////////////////////////////////////////
void initialisation (void);
void dessinerScene();
void affichage(void);
void attente(void);
void clavier(unsigned char touche, int x, int y);
void clavierSpecial(int touche, int x, int y);
void mouvementSouris(int x, int y);
void sourisClickee(int button, int state, int x, int y);
void redimensionnement(GLsizei w, GLsizei h);
void rafraichirCamera();
void compilerListeAffCartePoly(GLuint &listeID, float largeur, float hauteur, int nbRangees, int nbColonnes, bool normaleVersExt);
void dessinerGrilleDeQuads(float largeur, float hauteur, int nbRangees, int nbColonnes, bool normaleVersExt);
void rafraichirEclairage();
void rafraichirProjection();
void afficherShaderInfoLog(GLuint obj);
void afficherProgramInfoLog(GLuint obj);
void compilerNuanceurs();
void chargerBruitPerlin();


// le main
int main(int argc,char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

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
    // referencement de la fonction de gestion sp�ciale des touches extended
    glutSpecialFunc(clavierSpecial);
    // referencement de la fonction de redimensionnement
    glutReshapeFunc (redimensionnement);
    // r�f�rencement de la fonction de mouvement de la souris
    glutMotionFunc (mouvementSouris);
    // r�f�rencement de la fonction de click de la souris
    glutMouseFunc (sourisClickee);

    // v�rification de la version 2.0 d'openGL
	glewInit();
	if (glewIsSupported("GL_VERSION_4_0"))
		printf("Pret pour OpenGL 4.0\n");
	else {
		printf("OpenGL 4.0 n'est pas supporte! \n");
		exit(1);
	}

    // recueillir des informations sur le syst�me de rendu
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

    // initialisation de variables d'�tat openGL et cr�ation des listes
    initialisation();

    // boucle principale de gestion des evenements
    glutMainLoop();

    // on doit faire le m�nage... !
    delete lumieres[LumPonctuelle];
    delete lumieres[LumDirectionnelle];
    delete lumieres[LumSpot];
    delete cartePoly;
    delete skybox;
    delete gazon;

    // le programme n'arrivera jamais jusqu'ici
    return EXIT_SUCCESS;
}



// initialisation d'openGL
void initialisation (void) {

    ////////////////////////////////////////////////////
    // CONSTRUCTION DES LUMI�RES OPENGL
    ////////////////////////////////////////////////////

    // LUMI�RE PONCTUELLE ORKENT�E (enum : LumPonctuelle)
    lumieres[LumPonctuelle]     = new CLumiere(GL_LIGHT0, 0.0,0.0,0.0, 1.0,0.5,1.0, 0.8,0.3,0.8, 1.0,1.0,1.0, 20.0,0.0,50.0,1.0, true);

    // LUMI�RE DIRECTIONNELLE (enum : LumDirectionnelle)
    lumieres[LumDirectionnelle] = new CLumiere(GL_LIGHT1, 0.0,0.0,0.0, 0.86,0.69,0.04, 0.81,0.83,0.0, 1.0,1.0,1.0, -20.0,0.0,50.0,0.0, true );

    // LUMI�RE SPOT (enum : LumSpot)
    lumieres[LumSpot]           = new CLumiere(GL_LIGHT2, 0.0,0.0,0.0, 0.6,0.6,0.6, 0.7,0.7,0.7, 1.0,1.0,1.0, 10.0,30.0,90.0,1.0, true, 0.0, 1.0, 0.0, 100.0, 60.0);

    // D�finir les mod�le de rendu � utiliser
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

    // am�lioration de la perspective
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // D�finir le mod�le d'illumination et autres param�tres
    glLightModelfv(GL_LIGHT_MODEL_COLOR_CONTROL, &CCst::gl_separate_specular_color);
    glLightModelf( GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0 );
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 1 );       // two-side mode in openGL
    glEnable(GL_VERTEX_PROGRAM_TWO_SIDE);              // two-side mode in GLSL

    // les noms de fichier de la texture de la carte.
    std::vector<char*> texturesCarte;
    texturesCarte.push_back("textures/cartePolyRecto.bmp");
    texturesCarte.push_back("textures/cartePolyVerso.bmp");

    // charger la texture de perlin
    chargerBruitPerlin();

    // lier les attributs des nuanceurs
    glBindAttribLocation(progNuanceurCartePoly.getProg(), CCst::indexTangente, "Tangent");

    // construire le mod�le de la carte d'identit�
    cartePoly = new CCarte(&texturesCarte, CCst::largeurModele, CCst::hauteurModele, CCst::nbColonnes, CCst::nbRangees);

    // construire le skybox avec les textures
    skybox = new CSkybox("textures/uffizi_cross_LDR.bmp", CCst::grandeurSkybox); 

    // construire la surface bsplinaire
    gazon = new CGazon("textures/gazon.bmp", 1.0, 1.0);
}



void dessinerScene()
{
    //////////////////////////////////////
    // DESSINER LE SKYBOX
    
    // TODO:
    // Nuanceurs multiples ! r�aliser correctement l'affichage
    // ...

    glPushMatrix(); {

        // TODO: 
        // Corriger l'affichage du skybox afin 
        // d'obtenir une perspective correcte peu importe la position de la cam�ra
        // ...

        // afficher le graphisme (liste d'affichage)
        skybox->afficher();
    }glPopMatrix();
    ///////////////////////////////////////


    ///////////////////////////////////////
    // DESSINER LA SURFACE NURBS

    // TODO:
    // Nuanceurs multiples ! r�aliser correctement l'affichage
    // ...

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
        // appliquer les mat�riaux front and back du mod�le
        nurbs_mat_ambiant_model.appliquer();

        // positionner le maillage b-splinaire au bon endroit
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

        gazon->afficher();
    }glPopMatrix();
    ///////////////////////////////////////


    ///////////////////////////////////////
    // DESSINER LA CARTE D'IDENTIT�.
    progNuanceurCartePoly.activer();

    // ajouts d'autres uniforms
    if (lumieres[LumPonctuelle]->estAllumee()) {
        progNuanceurCartePoly.uniform1("pointLightOn", 1);
    } else {
        progNuanceurCartePoly.uniform1("pointLightOn", 0);
    }

    if (lumieres[LumDirectionnelle]->estAllumee()) {
        progNuanceurCartePoly.uniform1("dirLightOn", 1);
    } else {
        progNuanceurCartePoly.uniform1("dirLightOn", 0);
    }

    if (lumieres[LumSpot]->estAllumee()) {
        progNuanceurCartePoly.uniform1("spotLightOn", 1);
    } else {
        progNuanceurCartePoly.uniform1("spotLightOn", 0);
    }

    // inscrire le temps pr�sent pour l'animation
    progNuanceurCartePoly.uniform1("time", ((float)glutGet(GLUT_ELAPSED_TIME))/500.0f);

    // ajuster le fog de la carte dynamiquement selon la distance � l'observateur
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
        // appliquer les mat�riaux du mod�le
        front_mat_ambiant_model.appliquer();
        back_mat_ambiant_model.appliquer();

        // faire tourner la carte, si d�sir�
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

        cartePoly->afficher();
    }glPopMatrix();
    //////////////////////////////////////


    ///////////////////////////////////////////////////
    // Dessiner les axes pour se retrouver, si d�sir�
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
    ///////////////////////////////////////////////////

    // flush les derniers vertex du pipeline graphique
    glFlush();
}




// fonction d'affichage
void affichage(void)
{
    // effacement de l'ecran et du tampon de profondeur
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // charger matrice de mod�lisation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // mettre � jour la cam�ra openGL
    rafraichirCamera();

    // mettre � jour l'�clairage
    rafraichirEclairage();

    // dessiner la sc�ne
    dessinerScene(); 

    // charger la matrice de projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // mettre � jour la projection
    rafraichirProjection();

    // utiliser le "double-buffering"
    glutSwapBuffers();
}





///////////////////////////////////////////////////////////////////////////////
///  global public  attente \n
///
///  fonction de rappel appelee lorsque l'application ne fait rien.
///  Elle est utilis�e pour l'animation.
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2007-12-14
///
///////////////////////////////////////////////////////////////////////////////
void attente(void)
{
    // une variable pour memoriser le temps a attendre
    static int nWaitUntil = glutGet(GLUT_ELAPSED_TIME);

    // on recupere le temps pr�sent
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
///  @param [in]       touche unsigned char  le caract�re ASCII de la touche enfonc�e
///  @param [in]       x int    la position en x de la souris au moment du rappel
///  @param [in]       y int    la position en y de la souris au moment du rappel
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
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

          // permutation de la lumi�re "spot"
      case 's' :
          {
              if (lumieres[LumSpot]->estAllumee())
                  lumieres[LumSpot]->eteindre();
              else
                  lumieres[LumSpot]->allumer();
              glutPostRedisplay();
              break;
          }

          // permutation de la lumi�re directionnelle
      case 'd' :
          {
              if (lumieres[LumDirectionnelle]->estAllumee())
                  lumieres[LumDirectionnelle]->eteindre();
              else
                  lumieres[LumDirectionnelle]->allumer();
              glutPostRedisplay();
              break;
          }

          // permutation de la lumi�re ponctuelle orient�e
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

          // rotation automatique du mod�le
      case 'r' :
          {
              if (CVar::rotAutoOn)
                  CVar::rotAutoOn = false;
              else
                  CVar::rotAutoOn = true;
              glutPostRedisplay();
              break;
          }

          // animation du mod�le
      case ' ' :
          {
              if (CVar::animModeleOn)
                  CVar::animModeleOn = false;
              else
                  CVar::animModeleOn = true;
              glutPostRedisplay();
              break;
          }

          // faire reculer la cam�ra (zoom-out)
      case '-' :
          {
              CVar::rho += 2.0f;
              glutPostRedisplay();
              break;
          }

          // faire avancer la cam�ra (zoom-in)
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
              if (CVar::minFilter >= 5)
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
              if (CVar::magFilter >= 1)
                  CVar::magFilter = 0;
              else
                  CVar::magFilter++;
              glutPostRedisplay();
              break;
          }

          // permuter le bump mapping de Perlin
      case 'x' :
      case 'X' :
          {
              if (CVar::perlinOn)
                  CVar::perlinOn = false;
              else
                  CVar::perlinOn = true;
              glutPostRedisplay();
              break;
          }
    }
}




///////////////////////////////////////////////////////////////////////////////
///  global public  clavierSpecial \n
///
///  fonction de rappel pour la gestion du clavier sp�cial
///
///  @param [in]       touche int    le caract�re �tendu de la touche enfonc�e
///  @param [in]       x int    la position en x de la souris au moment du rappel
///  @param [in]       y int    la position en y de la souris au moment du rappel
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
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
///  fonction de rappel pour le redimensionnement de la fen�tre graphique
///
///  @param [in]       w GLsizei    nouvelle largeur "w" en pixels
///  @param [in]       h GLsizei    nouvelle hauteur "h" en pixels
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2007-12-14
///
///////////////////////////////////////////////////////////////////////////////
void redimensionnement(GLsizei w, GLsizei h)
{
    CVar::currentW = w;
    CVar::currentH = h;
    glViewport(0, 0, w, h);
    affichage();
}




///////////////////////////////////////////////////////////////////////////////
///  global public  sourisClickee \n
///
///  fonction de rappel pour la gestion d'un click de la souris
///
///  @param [in]       button int   l'identificateur du bouton de souris qui fut click�
///  @param [in]       state int    l'�tat du bouton (press� ou rel�ch�)
///  @param [in]       x int        la position en x de la souris au moment du click
///  @param [in]       y int        la position en y de la souris au moment du click
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
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
///  @author Fr�d�ric Plourde 
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
////////////  FONCTIONS POUR LA CAM�RA ///////////////////
//////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///  global public  rafraichirCamera \n
///
///  Fonction de gestion de la position de la cam�ra en coordonn�es sph�riques.
///  Elle s'occuper de trouver les coordonn�es x et y de la cam�ra � partir 
///  des theta et phi courants, puis fixe dans openGL la position de la cam�ra
///  � l'aide de gluLookAt().
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
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
///////////  FONCTIONS POUR L'�CLAIRAGE //////////////////
//////////////////////////////////////////////////////////
void rafraichirEclairage()
{
    // d�placer la lumi�re "lampe de poche" � l'endroit du glulookat
    lumieres[LumSpot]->modifierPos(CVar::x, CVar::y, CVar::z);

    // d�placer la direction de la lampe de poche vers le centre (0,0,0)
    lumieres[LumSpot]->modifierSpotDir(-CVar::x, -CVar::y, -CVar::z);

    // Mise � jour des diff�rentes lumi�res de la sc�ne
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
    // on compiler ici les programmes de nuanceurs qui furent pr�d�finis
    progNuanceurCartePoly.compilerEtLier();
    progNuanceurCartePoly.activer();
    
    // TODO:
    // On doit compiler les autres nuanceurs
    // ...

    // on enregistre les diff�rents uniforms que requerront les nuanceurs 
    progNuanceurCartePoly.enregistrerUniformInteger("frontColorMap", CCst::texUnit_0);
    progNuanceurCartePoly.enregistrerUniformInteger("backColorMap",  CCst::texUnit_1);
    progNuanceurCartePoly.enregistrerUniformInteger("normalMap",     CCst::texUnit_2);
    progNuanceurCartePoly.enregistrerUniformFloat("dispAmp1",        CCst::dispAmplitude1);
    progNuanceurCartePoly.enregistrerUniformFloat("dispAmp2",        CCst::dispAmplitude2);
    progNuanceurCartePoly.enregistrerUniformFloat("dispFreq1",       CCst::dispFrequency1);
    progNuanceurCartePoly.enregistrerUniformFloat("dispFreq2",       CCst::dispFrequency2);
    progNuanceurCartePoly.enregistrerUniformInteger("animModele",    CVar::animModeleOn);
    progNuanceurCartePoly.enregistrerUniformInteger("fogOn",         CVar::fogOn);
    progNuanceurCartePoly.enregistrerUniformInteger("perlinOn",      CVar::perlinOn);

    // TODO:
    // Le nuanceur du skybox utilise une variable "colorMap" qu'il faut enregistrer.
    // ..

    // TODO:
    // Le nuanceur du gazon utilise une variable "colorMap" qu'il faut enregistrer.
    // ..
}


void chargerBruitPerlin()
{
    // construire la texture
    // calculer la texture de bruit de Perlin
    float* texNoise2D = make2DNoiseArray(CCst::noiseWidth,CCst::noiseHeight, 32, 1);

    // calculer la "normal map"
    vect3D* texNormal2D = compute2DNormalTexture(texNoise2D, CCst::noiseWidth, CCst::noiseHeight);

    // cr�er la texture
    glGenTextures(1, &CVar::perlinTex); 
    glBindTexture(GL_TEXTURE_2D, CVar::perlinTex);

    // aiguiller openGL vers la zone m�moire contenant le graphisme de la texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, CCst::noiseWidth, CCst::noiseHeight, 0, GL_RGB, GL_FLOAT, texNormal2D);

    // lib�rer l'espace m�moire maintenant que la texture est copi�e dans la m�moire vid�o
    if( texNoise2D ) {
        free( texNoise2D );
    }

    if( texNormal2D ) {
        free( texNormal2D );
    }                       
}