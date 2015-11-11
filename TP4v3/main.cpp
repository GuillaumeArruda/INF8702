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
#include <GL/glut.h>
#include "textfile.h"
#include "Skybox.h"
#include "Gazon.h"
#include "Modele3DOBJ.h"
#include "SurfaceBSplinaire.h"
#include "Lumiere.h"
#include "Cst.h"
#include "Var.h"
#include "NuanceurProg.h"
#include "Materiau.h"
#include "Texture2D.h"
#include "FBO.h"
#include "TextureCubemap.h"


///////////////////////////////////////////////
// �num�rations locales                      //
///////////////////////////////////////////////

/// �num�ration des lumi�res de la sc�nes (� utiliser comme INDEX de lumieres[])
enum ENUM_LUM {LumPonctuelle, LumDirectionnelle, LumSpot};



///////////////////////////////////////////////
// LES OBJETS                                //
///////////////////////////////////////////////

// la liste des lumieres openGL de la sc�ne (maximum de 8)
std::vector<CLumiere*> lumieres(CCst::nbLumieresOpenGL);

// les diff�rents mat�riaux utilis�s dans le programme
CMateriau mat_ambiant_model      (0.0, 0.0, 0.0,  0.8, 0.8, 0.8,  0.7, 0.7, 0.7,  1.0, 1.0, 1.0,  0.0, 0.0, 0.0,  1.0,  10.0,  GL_FRONT_AND_BACK);
CMateriau nurbs_mat_ambiant_model(0.0, 0.0, 0.0,  1.0, 1.0, 1.0,  1.0, 1.0, 1.0,  1.0, 1.0, 1.0,  0.0, 0.0, 0.0,  1.0,  0.0,    GL_FRONT_AND_BACK);

// les programmes de nuanceurs
CNuanceurProg progNuanceurOpenGL;
CNuanceurProg progNuanceurSkybox("Nuanceurs/skyBoxSommets.glsl", "Nuanceurs/skyBoxFragments.glsl", false); 
CNuanceurProg progNuanceurGazon("Nuanceurs/gazonSommets.glsl", "Nuanceurs/gazonFragments.glsl", false); 
CNuanceurProg progNuanceurVenus("Nuanceurs/VenusSommets.glsl", "Nuanceurs/VenusFragments.glsl", false); 
CNuanceurProg progNuanceurTest("Nuanceurs/testSommets.glsl", "Nuanceurs/testFragments.glsl", false); 


// Les objets 3D graphiques (� instancier plus tard parce que les textures demandent le contexte graphique)
CSkybox *skybox;
CGazon  *gazon;
CModele3DOBJ *modele3D;
CFBO *shadowMaps[3];

// quelques matrices globales (pour le shadowmapping)
GLfloat modelviewMatrix[16];
GLfloat projectionMatrix[3][16];  // une matrice pour chacune des lumi�res
GLfloat scaleAndBiasMatrix[16] = {0.5f, 0.0f, 0.0f, 0.0f,  0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 0.0f, 0.5f, 0.0f,  0.5f, 0.5f, 0.5f, 1.0f};

///////////////////////////////////////////////
// PROTOTYPES DES FONCTIONS DU MAIN          //
///////////////////////////////////////////////
void initialisation (void);
void dessinerScene(bool dessinerObjets = true);
void affichage(void);
void attente(void);
void clavier(unsigned char touche, int x, int y);
void clavierSpecial(int touche, int x, int y);
void mouvementSouris(int x, int y);
void sourisClickee(int button, int state, int x, int y);
void redimensionnement(GLsizei w, GLsizei h);
void rafraichirCamera();
void construireCartesOmbrage();
void construireMatricesProjectivesEclairage();
void compilerListeAffmur(GLuint &listeID, float largeur, float hauteur, int nbRangees, int nbColonnes, bool normaleVersExt);
void dessinerGrilleDeQuads(float largeur, float hauteur, int nbRangees, int nbColonnes, bool normaleVersExt);
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
    glutInitWindowSize(CVar::currentW, CVar::currentH);
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
    if (glewIsSupported("GL_VERSION_2_0"))
	printf("Pret pour OpenGL 2.0\n");
    else {
	printf("OpenGL 2.0 n'est pas supporte! \n");
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
    delete skybox;
    delete gazon;
    if (shadowMaps) {
	for (int i=0;i<3;i++) {
	    delete shadowMaps[i];
	}    
    }

    // le programme n'arrivera jamais jusqu'ici
    return EXIT_SUCCESS;
}



// initialisation d'openGL
void initialisation (void) {

    ////////////////////////////////////////////////////
    // CONSTRUCTION DES LUMI�RES OPENGL
    ////////////////////////////////////////////////////

    // LUMI�RE PONCTUELLE ORKENT�E (enum : LumPonctuelle)
    lumieres[LumPonctuelle]     = new CLumiere(GL_LIGHT0, 0.0,0.0,0.0, 1.0,0.6,1.0, 0.8,0.4,0.8, 1.0,1.0,1.0, 20.0,0.0,50.0,1.0, true);

    // LUMI�RE DIRECTIONNELLE (enum : LumDirectionnelle)
    lumieres[LumDirectionnelle] = new CLumiere(GL_LIGHT1, 0.0,0.0,0.0, 0.86,0.69,0.04, 0.81,0.83,0.0, 1.0,1.0,1.0, -50.0,0.0,50.0,0.0, true );

    // LUMI�RE SPOT (enum : LumSpot)
    lumieres[LumSpot]           = new CLumiere(GL_LIGHT2, 0.0,0.0,0.0, 1.0,1.0,1.0, 1.0,1.0,1.0, 1.0,1.0,1.0, 10.0,30.0,90.0,1.0, true, 0.0, 1.0, 0.0, 100.0, 20.0);
    lumieres[LumSpot]->modifierLinAtt(0.02);
    lumieres[LumSpot]->modifierQuadAtt(0.001);

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

    // construction du mod�le 3D de la v�nus de Milo
    // On met � votre disposition DEUX VERSIONS 3D de la v�nus.
    // Une version "low-poly" de d�verminage qui acc�l�re le chargement ============> "Modeles/venus-low.obj"
    // Une version haute r�solution pour la config RELEASE pour un max de qualit� ==> "Modeles/venus.obj"
    // � vous de modifier le nom de fichier dans l'appel ci-dessous.
    char* modele3DTexture = NULL;  // on ne d�sire pas de texture pour l'instant
    modele3D = new CModele3DOBJ("Modeles/venus.obj", modele3DTexture, 1.0);

    // construire le skybox avec les textures
    skybox = new CSkybox("textures/uffizi_cross_LDR.bmp", CCst::grandeurSkybox); 

    // construire la surface bsplinaire
    gazon = new CGazon("textures/gazon.bmp", 1.0, 1.0);

    // cr�ation des trois FBOs
    for (int i=0;i<3;i++) {
	shadowMaps[i] = new CFBO();
	shadowMaps[i]->Init(CCst::largeurShadowMap, CCst::hauteurShadowMap);
    }
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



#ifdef AIDE_POUR_TP

    // AIDE POUR LE TP
    // Ce bout de code, activ� par le #define syst�me "AIDE_POUR_TP, 
    // d�clar� au tout d�but de Cst.h, permet de visualiser un de vos FBO 
    // en mode RGB. Modifiez ci-bas la variable shadowMapAAfficher = [0, 1 ou 2] afin 
    // d'afficher la shadowMap de la lumi�re "point", "directionnelle"
    // ou "spot" respectivement. Bon d�boguage !
    // L'int�r�t de vous fournir un tel outil de visualisation est de vous
    // permettre ais�ment d'ajuster les param�tres de projection lors de la 
    // construction de vos shadowMaps dans construireCartesOmbrage(), afin 
    // d'obtenir la meilleure r�solution possible (en X,Y ainsi qu'en 
    // niveaux de gris pour le depth component) lorsque vos FBO reviendront
    // en mode DEPTH_COMPONENT par la suite.

    progNuanceurTest.activer(); 
    int shadowMapAAfficher = 0;
    glPushMatrix(); {
	shadowMaps[shadowMapAAfficher]->Bind(GL_TEXTURE0);

	glBegin(GL_QUADS);{
	    glTexCoord2f(0.0,  0.0); glVertex3f(0.0,  0.0,  0.0);
	    glTexCoord2f(1.0,  0.0); glVertex3f(10.0, 0.0,  0.0);
	    glTexCoord2f(1.0,  1.0); glVertex3f(10.0, 10.0, 0.0);
	    glTexCoord2f(0.0,  1.0); glVertex3f(0.0,  10.0, 0.0);
	}glEnd();
    }glPopMatrix();
#endif



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
       // ajuster le fog de la carte dynamiquement selon la distance � l'observateur
   if (CVar::fogOn) {
      glFogi (GL_FOG_MODE,    GL_LINEAR);
      glFogf (GL_FOG_START,   80.0);
      glFogf (GL_FOG_END,     800.0);
      glFogfv(GL_FOG_COLOR,   CCst::fogColor);
      glFogf (GL_FOG_DENSITY, 1.0);
      glEnable(GL_FOG);
   } else {
      glDisable(GL_FOG);
   }
    glPushMatrix(); {
	// appliquer les mat�riaux front and back du mod�le
	nurbs_mat_ambiant_model.appliquer();

	glTranslatef(-150.0, -30.0, 14.2);
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

	// lier les cartes d'ombrages pr�construites
	// aux unit�s de texture GL_TEXTURE1, GL_TEXTURE2 et GL_TEXTURE3
	// � l'int�rieur des nuanceurs du gazon.
	for (int i=0;i<3;i++) {
	    shadowMaps[i]->Bind(CCst::unitesTextures[i+1]);
	}
	// construire les matrices projectives associ�es � chaque shadowMap
	construireMatricesProjectivesEclairage();

	gazon->afficher();
    }glPopMatrix();
    ///////////////////////////////////////



    if (dessinerObjets)
    {
	if (CVar::modeleOn) {
	    ///////////////////////////////////////
	    // DESSINER LE MOD�LE 3D en OBJ de la cam�ra

	    progNuanceurVenus.activer();

	    // ajouts d'autres uniforms
	    if (lumieres[LumPonctuelle]->estAllumee()) {
		progNuanceurVenus.uniform1("pointLightOn", 1);
	    } else {
		progNuanceurVenus.uniform1("pointLightOn", 0);
	    }

	    if (lumieres[LumDirectionnelle]->estAllumee()) {
		progNuanceurVenus.uniform1("dirLightOn", 1);
	    } else {
		progNuanceurVenus.uniform1("dirLightOn", 0);
	    }

	    if (lumieres[LumSpot]->estAllumee()) {
		progNuanceurVenus.uniform1("spotLightOn", 1);
	    } else {
		progNuanceurVenus.uniform1("spotLightOn", 0);
	    }

	    glPushMatrix(); {
		// appliquer les mat�riaux du mod�le
		mat_ambiant_model.appliquer();

		// faire tourner le mod�le, si d�sir�
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

		// afficher le mod�le 3D
		modele3D->afficher();

	    }glPopMatrix();
	    //////////////////////////////////////
	}



	///////////////////////////////////////////////////
	// Dessiner les axes pour se retrouver, si d�sir�
	if (CVar::axesOn) {
	    progNuanceurOpenGL.activer(); 

	    glPushMatrix(); {
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE3);
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
    
    // � chaque image, le point de vue ou
    // l'orientation du mod�le 3D ont peut-�tre chang�.
    // Alors construire les cartes d'ombrage... si shadowMap est � on.
    if (CVar::shadowMapOn) {   
	construireCartesOmbrage();
    }

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

	  // permuter le shadow mapping
      case 'o' :
      case 'O' :
         {
            if (CVar::shadowMapOn)
               CVar::shadowMapOn = false;
            else {
               CVar::shadowMapOn = true;
            }
            glutPostRedisplay();
            break;
         }

	 // permuter l'affichage du mod�le 3D (pour mieux voir l'ombre derri�re)
      case 'z' :
      case 'Z' :
         {
            if (CVar::modeleOn)
               CVar::modeleOn = false;
            else {
               CVar::modeleOn = true;
            }
            glutPostRedisplay();
            break;
         }

	// permuter la rotation des lumi�res openGL (pour les ombres)
      case 'k' :
      case 'K' :
         {
            if (CVar::rotLumOn)
               CVar::rotLumOn = false;
            else {
               CVar::rotLumOn = true;
            }
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
//////////  FONCTIONS POUR LE SHADOWMAPPING //////////////
//////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///  global public  construireCartesOmbrage \n
///
///  fonction de construction des cartes d'ombrage
///
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-10-29
///
///////////////////////////////////////////////////////////////////////////////
void construireCartesOmbrage()
{
    // TODO:
    // compl�ter la fonction "construireCartesOmbrage"

    GLfloat pos[4];

    // Construire les trois cartes d'ombrage
    for (unsigned int i = 0; i < lumieres.size(); i++)
    {
        // obtenir la position de la lumi�re en cours dans "pos"
        progNuanceurVenus.activer();
        lumieres[i]->obtenirPos(pos);

        // commencer la d�finition du FBO correspondant
        shadowMaps[i]->CommencerCapture();

        // D�finir le bon viewport de la shadow map
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, shadowMaps[i]->GetWidth(), shadowMaps[i]->GetHeight());

        // Ajustement de l'environnement correct pour lumi�re en cours
        // � ajuster correctement : GL_MODELVIEW et GL_PROJECTION
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0, (GLfloat)CVar::currentW / (GLfloat)CVar::currentH, 0.1, 2000.0);

        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(pos[0], pos[1], pos[2], 0, 0, 0, 0, 1, 0);

        // Ajout d'un polygon offset pour obtenir un meilleur r�sultat (faccultatif)
        // ...


        // ne pas oublier d'effacer les bits de couleur et de profondeur
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glPushMatrix();

	    // afficher le mod�le 3D en GL_MODELVIEW
        glRotated(180.0, 1.0, 0.0, 0.0);
        glRotated(CVar::angleRotX, 1.0, 0.0, 0.0);
        glRotated(CVar::angleRotY, 0.0, 1.0, 0.0);
        glRotated(CVar::angleRotZ, 0.0, 0.0, 1.0);

        modele3D->afficher();
        glPopMatrix();

	    // remettre les matrices GL_MODELVIEW ET GL_PROJECTION
	    // qui �taient l� avant...


	    // terminer la d�finition du FBO correspondant
        shadowMaps[i]->TerminerCapture();
        
	    // "Popper" le viewport
        glPopAttrib();
    }
}


///////////////////////////////////////////////////////////////////////////////
///  global public  construireMatricesProjectivesEclairage \n
///
///  fonction de construction des matrices projectives d'�clairage
///
///  @return Aucune
///
///  @author Fr�d�ric Plourde 
///  @date   2008-10-29
///
///////////////////////////////////////////////////////////////////////////////
void construireMatricesProjectivesEclairage()
{
    // TODO:
    // compl�ter la fonction "construireCartesOmbrage"

    // matrices ci-bas seront r�utilis�es par chaque carte
    // pour le calcul de la matrice de transfo finale dans GL_TEXTURE

    // matrice MODEL de la patch de gazon
    GLfloat gazonModelMatrix[16];
    // matrice VIEW de la lumi�re
    GLfloat lightViewMatrix[16];
    // matrice PROJECTION de la lumi�re
    GLfloat lightProjectionMatrix[16];

    // position de la lumi�re (mise � jour par chaque lumi�re)
    GLfloat pos[4];

    for (unsigned int i = 0; i < lumieres.size(); i++)
    {
	// on obtient la position de la lumi�re consid�r�e
	lumieres[i]->obtenirPos(pos);

	// TODO: 
	// � vous de compl�ter la construction des matrices
	// projective d'�clairage (une pour chaque lumi�re)
	// pour vous aider.. on laisse ici quelques �l�ments
	// de pseudo-algo.

	// r�construire/r�cup�rer la matrice MODEL *du gazon*!
	// ...
	// ...
	// ... la sauvegarder dans "gazonModelMatrix"
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glTranslatef(-150.0, -30.0, 14.2);
    glScalef(CCst::largeurGazon, CCst::longueurGazon, CCst::hauteurGazon);
    glTranslatef(-3.5f, -1.5f, 0.0);
    glGetFloatv(GL_MODELVIEW, gazonModelMatrix);

	// r�construire/r�cup�rer la matrice VIEW de la lumi�re
	// ...
	// ...
	// ... la sauvegarder dans "lightViewMatrix"
    glLoadIdentity();
    gluLookAt(pos[0], pos[1], pos[2], 0, 0, 0, 0, 1, 0);

    glGetFloatv(GL_MODELVIEW, lightViewMatrix);



	// r�construire/r�cup�rer la matrice PROJECTION de la lumi�re
	// ...
	// ...
	// ... la sauvegarder dans "lightProjectionMatrix"
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)CVar::currentW / (GLfloat)CVar::currentH, 0.1, 2000.0);
    glGetFloatv(GL_PROJECTION, lightProjectionMatrix);
	
    // Construire la matrice de TEXTURE � passer au nuanceur
	// et Changer la texture active en fonction de la lumi�re consid�r�e
	glMatrixMode( GL_TEXTURE );
	glActiveTexture(CCst::unitesTextures[i+1]);
    glLoadIdentity();
	// construire/calculer la matrice projective d'�clairage
	// ...
	// ...
	// ...
	// ...
    glMultMatrixf(gazonModelMatrix);
    glMultMatrixf(lightViewMatrix);
    glMultMatrixf(lightProjectionMatrix);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
	// Remettre en MODELVIEW avant de quitter la fonction
	glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    }
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
    float deltaLum0, deltaLum1;
    // si d�sir�, faire tourner les lumi�res 0 et 1
    // pour un effet maximal des ombrages.
    if (CVar::rotLumOn) {
	CVar::angleRotLum[0] += 2.0 * PI * (CCst::freqRotLum0 / CCst::frameRate);
	CVar::angleRotLum[1] += 2.0 * PI * (CCst::freqRotLum1 / CCst::frameRate);
	// nous ne toucherons jamais � angleRotLum[2].. la lumi�re spot !
	// cette valeur fut initialis�e � 0.0 dans CVar::
	deltaLum0 = 40.0 * sin(CVar::angleRotLum[0]);
	deltaLum1 = 30.0 * sin(CVar::angleRotLum[1]);
    }
    else {
	deltaLum0 = 0.0f;
	deltaLum1 = 0.0f;
    }

    lumieres[LumPonctuelle]->modifierPos(20.0 + deltaLum0, 0 + deltaLum0/4.0, 50.0);
    lumieres[LumDirectionnelle]->modifierPos(-50.0 + deltaLum1/2.0, deltaLum1 , 50.0);

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
    progNuanceurVenus.compilerEtLier();
    progNuanceurSkybox.compilerEtLier();
    progNuanceurGazon.compilerEtLier();
    progNuanceurTest.compilerEtLier();

    // on enregistre les diff�rents uniforms que requerront les nuanceurs 
    progNuanceurSkybox.enregistrerUniformInteger("colorMap",  CCst::texUnit_0);

    progNuanceurGazon.enregistrerUniformInteger("colorMap",   CCst::texUnit_0);
    progNuanceurGazon.enregistrerUniformInteger("shadowMap1", CCst::texUnit_1);
    progNuanceurGazon.enregistrerUniformInteger("shadowMap2", CCst::texUnit_2);
    progNuanceurGazon.enregistrerUniformInteger("shadowMap3", CCst::texUnit_3);
    progNuanceurGazon.enregistrerUniformInteger("shadowMapOn",CVar::shadowMapOn);
    progNuanceurGazon.enregistrerUniformInteger("fogOn",      CVar::fogOn);

    progNuanceurTest.enregistrerUniformInteger("colorMap",  CCst::texUnit_0);
}