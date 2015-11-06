///////////////////////////////////////////////////////////////////////////////
///  @file ParserTextAbstrait.cpp
///  @author  Olivier Dionne 
///  @brief   Déclare une classe générique pour parser des fichiers texts.
///  @date    2008-10-29
///  @version 1.0
///
///////////////////////////////////////////////////////////////////////////////
#include "ParserTextAbstrait.h"

///////////////////////////////////////////////////////////////////////////////
///  public overloaded constructor  CParserTextAbstrait \n
///
///  Initialise les données du Parser abstrait
///
///  @return Aucune
///
///  @author Olivier Dionne
///  @date   2008-10-29
///
///////////////////////////////////////////////////////////////////////////////
CParserTextAbstrait::CParserTextAbstrait( void )
	: m_Filename( "" )
	, m_FileStream()
	, m_IsOpen( false )
{
}

///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CParserTextAbstrait \n
///
///  @return Aucune
///
///  @author Olivier Dionne
///  @date   2008-10-29
///
///////////////////////////////////////////////////////////////////////////////
CParserTextAbstrait::~CParserTextAbstrait( void )
{
	
}	

///////////////////////////////////////////////////////////////////////////////
///  public  OpenFile \n
///
///  Cette méthode ouvert un fichier text en lecture seulement
///  
///  @param [in]       Filename string&   Le nom du fichier text a ouvrir
///
///  @return Aucune
///
///  @author Olivier Dionne
///  @date   2008-10-29
///
///////////////////////////////////////////////////////////////////////////////
bool CParserTextAbstrait::OpenFile( const string& Filename )
{
	m_Filename = Filename;
	m_FileStream.open( Filename.c_str(), ios::in );

	m_IsOpen = m_FileStream.is_open();

	return m_IsOpen;
}

///////////////////////////////////////////////////////////////////////////////
///  public  CloseFile \n
///
///  Cette méthode ferme le fichier text
///  
///  @return Aucune
///
///  @author Olivier Dionne
///  @date   2008-10-29
///
///////////////////////////////////////////////////////////////////////////////
void CParserTextAbstrait::CloseFile( void )
{
	if( m_IsOpen )
	{
		m_FileStream.close();
		m_IsOpen = false;
		m_Filename.clear();
	}
}
