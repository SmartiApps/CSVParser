// CSVParser.cpp: implementation of the CCSVParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CSVParsing.h"
#include "CSVParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCSVParser::CCSVParser()
{
	m_ptrFileContent = NULL ;
	m_strFileName.erase();

}

CCSVParser::~CCSVParser()
{

	delete [] m_ptrFileContent;

}
int CCSVParser::Parse(string strFileName , vector<vector<string> > &vRow)
{

	CFile file;

	vector<string> vColumn;

	//AfxMessageBox("Opening file");

	file.Open(strFileName.c_str(),CFile::modeRead);

	//AfxMessageBox("file opened");

	if(file.m_hFile == CFile::hFileNull)
		return -1;

	unsigned long nFileLen = file.GetLength();

	if(nFileLen <=0 )
		return -1;

	m_ptrFileContent = new char[nFileLen+1];

	if(!m_ptrFileContent )
		return -1;

	file.Read(m_ptrFileContent,nFileLen);

	string strData = m_ptrFileContent ;

	char *ptrTemp = m_ptrFileContent ;

	string sWord;

	//int nLoop = 0;
	BOOL bNewLine = TRUE;
//	int i =0 , j = 0;

	//AfxMessageBox("Start Parse logic");
 
	try
	{
		while(ptrTemp != NULL )
		{
			if( (*ptrTemp) < 0)
				break;
			if( (*ptrTemp) == '"' )
			{			
				sWord.erase();
				while(ptrTemp != NULL )
				{				
					ptrTemp++;
					if( (*ptrTemp) == '"' )
					{
						while(ptrTemp != NULL )
						{
							ptrTemp++;
							if( (*ptrTemp) == '"' )
							{
								bNewLine = FALSE;
								break;
							}
							if( (*ptrTemp) == '\r' )
							{
								bNewLine = TRUE;
								break;
							}	
						}
						break;
					}
					sWord += (*ptrTemp); 			
				}	
				vColumn.push_back( sWord );			
				if(bNewLine == TRUE)
				{
					vRow.push_back( vColumn );
					vColumn.erase(vColumn.begin(),vColumn.end());
				}
			}		
			if(bNewLine == FALSE)
			{
				continue;
			}		
			ptrTemp++;	
		}
	}
	catch( ... )
	{
		//AfxMessageBox( "Exception Thrown");

	}
	//AfxMessageBox("end Parse logic");
	return 0;
}

