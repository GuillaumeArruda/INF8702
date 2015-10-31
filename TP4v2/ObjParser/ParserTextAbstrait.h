///////////////////////////////////////////////////////////////////////////////
///  @file ParserTextAbstrait.h
///  @author  Olivier Dionne 
///  @brief   D�clare une classe g�n�rique pour parser des fichiers texts.
///  @date    2008-10-29
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#ifndef __PARSER_TEXT_ABSTRAIT__
#define __PARSER_TEXT_ABSTRAIT__

#include <fstream>
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///  CParserTextAbstrait
///  Cette classe repr�sente une base g�n�rique pour la d�claration
///  de parser text.
///
///  @author Olivier Dionne 
///  @date   29/10/2008 
///
///////////////////////////////////////////////////////////////////////////////
class CParserTextAbstrait
{
	public:
		CParserTextAbstrait( void );
		virtual ~CParserTextAbstrait( void );

		bool OpenFile( const string& Filename );
		void CloseFile( void );
		
		virtual void ParseFile( void ) = 0;

	protected:
		string  m_Filename;
	    fstream m_FileStream;
		bool	m_IsOpen;
};

#endif
