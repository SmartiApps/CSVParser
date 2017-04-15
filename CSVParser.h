// CSVParser.h: interface for the CCSVParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSVPARSER_H__A24CC3F0_C34E_47EF_AF83_BABCF9928825__INCLUDED_)
#define AFX_CSVPARSER_H__A24CC3F0_C34E_47EF_AF83_BABCF9928825__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <vector>
#include <string>

using namespace std;


class CCSVParser  
{
public:
	CCSVParser();
	virtual ~CCSVParser();

	int Parse(string strFileName , vector<vector<string> > &vData);

private:
	
	string m_strFileName ;
	char *m_ptrFileContent;

	


};

#endif // !defined(AFX_CSVPARSER_H__A24CC3F0_C34E_47EF_AF83_BABCF9928825__INCLUDED_)
