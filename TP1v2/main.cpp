#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "textfile.h"


///////////////////////////////////////////////
// déclaration des macros et defines globaux //
///////////////////////////////////////////////

#ifndef M_PI
#define M_PI 3.1415926535897932385
#endif
#define DEG2RAD(a) ( a / 180.0f ) * ( M_PI );


/////////////////////////////////////////
// déclaration des constantes globales //
/////////////////////////////////////////

const GLfloat gl_separate_specular_color = GL_SEPARATE_SPECULAR_COLOR;  // constante de paramètre

const float freqRotX     = 0.15;             // la fréquence de rotation en X du modèle
const float freqRotY     = 0.1;              // la fréquence de rotation en Y du modèle
const float freqRotZ     = 0.2;              // la fréquence de rotation en Z du modèle

const float frameRate    = 50.0;             // le framerate cible de l'application graphique.                   

const float modelSize    = 15.0;             // la grandeur du modèle
const float axeScaleFact = 15.0;             // le facteur multiplicatif des axes unitaires dessinés dans la scène
const GLfloat fogColor[] = {0.0, 0.0, 0.0};  // la couleur du FOG dans l'application

bool rotAutoModele       = false;            // la rotation automatique du modèle ou non



///////////////////////////////////////
// déclaration des variables globales //
////////////////////////////////////////
int g_nWindowID;                 // le ID de la fenêtre graphique GLUT
const int g_nWidth = 640;        // la largeur initiale de la fenêtre GLUT
const int g_nHeight = 480;       // la hauteur initiale de la fenêtre GLUT
int currentW, currentH;          // Largeur et hauteur courantes de la fenêtre graphique (viewport)
bool isPerspective = true;       // indique si on est en mode Perspective (true) ou glOrtho (false) 

double theta = DEG2RAD(270.0f);  // angle de rotation de la caméra en theta (coord. sphériques)
double phi = DEG2RAD(90.0f);     // angle de rotation de la caméra en phi (coord. sphériques)
double rho = 30.0;               // angle de rotation de la caméra en rho (coord. sphériques)
float  x, y, z;					   // la position de la camera

float angleRotX = 0.0f;          // angle courant de rotation en X du modèle
float angleRotY = 0.0f;          // angle courant de rotation en Y du modèle
float angleRotZ = 0.0f;          // angle courant de rotation en Z du modèle

bool premierMouvement = true;    // indique si la souris fut bougée déjà
int dernierX = 0;                // la dernière valeur en X de position lue de la souris
int dernierY = 0;                // la derniere valeur en Y de position lus de la souris

bool spotLightAllumee  = true;   // indique si on désire activer la lumière Spot
bool dirLightAllumee   = true;   // indique si on désire activer la lumière Directionnelle
bool pointLightAllumee = true;   // indique si on désire activer la lumière Ponctuelle
bool SHADER_ON         = false;  // indique si l'on veut utiliser le nuanceur

#define QUAD_LIST 1              // déclaration de la liste d'affichage du modèle  
#define CUBE_LIST 2              // déclaration de la liste d'affichage d'un cube 

GLuint texture0;	               // les noms de textures
GLuint texture1;	               // les noms de textures
GLuint texture2;	               // les noms de textures

GLuint noShaderProg = 0 ;        // Un "nuanceur" NUL ==> représentant la fonctionalité fixe d'OPENGL (=0)
GLuint fixedPipelineShaderProg;	// Votre programme de nuanceur émulant la fonctionnalité fixe.


// Définition des lumières openGL
// Les lumières sont et restent définies dans la PARTIE OPENGL, même lorsque 
// l'on utilise les nuanceurs. Alors ici, pour obtenir l'effet désiré dans ShaderGen,
// on doit créer les mêmes trois (3) lumières openGL que dans ShaderGen et les 
// positionner aux mêmes endroits.

// POINT LIGHT
GLfloat light_position0[] = { 20.0, 10.0, 3.0, 1.0 };
GLfloat light_ambient0[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_diffuse0[] = { 1.0, 0.5, 1.0, 1.0 };
GLfloat light_specular0[] = { 1.0, 0.5, 1.0, 1.0 };
GLfloat light_emission0[] = { 1.0, 1.0, 1.0, 1.0 };

// SPOT LIGHT
GLfloat light_position1[] = { 0.0,  0.0,  0.0,  1.0 };
GLfloat light_ambient1[] =  { 0.0,  0.0,  0.0,  1.0 };
GLfloat light_diffuse1[] =  { 1.0,  1.0,  1.0,  1.0 };
GLfloat light_specular1[] = { 1.0,  1.0,  1.0,  1.0 };
GLfloat light_emission1[] = { 1.0, 1.0, 1.0, 1.0 };

// DIRECTIONNAL LIGHT
GLfloat light_position2[] = {  -60.0, 0.0, 3.0, 0.0 };
GLfloat light_ambient2[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_diffuse2[] = { 0.86, 0.69, 0.04, 1.0 };
GLfloat light_specular2[] = { 0.91, 0.93, 0.0, 1.0 };
GLfloat light_emission2[] = { 1.0, 1.0, 1.0, 1.0 };

// définition du matériau utilisé.
GLfloat mat_ambiant_model[]  = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_diffuse_model[]  = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_specular_model[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_emission_model[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_shininess_model[] = { 100.0 };

// Structure qui représente une image RGB
struct SImage
{
    GLuint    width;
    GLuint    height;
    GLubyte*  data;

    SImage( int Width, int Height )
        : width ( Width  )
        , height( Height )
        , data  ( NULL   )
    {
        data = new GLubyte[ width * height * 3 ];
    }

    ~SImage( void )
    {
        if( data )
            delete [] data;
    }
};

// PROTOTYPES DES FONCTIONS
void initialisation (void);
void dessinerScene();
void affichage(void);
void attente(void);
void clavier(unsigned char touche, int x, int y);
void clavierSpecial(int touche, int x, int y);
void redimensionnement(GLsizei w, GLsizei h);

void rafraichirCamera();
void compileGrilleQuads(int listID, float size, int nbRows, int nbColums, bool outsideNormal);
void compileCube(int listID, float size, int nbRows, int nbColums, bool outsideNormal);
void dessinerGrilleQuads(float size, int nbRows, int nbColums, bool outsideNormal);
void rafraichirEclairage();
void rafraichirProjection();
void initialiserNuanceurs() ;
void afficherShaderInfoLog(GLuint obj);
void afficherProgramInfoLog(GLuint obj);
void appliquerTextures();
SImage* LoadBMP(char* Filename);
void init2DTexture(GLint texName, GLint texWidth, GLint texHeight, GLubyte *texPtr);
void chargerTextures();
void desactiverTextures();



// initialisation d'openGL
void initialisation (void) {

   // modèle de rendu
   glShadeModel(GL_SMOOTH);	

   /*  couleur de l'arrière-plan   */
   glClearColor (0.2, 0.2, 0.2, 1.0);

   // activer les etats openGL
   glEnable(GL_NORMALIZE);
   glEnable(GL_LIGHTING);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_FOG);

   // fonction de profondeur
   glDepthFunc(GL_LEQUAL);

   // amélioration de la perspective
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  
   glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE );
   glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );     // two-side mode in openGL
   glEnable( GL_VERTEX_PROGRAM_TWO_SIDE );                // two-side mode in GLSL
   // ajouter ici le mode de calcul séparé de la couleur spéculaire
   glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

   // charger les textures
   chargerTextures();

   // compiler les listes d'affichage
   compileGrilleQuads(QUAD_LIST, modelSize, 30, 30, true);
   compileCube(CUBE_LIST, modelSize, 30, 30, true);
}



void dessinerScene()
{

   // ajuster le fog dynamiquement selon la distance de l'observateur
   // vous devez ici l'améliorer... pour que les paramètres
   // GL_FOG_START et GL_FOG_END soient ajustés automatiquement
   // en fonction du zoom.

	glFogi (GL_FOG_MODE,    GL_LINEAR);
	glFogf (GL_FOG_START,   rho - 6);
	glFogf (GL_FOG_END,     rho + 4);
	glFogfv(GL_FOG_COLOR,   fogColor);
	glFogf (GL_FOG_DENSITY, 0.0);

   if (SHADER_ON)
      glUseProgram(fixedPipelineShaderProg);
   else
      glUseProgram(noShaderProg);

   // Dessiner le modèle 
	glPushMatrix(); {
		// appliquer le matériau du cube
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambiant_model);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_model);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular_model);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission_model);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess_model);

		// dessiner le cube tournant, si désiré
      if (rotAutoModele) {
		   angleRotX += 360.0 * (freqRotX / frameRate);
		   angleRotY += 360.0 * (freqRotY / frameRate);
		   angleRotZ += 360.0 * (freqRotZ / frameRate);

         if (angleRotX >= 360.0)
			   angleRotX -= 360.0;
		   if (angleRotY >= 360.0)
			   angleRotY -= 360.0;
		   if (angleRotZ >= 360.0)
			   angleRotZ -= 360.0;
      }	

		glRotated(180.0, 1.0, 0.0, 0.0);
		glRotated(angleRotX, 1.0, 0.0, 0.0);
      glRotated(angleRotY, 0.0, 1.0, 0.0);
      glRotated(angleRotZ, 0.0, 0.0, 1.0);

      glCallList(QUAD_LIST);    
      desactiverTextures();
   }
   glPopMatrix();

   glUseProgram(noShaderProg); 

	// Dessiner les axes pour se retrouver
	glPushMatrix(); {
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_LIGHTING);
      glDisable(GL_FOG); 
		
		glLineWidth(3.0);

		glBegin(GL_LINES);{
			// l'axe des x en Rouge
			glColor4f(1.0, 0.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(1.0*axeScaleFact, 0.0, 0.0);

			// l'axe des y en Vert
			glColor4f(0.0, 1.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 1.0*axeScaleFact, 0.0);

			// l'axe des z en Bleu
			glColor4f(0.0, 0.0, 1.0, 1.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, 1.0*axeScaleFact);
		}
		glEnd();

		glEnable(GL_FOG);
	   glEnable(GL_LIGHTING);
      glEnable(GL_TEXTURE_2D);
	}
	glPopMatrix();

   if (SHADER_ON)
      glUseProgram(fixedPipelineShaderProg);
   else
      glUseProgram(noShaderProg);

    // flush les derniers vertex du pipeline graphique
	glFlush();
}



// fonction d'affichage
void affichage(void)
{
    // effacement de l'ecran et du tampon de profondeur
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // charger matrice de modélisation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // mettre à jour la caméra openGL
    rafraichirCamera();  // ne pas permettre pour l'instant de déplacer la caméra...

    // mettre à jour l'éclairage
    rafraichirEclairage();

    // dessiner la scène
    dessinerScene(); 

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // mettre à jour la projection
    rafraichirProjection();

    // utiliser le "double-buffering"
    glutSwapBuffers();
}



// fonction appelee lorsque l'application ne fait rien
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
		// 5 fois pa5 seconde
		nWaitUntil = nTimer + (1000 / frameRate);
	}
}



// fonction de gestion du clavier
void clavier(unsigned char touche, int x, int y)
{
	// traitement des touches q et echap
   switch(touche) {
      case 'q' :
		// destruction de la fenetre GLUT
		glutDestroyWindow(g_nWindowID);
		// on quitte notre programme
		exit(EXIT_SUCCESS);
      break;
      
      case 'p' :
         if (isPerspective)
            isPerspective = false;
         else
            isPerspective = true;
         affichage();
      break;

      // permutation du "spot light"
      case 's' :
         if (spotLightAllumee)
            spotLightAllumee = false;
         else
            spotLightAllumee = true;
         glutPostRedisplay();
      break;

      // permutation du "dirLight"
      case 'd' :
         if (dirLightAllumee)
            dirLightAllumee = false;
         else
            dirLightAllumee = true;
         glutPostRedisplay();
      break;

      // permutation du "point light"
      case 'a' :
         if (pointLightAllumee)
            pointLightAllumee = false;
         else
            pointLightAllumee = true;
         glutPostRedisplay();
      break;

      // permutation de l'activation des shaders
      case 'x' :
         if (SHADER_ON)
            SHADER_ON = false;
         else
            SHADER_ON = true;
         glutPostRedisplay();
      break;

      case '-' :
         // la caméra recule
         rho += 2.0f;
         affichage();
      break;

      case '=' :
         // la caméra avance
         rho -= 2.0f;
         if (rho <= 1.0)
            rho = 1.0f;
         affichage();
      break;

      case ' ' :
         // permuter la rotation automatique du modèle
         rotAutoModele ? rotAutoModele = false : rotAutoModele = true;
         affichage();
      break;
      
	}
}



// fonction de gestion du clavier spécial
void clavierSpecial(int touche, int x, int y)
{
	// traitement des touches q et echap
   switch(touche) {
      case GLUT_KEY_RIGHT :
		   // effectuer une rotation vers la droite autour du cube
         theta += DEG2RAD(2.0f);
         affichage();
      break;

      case GLUT_KEY_LEFT :
		   // effectuer une rotation vers la droite autour du cube
         theta -= DEG2RAD(2.0f);
         affichage();
      break;

      case GLUT_KEY_UP :
		   // effectuer une rotation vers la droite autour du cube
         phi -= DEG2RAD(2.0f);
         affichage();
      break;

      case GLUT_KEY_DOWN :
		   // effectuer une rotation vers la droite autour du cube
         phi += DEG2RAD(2.0f);
         affichage();
      break;
	}
}



// fonction de redimensionnement de la fenêtre graphique
void redimensionnement(GLsizei w, GLsizei h)
{
   currentW = w;
   currentH = h;
   glViewport(0, 0, w, h);
   affichage();
}


// fonction callback lorsque la souris est cliquee
void sourisClickee(int button, int state, int x, int y)
{
   if (state == GLUT_DOWN) {
      // on vient de presser la souris
      dernierX = x;
      dernierY = y;
   } else {
      // on vient de relacher la souris

   }
}



// fonction de redimensionnement de la fenêtre graphique
void mouvementSouris(int x, int y)
{
   angleRotX += y - dernierY;
   angleRotZ -= x - dernierX;

   dernierX = x;
   dernierY = y;
}



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
	g_nWindowID = glutCreateWindow("Exemple");

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

	// vérification de la présence des extensions
	// ou de la version 2.0 d'openGL
	glewInit();
	if (glewIsSupported("GL_VERSION_2_0"))
		printf("Ready for OpenGL 2.0\n");
	else {
		printf("OpenGL 2.0 not supported\n");
		exit(1);
	}

	// compiler et linker les nuanceurs
	initialiserNuanceurs();

	// initialisation de variables d'état openGL et création des listes
	initialisation();

	// boucle principale de gestion des evenements
	glutMainLoop();

	// le programme n'arrivera jamais jusqu'ici
	return EXIT_SUCCESS;
}



//////////////////////////////////////////////////////////
////////////  FONCTIONS POUR LA CAMÉRA ///////////////////
//////////////////////////////////////////////////////////
void rafraichirCamera()
{
   x = rho*cos(theta)*sin(phi);
   y = rho*sin(theta)*sin(phi);
   z = rho*cos(phi);

   gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
}



//////////////////////////////////////////////////////////
//////  FONCTIONS POUR LES LISTES D'AFFICHAGE ////////////
//////////////////////////////////////////////////////////
void compileCube(int listID, float size, int nbRows, int nbColums, bool outsideNormal) {
   // dessiner la pièce
   glNewList(listID , GL_COMPILE);

      // face du fond
      glPushMatrix();
         glTranslatef(0.0f, size/2.0f, 0.0f);
         glCallList(QUAD_LIST);
      glPopMatrix();

      // face de derrière
      glPushMatrix();
         glTranslatef(0.0f, -size/2.0f, 0.0f);
         glRotatef(180.0, 0.0, 0.0, 1.0);
         glCallList(QUAD_LIST);
      glPopMatrix();

      // face de gauche
      glPushMatrix();
         glTranslatef(-size/2.0f, 0.0f, 0.0f);
         glRotatef(90.0, 0.0, 0.0, 1.0);
         glCallList(QUAD_LIST);
      glPopMatrix();

      // face de droite
      glPushMatrix();
         glTranslatef(size/2.0f, 0.0f, 0.0f);
         glRotatef(-90.0, 0.0, 0.0, 1.0);
         glCallList(QUAD_LIST);
      glPopMatrix();

      // face du bas
      glPushMatrix();
         glTranslatef(0.0f, 0.0f, size/2.0f);
         glRotatef(90.0, 1.0, 0.0, 0.0);
         glCallList(QUAD_LIST);
      glPopMatrix();

      // face du haut
      glPushMatrix();
         glTranslatef(0.0f, 0.0f, -size/2.0f);
         glRotatef(-90.0, 1.0, 0.0, 0.0);
         glCallList(QUAD_LIST);
      glPopMatrix();
      
   glEndList();
}



void compileGrilleQuads(int listID, float size, int nbRows, int nbColums, bool outsideNormal)
{

   glNewList(listID, GL_COMPILE);
      appliquerTextures();
      dessinerGrilleQuads(size, nbRows, nbColums, outsideNormal);
   glEndList();
}



void dessinerGrilleQuads(float size, int nbRows, int nbColums, bool outsideNormal) {
   
   int normFact = outsideNormal ? 1 : -1;
   
   float startX = -size/2.0f;
   float startY = 0.0f;
   float startZ = size/2.0f;
   float s1, t1, s2, t2;

   float incrementX = size / (float)nbColums;
   float incrementZ = size / (float)nbRows;

   glBegin(GL_QUADS);
   glNormal3f(0.0, normFact, 0.0);

   for (int i=0;i<nbColums;i++) {
      for (int k=0;k<nbRows;k++) {

         s1 = (i*incrementX)/size;
         s2 = ((i+1)*incrementX)/size;
         t1 = (k*incrementZ)/size;
         t2 = ((k+1)*incrementZ)/size;

         glMultiTexCoord2f(GL_TEXTURE0, s1, t1); 
         glMultiTexCoord2f(GL_TEXTURE1, s1, t1); 
         glMultiTexCoord2f(GL_TEXTURE2, s1, t1); 
         glVertex3f(startX + i*incrementX, 0.0f, startZ - k*incrementZ);

         glMultiTexCoord2f(GL_TEXTURE0, s2, t1); 
         glMultiTexCoord2f(GL_TEXTURE1, s2, t1); 
         glMultiTexCoord2f(GL_TEXTURE2, s2, t1); 
         glVertex3f((startX + i*incrementX) + incrementX, 0.0f, startZ - k*incrementZ);

         glMultiTexCoord2f(GL_TEXTURE0, s2, t2); 
         glMultiTexCoord2f(GL_TEXTURE1, s2, t2); 
         glMultiTexCoord2f(GL_TEXTURE2, s2, t2); 
         glVertex3f((startX + i*incrementX) + incrementX, 0.0f, (startZ - k*incrementZ) - incrementZ);

         glMultiTexCoord2f(GL_TEXTURE0, s1, t2); 
         glMultiTexCoord2f(GL_TEXTURE1, s1, t2); 
         glMultiTexCoord2f(GL_TEXTURE2, s1, t2); 
         glVertex3f(startX + i*incrementX, 0.0f, (startZ - k*incrementZ) - incrementZ);
      }
   }
   glEnd();
}



//////////////////////////////////////////////////////////
///////////  FONCTIONS POUR L'ÉCLAIRAGE //////////////////
//////////////////////////////////////////////////////////
void rafraichirEclairage()
{
   // inscription des variables de lumière dans le nuanceur
  
   if (pointLightAllumee) {
      glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
      glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
      glLightfv(GL_LIGHT0, GL_EMISSION, light_emission0);
      glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

      glEnable(GL_LIGHT0);
      glUniform1i(glGetUniformLocation(fixedPipelineShaderProg, "pointLightOn"), 1);
   }
   else {
      glDisable(GL_LIGHT0);
      glUniform1i(glGetUniformLocation(fixedPipelineShaderProg, "pointLightOn"), 0);
   }

   if (spotLightAllumee) {
      // variables position et spotDirection pour la "flashLight")
      GLfloat spotPosition[4] =  { x, y, z, 1.0};
      GLfloat spotDirection[3] = {-x,-y,-z};

      glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
      glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
      glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
      glLightfv(GL_LIGHT1, GL_EMISSION, light_emission1);
      glLightfv(GL_LIGHT1, GL_POSITION, spotPosition);
      glLightf (GL_LIGHT1, GL_SPOT_EXPONENT, 1.0);
      glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, 30.0); 
      glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDirection); 

      glEnable(GL_LIGHT1);
      glUniform1i(glGetUniformLocation(fixedPipelineShaderProg, "spotLightOn"), 1);
   }
   else {
      glDisable(GL_LIGHT1);
      glUniform1i(glGetUniformLocation(fixedPipelineShaderProg, "spotLightOn"), 0);
   }

   if (dirLightAllumee) {
      glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient2);
      glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);
      glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);
      glLightfv(GL_LIGHT2, GL_EMISSION, light_emission2);
      glLightfv(GL_LIGHT2, GL_POSITION, light_position2);

      glEnable(GL_LIGHT2);
      glUniform1i(glGetUniformLocation(fixedPipelineShaderProg, "dirLightOn"), 1);
   }
   else {
      glDisable(GL_LIGHT2);
      glUniform1i(glGetUniformLocation(fixedPipelineShaderProg, "dirLightOn"), 0);
   }
}



//////////////////////////////////////////////////////////
//////////  FONCTIONS POUR LA PROJECTION /////////////////
//////////////////////////////////////////////////////////
void rafraichirProjection()
{
   if (isPerspective) {
       gluPerspective(60.0, (GLfloat)currentW / (GLfloat) currentH, 0.1, 300.0);
   } else {
       if (currentW <= currentH) 
           glOrtho (-10.0, 10.0, -10.0*(GLfloat)currentH/(GLfloat)currentW, 
               10.0*(GLfloat)currentH/(GLfloat)currentW, 0.01, 300.0);
       else 
           glOrtho (-10.0*(GLfloat)currentW/(GLfloat)currentH, 
               10.0*(GLfloat)currentW/(GLfloat)currentH, -10.0, 10.0, 0.01, 300.0);
   }
}



//////////////////////////////////////////////////////////
//////////  FONCTIONS POUR LES NUANCEURS /////////////////
//////////////////////////////////////////////////////////
void afficherShaderInfoLog(GLuint obj, char* message)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

   // afficher le message d'en-tête
   printf("%s\n", message);

   // afficher le message d'erreur, le cas échéant
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 1)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	} 
   else 
   {
      printf("Aucune erreur :-)\n\n");
   }
}




void afficherProgramInfoLog(GLuint obj, char* message)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

   // afficher le message d'en-tête
   printf("%s\n", message);

   // afficher le message d'erreur, le cas échéant
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	} 
   else 
   {
      printf("Aucune erreur :-)\n\n");
   }
}



void initialiserNuanceurs() 
{	
	// définition des nuanceurs à créer ci-bas...
   GLuint nuanceurSommets;
   GLuint nuanceurFragments;

   // déclaration des chaînes qui devront recevoir le code des nuanceurs
	char *ns = NULL;
	char *nf = NULL;

	// création des nuanceurs
	nuanceurSommets = glCreateShader(GL_VERTEX_SHADER);
	nuanceurFragments = glCreateShader(GL_FRAGMENT_SHADER);

	// lecture du code des nuanceurs
	ns = textFileRead("nuanceurs/nuanceurSommets.glsl");
	nf = textFileRead("nuanceurs/nuanceurFragments.glsl");

	// créer des pointeurs sur le texte du code des nuanceurs
	const char * ns_ptr = ns;
	const char * nf_ptr = nf;

	// sourcer les nuanceurs
	glShaderSource(nuanceurSommets, 1, &ns_ptr, NULL);
	glShaderSource(nuanceurFragments, 1, &nf_ptr, NULL);

	// libérer la mémoire des codes source
	free(ns);
	free(nf);

   // compiler les nuanceurs et vérifier les erreurs
   glCompileShader(nuanceurSommets);
   glCompileShader(nuanceurFragments);

   // Afficher les erreurs de compilation !!
   afficherShaderInfoLog(nuanceurSommets, "ERREURS DE COMPILATION DU NUANCEUR DE SOMMETS : ");
   afficherShaderInfoLog(nuanceurFragments, "ERREURS DE COMPILATION DU NUANCEUR DE FRAGMENTS : ");

	// créer le programme des nuanceurs et lier
	fixedPipelineShaderProg = glCreateProgram();
	glAttachShader(fixedPipelineShaderProg,nuanceurSommets);
	glAttachShader(fixedPipelineShaderProg,nuanceurFragments);
	glLinkProgram(fixedPipelineShaderProg);

	// afficher les erreurs de compilation et de linkage du programme
   afficherProgramInfoLog(fixedPipelineShaderProg, "ERREURS DE L'EDITION DES LIENS : ");    

   glUseProgram(fixedPipelineShaderProg);
}



//////////////////////////////////////////////////////////
//////////  FONCTIONS POUR LES TEXTURES //////////////////
//////////////////////////////////////////////////////////
void appliquerTextures()
{
	// ETAGE 0 (texture de mur)
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture0);
	glUniform1i(glGetUniformLocation(fixedPipelineShaderProg, "texUnit0"), 0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	// ETAGE 1 (texture de rust)
	float white[] = { 1.0, 1.0, 1.0, 1.0 };
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glUniform1i(glGetUniformLocation(fixedPipelineShaderProg, "texUnit1"), 1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
	glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE , 2.0);
	// ETAGE 2 (texture "paint job")
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(fixedPipelineShaderProg, "texUnit2"), 2);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);   //Modulate RGB with RGB*/
}



SImage* LoadBMP( char *Filename )
{
    if( !Filename )
        return NULL;

    FILE *file;
    unsigned int  width;
    unsigned int  height;
	unsigned long i;
	unsigned short int planes; // planes in image (must be 1) 
	unsigned short int bpp;    // bits per pixel (must be 24)
	
	// make sure the file is there.
	if( ( file = fopen( Filename, "rb" ) ) == NULL )
    {
		printf( "File Not Found : %s\n", Filename );
		return NULL;
	}

	// seek through the bmp header, up to the width/height:
	fseek( file, 18, SEEK_CUR );

	// read the width
	if( ( i = ( unsigned long )fread( &width, 4, 1, file ) ) != 1 )
    {
		printf( "Error reading width from %s.\n", Filename );
		return NULL;
	}

	// read the height 
	if( ( i = ( unsigned long )fread( &height, 4, 1, file ) ) != 1 )
    {
		printf( "Error reading height from %s.\n", Filename );
		return NULL;
	}

	// read the planes
	if( ( ( unsigned long )fread( &planes, 2, 1, file ) ) != 1 )
    {
		printf( "Error reading planes from %s.\n", Filename );
		return NULL;
	}

	if( planes != 1 )
    {
		printf( "Planes from %s is not 1: %u\n", Filename, planes );
		return NULL;
	}

	// read the bpp
	if( ( i = ( unsigned long )fread( &bpp, 2, 1, file ) ) != 1 )
    {
		printf("Error reading bpp from %s.\n", Filename );
		return NULL;
	}

	if( bpp != 24 )
    {
		printf( "Bpp from %s is not 24: %u\n", Filename, bpp );
		return NULL;
	}

	// seek past the rest of the bitmap header.
	fseek( file, 24, SEEK_CUR );

	// read the data.
    SImage* Image = new SImage( width, height );

	if( ( i = ( unsigned long )fread( Image->data, width * height * 3, 1, file ) ) != 1 )
    {
		printf("Error reading image data from %s.\n", Filename );
		delete Image;
		return NULL;
	}

	return Image;
}



void init2DTexture(GLint texName, GLint texWidth, GLint texHeight, GLubyte *texPtr)
{
   glBindTexture(GL_TEXTURE_2D, texName);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, 1.0); 
   glTexImage2D(GL_TEXTURE_2D, 0, 3, texWidth, texHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, texPtr);
}



void chargerTextures()
{	
   SImage* Image = NULL;

   if( Image = LoadBMP("textures/stonewalldiffuse.bmp" ) )
   {
      glGenTextures( 1, &texture0 );	
      init2DTexture( texture0, Image->width, Image->height, Image->data );
      delete Image;
   }

   if( Image = LoadBMP( "textures/rust.bmp" ) )
   {
      glGenTextures( 1, &texture1 );	
      init2DTexture( texture1, Image->width, Image->height, Image->data );
      delete Image;
   }

   if( Image = LoadBMP( "textures/3dlabs.bmp" ) )
   {
      glGenTextures( 1, &texture2 );
      init2DTexture( texture2, Image->width, Image->height, Image->data );
      delete Image;
   }
}


void desactiverTextures()
{
   glActiveTexture(GL_TEXTURE0);
   glDisable(GL_TEXTURE_2D);
   glActiveTexture(GL_TEXTURE1);
   glDisable(GL_TEXTURE_2D);
   glActiveTexture(GL_TEXTURE2);
   glDisable(GL_TEXTURE_2D);
}
