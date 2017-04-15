// CSVParsingDlg.h : header file
//

#if !defined(AFX_CSVPARSINGDLG_H__47363E2F_FE67_4812_8EC7_D71220747F4C__INCLUDED_)
#define AFX_CSVPARSINGDLG_H__47363E2F_FE67_4812_8EC7_D71220747F4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCSVParsingDlg dialog

#include "CSVParser.h"

class CCSVParsingDlg : public CDialog
{
// Construction
public:
	CCSVParsingDlg(CWnd* pParent = NULL);	// standard constructor

CCSVParser objParser;

// Dialog Data
	//{{AFX_DATA(CCSVParsingDlg)
	enum { IDD = IDD_CSVPARSING_DIALOG };
	CStatic	m_stTimer;
	CListCtrlEx	m_ltGrid;
	//CListCtrl m_ltGrid;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCSVParsingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCSVParsingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSVPARSINGDLG_H__47363E2F_FE67_4812_8EC7_D71220747F4C__INCLUDED_)
