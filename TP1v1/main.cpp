#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <glew.h>
#include <glut.h>
#include "textfile.h"


///////////////////////////////////////////////
// déclaration des macros et defines globaux //
///////////////////////////////////////////////
#ifndef M_PI
#define M_PI 3.1415926535897932385
#endif
#define DEG2RAD( a ) ( a / 180.0f ) * ( M_PI );
#define CLIP( a, min, max ) ( a < min ) ? min : ( ( a > max ) ? max : a )

/////////////////////////////////////////
// déclaration des constantes globales //
/////////////////////////////////////////

const float freqRotX     = 0.15f;                 // la fréquence de rotation en X du modèle
const float freqRotY     = 0.1f;                  // la fréquence de rotation en Y du modèle
const float freqRotZ     = 0.2f;                  // la fréquence de rotation en Z du modèle

const float frameRate    = 50.0f;                 // le framerate cible de l'application graphique.                   

const float modelSize    = 10.0f;                 // la grandeur du modèle
const float axeScaleFact = 15.0f;                 // le facteur multiplicatif des axes unitaires dessinés dans la scène
const GLfloat fogColor[] = { 0.0f, 0.0f, 0.0f };  // la couleur du FOG dans l'application

const bool rotAutoModele = false;                 // la rotation automatique du modèle ou non



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

int   modeCouleur = 1;           // conserve le mode Couleur (1=rouge, 2=vert, 3=bleu)
float valeurRouge = 1.0f;        // la composante rouge du cube en nuanceurs
float valeurVert  = 0.0f;        // la composante verte du cube en nuanceurs
float valeurBleu  = 0.0f;        // la composante bleue du cube en nuanceurs

bool premierMouvement = true;    // indique si la souris fut bougée déjà
int dernierX = 0;                // la dernière valeur en X de position lue de la souris
int dernierY = 0;                // la derniere valeur en Y de position lus de la souris

bool SHADER_ON = false ;         // indique si l'on veut utiliser le nuanceur

#define QUAD_LIST 1              // déclaration de la liste d'affichage du modèle  
#define CUBE_LIST 2              // déclaration de la liste d'affichage d'un cube 

GLuint noShaderProg = 0 ;        // Un "nuanceur" NUL ==> représentant la fonctionalité fixe d'OPENGL (=0)
GLuint fixedPipelineShaderProg;	 // Votre programme de nuanceur.

bool useOldGL = true;	//indique si l'on veut utiliser la lsite d'affichage ou bien le vbo pour dessiner le modèle
bool activerIPS = false;
int currentTime = 0;
GLuint vertexBufferObject;
GLuint indexBufferObject;
// Indique les positions des sommets composant le cube
float vertexPositions[] = { 
	5.0f, -5.0f, -5.0f, 
	5.0f, -5.0f, 5.0f,
	-5.0f, -5.0f, 5.0f,
	-5.0f, -5.0f, -5.0f,
	5.0f, 5.0f, -5.0f,
	5.0f, 5.0f, 5.0f,
	-5.0f, 5.0f, 5.0f,
	-5.0f, 5.0f, -5.0f};

	// Indique les index des sommets composant les faces du cube 
	unsigned int cubeFaces[] = { 
		0, 1, 2, 3,
		4, 7, 6, 5,
		0, 4, 5, 1,
		1, 5, 6, 2,
		2, 6, 7, 3,
		4, 0, 3, 7};

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
		void drawQuadGrid(float size, int nbRows, int nbColums, bool outsideNormal);
		void rafraichirProjection();
		void initialiserNuanceurs();
		void initialiserVBO();
		void afficherShaderInfoLog(GLuint obj);
		void afficherProgramInfoLog(GLuint obj);
		void afficherIPS();



		// initialisation d'openGL
		void initialisation (void)
		{
			// modèle de rendu
			glShadeModel(GL_SMOOTH);	

			//  background to be cleared to black
			glClearColor (0.2, 0.2, 0.2, 1.0);

			// activer les etats openGL
			glEnable(GL_NORMALIZE);
			glEnable(GL_DEPTH_TEST);

			// fonction de profondeur
			glDepthFunc(GL_LEQUAL);

			// amélioration de la perspective
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			// compiler les listes d'affichage
			compileGrilleQuads(QUAD_LIST, modelSize, 1, 1, true);
			compileCube(CUBE_LIST, modelSize, 1, 1, true);
		}



		void dessinerScene()
		{
			if (SHADER_ON)
			{
				glUseProgram(fixedPipelineShaderProg);

				// injecter les valeurs de couleur dans le nuanceur
				// ...
				// ...
				// ...
			} 
			else
			{
				glUseProgram(noShaderProg);
			}

			// Dessiner le modèle 
			glPushMatrix();
			{
				// dessiner le cube tournant, si désiré
				if (rotAutoModele) 
				{
					angleRotX += 360.0 * (freqRotX / frameRate);
					angleRotY += 360.0 * (freqRotY / frameRate);
					angleRotZ += 360.0 * (freqRotZ / frameRate);

					if (angleRotX >= 360.0) angleRotX -= 360.0;
					if (angleRotY >= 360.0) angleRotY -= 360.0;
					if (angleRotZ >= 360.0) angleRotZ -= 360.0;
				}	

				glRotated(180.0, 1.0, 0.0, 0.0);
				glRotated(angleRotX, 1.0, 0.0, 0.0);
				glRotated(angleRotY, 0.0, 1.0, 0.0);
				glRotated(angleRotZ, 0.0, 0.0, 1.0);

				// appliquer le matériau du cube
				glColor4f(0.0, 0.0, 1.0, 1.0);

				if(useOldGL)
				{
					glCallList(CUBE_LIST);
				}
				else
				{
					// Compléter ici pour l'utilisation du VBO avec l'IBO.........
					//
					glEnableClientState(GL_VERTEX_ARRAY);
					glBindBuffer(GL_VERTEX_ARRAY,vertexBufferObject);
					glBindBuffer(GL_INDEX_ARRAY, indexBufferObject);
					glVertexPointer(3, GL_FLOAT, 0, NULL);
					glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, NULL);
					glDisableClientState(GL_VERTEX_ARRAY);
				}
			}
			glPopMatrix();

			glUseProgram(noShaderProg); 

			// Dessiner les axes pour se retrouver
			glPushMatrix();
			{		
				glLineWidth(3.0);

				glBegin(GL_LINES);
				{
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
			}
			glPopMatrix();

			glUseProgram( SHADER_ON ? fixedPipelineShaderProg : noShaderProg );

			if(activerIPS)
				afficherIPS();

			// flush les derniers vertex du pipeline graphique
			glFlush();
		}



		// fonction d'affichage
		void affichage(void)
		{
			// effacement de l'ecran et du tampon de profondeur
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			// charger matrice de modélisation
			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();

			// mettre à jour la caméra openGL
			rafraichirCamera();  // ne pas permettre pour l'instant de déplacer la caméra...

			// dessiner la scène
			dessinerScene(); 

			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();

			// mettre à jour la projection
			rafraichirProjection();

			// utiliser le "double-buffering"
			glutSwapBuffers();
		}



		// fonction appelee lorsque l'application ne fait rien
		void attente(void)
		{
			glutPostRedisplay();
			/*
			// une variable pour memoriser le temps a attendre
			static int nWaitUntil = glutGet( GLUT_ELAPSED_TIME );

			// on recupere le temps présent
			int nTimer = glutGet( GLUT_ELAPSED_TIME );
			// et on le compare a l'instant qu'il faut attendre
			if( nTimer >= nWaitUntil )
			{
			// pour rafraichir l'affichage
			glutPostRedisplay();
			// 5 fois pa5 seconde
			nWaitUntil = nTimer + ( 1000 / frameRate );
			}
			*/
		}



		// fonction de gestion du clavier
		void clavier(unsigned char touche, int x, int y)
		{
			// traitement des touches q et echap
			switch(touche) 
			{
			case 'q' :
				// destruction de la fenetre GLUT
				glutDestroyWindow(g_nWindowID);
				// on quitte notre programme
				exit(EXIT_SUCCESS);
				break;

				// permutation de la perspective
			case 'p' :
				if (isPerspective)
					isPerspective = false;
				else
					isPerspective = true;
				affichage();
				break;

				// permutation de l'activation des shaders
			case 'x' :
				if (SHADER_ON)
					SHADER_ON = false;
				else
					SHADER_ON = true;
				glutPostRedisplay();
				break;

				// ajustement du zoom
			case '-' :
				// la caméra recule
				rho += 2.0f;
				affichage();
				break;

				// ajustement du zoom
			case '=' :
				// la caméra avance
				rho -= 2.0f;
				if (rho <= 1.0)
					rho = 1.0f;
				affichage();
				break;
				// activation/desactivation du VBO
			case 'v' :
				useOldGL = !useOldGL;
				if(useOldGL)
					printf("desactivation VBO\n");
				else
					printf("activation VBO\n");
				break;
			case 'f':
					activerIPS = !activerIPS;
					break;
			}
		}



		// fonction de gestion du clavier spécial
		void clavierSpecial(int touche, int x, int y)
		{
			// traitement des touches q et echap
			switch(touche) 
			{
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

			case GLUT_KEY_F1 :
				// active le mode "ROUGE"
				modeCouleur = 1;
				break;

			case GLUT_KEY_F2 :
				// active le mode "VERT"
				modeCouleur = 2;
				break;

			case GLUT_KEY_F3 :
				// active le mode "BLEU"
				modeCouleur = 3;
				break;

			case GLUT_KEY_PAGE_UP :
				// ajouter de la couleur à la composant activée
				if (modeCouleur == 1)
				{
					valeurRouge = CLIP(valeurRouge + 0.05f, 0.0, 1.0);
				} 
				else if (modeCouleur == 2)
				{
					valeurVert  = CLIP(valeurVert  + 0.05f, 0.0, 1.0);
				} 
				else
				{
					valeurBleu  = CLIP(valeurBleu  + 0.05f, 0.0, 1.0);
				}
				affichage();
				break;

			case GLUT_KEY_PAGE_DOWN :
				// enlever de la couleur à la composant activée
				if (modeCouleur == 1)
				{
					valeurRouge = CLIP(valeurRouge - 0.05f, 0.0, 1.0);
				} 
				else if (modeCouleur == 2)
				{
					valeurVert  = CLIP(valeurVert  - 0.05f, 0.0, 1.0);
				} 
				else
				{
					valeurBleu  = CLIP(valeurBleu  - 0.05f, 0.0, 1.0);
				}
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
			if (state == GLUT_DOWN)
			{
				// on vient de presser la souris
				dernierX = x;
				dernierY = y;
			} 
			else
			{
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
			g_nWindowID = glutCreateWindow("TP1v1");

			// referencement de la fonction de rappel pour l'affichage
			glutDisplayFunc(affichage);
			// referencement de la fonction de rappel pour l'attente
			glutIdleFunc(attente);
			// referencement de la fonction de gestion des touches
			glutKeyboardFunc(clavier);
			// referencement de la fonction de gestion spéciale des touches extended
			glutSpecialFunc(clavierSpecial);
			// referencement de la fonction de rappel pour le redimensionnement
			glutReshapeFunc (redimensionnement);
			// référencement de la fonction de rappel pour le mouvement de la souris
			glutMotionFunc (mouvementSouris);
			// référencement de la fonction de rappel pour le click de la souris
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

			// compiler et linker les shaders
			initialiserNuanceurs();

			// creer les vbos
			initialiserVBO();

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
			{
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
			}
			glEndList();
		}



		void compileGrilleQuads(int listID, float size, int nbRows, int nbColums, bool outsideNormal)
		{
			glNewList(listID, GL_COMPILE);
			drawQuadGrid(size, nbRows, nbColums, outsideNormal);
			glEndList();
		}



		void drawQuadGrid(float size, int nbRows, int nbColums, bool outsideNormal) {

			int normFact = outsideNormal ? 1 : -1;

			float startX = -size/2.0f;
			float startY = 0.0f;
			float startZ = size/2.0f;
			float s1, t1, s2, t2;

			float incrementX = size / (float)nbColums;
			float incrementZ = size / (float)nbRows;

			glBegin(GL_QUADS);
			glNormal3f(0.0, normFact, 0.0);

			for (int i=0;i<nbColums;i++)
			{
				for (int k=0;k<nbRows;k++)
				{

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
		//////////  FONCTIONS POUR LA PROJECTION /////////////////
		//////////////////////////////////////////////////////////
		void rafraichirProjection()
		{
			if (isPerspective)
			{
				gluPerspective(60.0, (GLfloat)currentW / (GLfloat) currentH, 0.1, 300.0);
			} 
			else
			{
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

		void initialiserVBO()
		{
			// définition du VBO et de l'IBO à utiliser
			glGenBuffers(1, &vertexBufferObject);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3, vertexPositions, GL_STATIC_DRAW);

			glGenBuffers(1, &indexBufferObject);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 8 * 3, cubeFaces, GL_STATIC_DRAW);
		}

		void initialiserNuanceurs() 
		{	
			// définition des nuanceurs à créer ci-bas...
			GLuint nuanceurSommets;
			GLuint nuanceurFragments;

			// L'objet-programme du nuanceur, lui, est déjà défini globalement...
			/* GLuint fixedPipelineShaderProg; */
			nuanceurSommets = glCreateShader(GL_VERTEX_SHADER);
			nuanceurFragments = glCreateShader(GL_FRAGMENT_SHADER);


			// déclaration des chaînes qui devront recevoir le code des nuanceurs
			const GLchar *ns = NULL;
			const GLchar *nf = NULL;

			// compléter la compilation des nuanceurs
			ns = textFileRead("nuanceurs\\nuanceurSommets.glsl");
			nf = textFileRead("nuanceurs\\nuanceurFragments.glsl");
			GLint sizeNS = 0;
			GLint sizeNF = 0;
			while (ns[sizeNS] != '\0')
			{
				++sizeNS;
			}
			while (nf[sizeNF] != '\0')
			{
				++sizeNF;
			}
			glShaderSource(nuanceurSommets, 1, &ns, &sizeNS);
			glShaderSource(nuanceurFragments, 1, &nf, &sizeNF);
			glCompileShader(nuanceurSommets);
			glCompileShader(nuanceurFragments);
			// À DÉCOMMENTER
			// Afficher les erreurs de compilation !!
			afficherShaderInfoLog(nuanceurSommets, "ERREURS DE COMPILATION DU NUANCEUR DE SOMMETS : ");
			afficherShaderInfoLog(nuanceurFragments, "ERREURS DE COMPILATION DU NUANCEUR DE FRAGMENTS : ");

			// créer le programme des nuanceurs et lier
			fixedPipelineShaderProg =  glCreateProgram();
			glAttachShader(fixedPipelineShaderProg, nuanceurSommets);
			glAttachShader(fixedPipelineShaderProg, nuanceurFragments);
			glLinkProgram(fixedPipelineShaderProg);
			// À DÉCOMMENTER
			// afficher les erreurs de compilation et de linkage du programme
			afficherProgramInfoLog(fixedPipelineShaderProg, "ERREURS DE L'EDITION DES LIENS : ");


			glDetachShader(fixedPipelineShaderProg, nuanceurSommets);
			glDetachShader(fixedPipelineShaderProg, nuanceurFragments);
			glDeleteShader(nuanceurSommets);
			glDeleteShader(nuanceurFragments);



			glUseProgram( fixedPipelineShaderProg );
		}

		void afficherIPS()
		{
			glColor3f(1.0f,0.0f,0.0f);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(0, 800, 0, 800);
			glScalef(1, -1, 1);
			glTranslatef(0, -1*800.0f, 0);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glEnable(GL_BLEND);
			glPushMatrix();
			glLoadIdentity();


			char* timeString = new char[20];
			int t = glutGet(GLUT_ELAPSED_TIME);
			sprintf_s(timeString,20,"FPS: %4.2f",1000.0f/(t-currentTime));
			char *c;
			float x1=10.0f;
			glRasterPos2f(10.0f,20.0f);
			for (c=timeString; *c != '\0'; c++) 
			{
				glutBitmapCharacter((void *)GLUT_BITMAP_HELVETICA_18, *c);
				x1 = x1 + glutBitmapWidth((void *)GLUT_BITMAP_HELVETICA_18,*c) + 3;
			}

			currentTime = t;

			glDisable(GL_BLEND);

			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			delete [] timeString;
		}