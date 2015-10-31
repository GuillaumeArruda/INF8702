#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

///////////////////////////////////////////////////////////////////////////////
///  @file Geometry.h
///
///  @brief    Ce fichier contient des définitions d'éléments géométriques tels que :
///            un vertex, un triangle et un quad.
///
///  @author  Olivier Dionne 
///  @date    29/10/2008
///  @version 1.0.0
///
///////////////////////////////////////////////////////////////////////////////

#include "Vecteur3.h"

using namespace Math3D;

struct Vertex
{
    float Texture[ 2 ];
    CVecteur3 Normal;
    CVecteur3 Position;
    

    Vertex( void )
        : Normal( CVecteur3:: ZERO )
        , Position( CVecteur3::ZERO )
    {
        Texture[ 0 ] = 0.0f;
        Texture[ 1 ] = 0.0f;
    }

    Vertex( const CVecteur3& P, const CVecteur3& N = CVecteur3::ZERO )
        : Normal( N )
        , Position( P )
    {
        Texture[ 0 ] = 0.0f;
        Texture[ 1 ] = 0.0f;
    }
};

struct Triangle
{
    unsigned int Vertex[ 3 ];

    Triangle( void )
    {
        memset( Vertex, 0, 3 * sizeof( unsigned int ) );
    }

    Triangle( const int V0, const int V1, const int V2 )
    {
        Vertex[ 0 ] = V0;
        Vertex[ 1 ] = V1;
        Vertex[ 2 ] = V2;
    }
};

struct Quad
{
    unsigned int Vertex[ 4 ];

    Quad( void )
    {
        memset( Vertex, 0, 4 * sizeof( unsigned int ) );
    }

    Quad( const int V0, const int V1, const int V2, const int V3 )
    {
        Vertex[ 0 ] = V0;
        Vertex[ 1 ] = V1;
        Vertex[ 2 ] = V2;
        Vertex[ 3 ] = V3;
    }
};

#endif