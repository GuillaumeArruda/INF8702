///////////////////////////////////////////////////////////////////////////////
///  @file    OBJParser.h
///  @author  Olivier Dionne 
///  @brief   Déclare une classe implémentant un parser de fichier type wavefront OBJ
///  @date    2008-10-29
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#ifndef __OBJ_PARSER__
#define __OBJ_PARSER__

#include <vector>
#include <iostream>

#include "StringUtils.h"
#include "ParserTextAbstrait.h"
#include "Geometry.h"
using namespace std;

///////////////////////////////////////////////////////////////////////////
/// @class COBJParser
/// @brief Cette classe représente un parser de fichier format wavefront OBJ.
///
/// @author Olivier Dionne
/// @date   29/10/2008 
///////////////////////////////////////////////////////////////////////////
class COBJParser : public CParserTextAbstrait
{
	public:
		COBJParser( void );
		virtual ~COBJParser( void );

		void ParseFile( void );

        inline const vector<Vertex>   GetVertices ( void ) const;
        inline const vector<Triangle> GetTriangles( void ) const;
        inline const vector<Quad>     GetQuads    ( void ) const;
        inline const CVecteur3        GetCentroid ( void ) const;

	private:
		vector<Vertex>   m_Vertices;
		vector<Triangle> m_Triangles;
        vector<Quad>     m_Quads;

        CVecteur3        m_Centroid;
};

const vector<Vertex> COBJParser::GetVertices( void ) const
{
    return m_Vertices;
}

const vector<Triangle> COBJParser::GetTriangles( void ) const
{
    return m_Triangles;
}

const vector<Quad> COBJParser::GetQuads( void ) const
{
    return m_Quads;
}

const CVecteur3 COBJParser::GetCentroid( void ) const
{
    return m_Centroid;
}

#endif
