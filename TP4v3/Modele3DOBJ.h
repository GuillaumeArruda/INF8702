///////////////////////////////////////////////////////////////////////////////
///  @file Modele3D.h
///  @author  Frédéric Plourde
///  @brief   Déclare la classe CModele3DOBJ qui encapsule un modele3D spécifique 
///           chargé à partir d'un fichier OBJ
///  @date    2008-10-27
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "modele3dabstrait.h"
#include "Cst.h"
#include "Texture2D.h"
#include "Objparser/OBJParser.h"
#include <vector>

///////////////////////////////////////////////////////////////////////////////
///  CModele3DOBJ
///  Cette classe encapsule l'objet grahique principal du TP : un modèle 3D chargé
///  à partir d'un fichier OBJ
///
///  Base class CModele3DOBJAbstrait La classe de base encapsule tout modèle 3D openGL
///
///
///  @author Frédéric Plourde 
///  @date   2008-10-27
///
///////////////////////////////////////////////////////////////////////////////
class CModele3DOBJ :
    public CModele3DAbstrait
{
public:
    /// constructeur spécifique pour création du modèle 3D ne comportant qu'une seule texture
    CModele3DOBJ(char* nomFichierOBJ, char * nomFichierTexture, float echelle = 1.0);
    /// constructeur spécifique pour création du modèle 3D comportant plusieurs textures
    CModele3DOBJ(char* nomFichierOBJ, std::vector<char*> *nomsFichiersTextures, float echelle = 1.0);
    /// destructeur par défaut
    ~CModele3DOBJ(void);

    /// Fonction qui affiche le modèle 3D à partir des tableaux de sommets lus dans le fichier source OBJ.
    static void afficherModele3D();
    /// fonction qui charge en mémoire les sommets et les normales d'un modèle définit dans un fichier .OBJ.
    static void chargerModeleOBJ(char* fichierOBJ);
    /// fonction à implémenter pour définir la géométrie openGL du modèle 3D (est statique, car est référée par un pointeur de fonction)
    static void dessiner();
    /// fonction à implémenter pour personnaliser l'application de texture (à faire aussi : dans le constructeur, appeler "construire(&dessiner, &appliquerTextures)"
    static void appliquerTextures(void*);

private:
    /// le tableau des sommets du modèle 3D
    static vector<Vertex>   g_Vertices_;

    /// le tableau des triangles du modèle 3D
    static vector<Triangle> g_Triangles_;

    /// le tableau des quads du modèle 3D
    static vector<Quad>     g_Quads_;

    /// le centroïde du modèle3D
    static CVecteur3        g_ObjCentroid_;
};
