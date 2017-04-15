// ------------------------------------------------------------------------------------------
//
// Copyright © 1999 - 2008 eGrabber Inc. All rights reserved
//
// -------------------------------------------------------------------------------------------

////////////////////
// Header
#if !defined(AFX_LISTCTRLEX_H)
#define AFX_LISTCTRLEX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlEx.h : header file 
//
#pragma warning( disable : 4786 )
#pragma warning( disable : 4311 )
#pragma warning( disable : 4312 )

#include <vector>
#include <string>
#include <map>
#include <afxtempl.h>
#include "InPlaceCombo.h"
#include "InPlaceEdit.h"

#define FIRST_COLUMN				0
#define MIN_COLUMN_WIDTH			10
#define MAX_DROP_DOWN_ITEM_COUNT	50

class CInPlaceCombo;
class CInPlaceEdit;

// User define message 
// This message is posted to the parent
// The message can be handled to make the necessary validations, if any
#define WM_VALIDATE		WM_USER + 0x7FFD

// User define message 
// This message is posted to the parent
// The message should be handled to spcify the items to the added to the combo
#define WM_SET_ITEMS			WM_USER + 0x7FFC
#define WM_FIND_GRID			WM_USER + 0x7FFE
#define WM_FIND_ROW_COL			WM_USER + 0x7FFB
#define WM_DELETE_ROW_NOTIFY	WM_USER + 0x7FFA

using namespace std;
///////////////////////////////////////////////////////////
// CListCtrlEx window

class CListCtrlEx : public CListCtrl
{
	// Construction
public:
	CListCtrlEx();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEx)
	//}}AFX_VIRTUAL
	// Implementation
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int SetItemText( int nRow , int nCol , LPCTSTR lpcText );
	virtual BOOL DeleteAllItems();
	virtual int InsertColumn(int nCol , LPCTSTR lpcText , int nFormat , int nWidth , int nSubItem);
	virtual BOOL DeleteColumn(int nCol);
	virtual int InsertItem( int nRow , LPCTSTR lpcText );
	virtual int GetItemCount();
	virtual BOOL GetCheck( int nItem );
	virtual BOOL SetCheck( int nRow , BOOL fCheck = 1 );
	virtual BOOL DeleteItem( int nRow );

	virtual void SortAscending( int iIndex );
	virtual void SortDescending( int iIndex );

	int CheckAll();
	int UnCheckAll();

	int CopyToClipboard();
	int PasteFromClipboard( BOOL bInsert = FALSE );
	int PasteFromClipboardAsNewCol();

	int FindNext( CString csText , BOOL bSubString = TRUE  , BOOL bCaseSensitive = FALSE);
	int FindPrev( CString csText , BOOL bSubString = TRUE , BOOL bCaseSensitive = FALSE );
	int FillFindMap( CString csText , BOOL bSubString = TRUE , BOOL bCaseSensitive = FALSE );

	int DeleteSelected();

	int ExportGridToCSV( CString &csSavePath , BOOL bExportSel = FALSE );
	BOOL SelectAll();
	BOOL DeselectAll();

	int GetColumnHeader( int nCol , CString &csHeader );
	int GetAllColumns( CStringArray &csaArray );

	void ClearGrid();

	CString  GetItemTextEx( int nRow , int nCol );
	
	size_t GetRecordCount();
	
	int GetCheckedMap( std::map<LONG,LONG> &cmCheckedTrackMap)
	{
		cmCheckedTrackMap = m_cmCheckedTrackMap;
		return 1;
	}
	int GetCheckedCount()
	{
		return m_cmCheckedTrackMap.size(); 
	}

	int GetColumnIndex( LPCTSTR lpcColumnName , BOOL bSubstr = FALSE);
	int CheckRecord( LPCTSTR lpcColumn , LPCTSTR lpcContent );

	CString GetLastDel(){ return csLastDel;}
	virtual ~CListCtrlEx();

	//Holding Combo values
	map< int , vector < CString > >m_HeaderToComboMap;

	// Sets/Resets the column which support the in place combo box
	void SetComboColumns(int iColumnIndex, bool bSet = true);
	
	// Sets/Resets the column which support the in place edit control
	void SetReadOnlyColumns(int iColumnIndex, bool bSet = true);

	// Sets the valid characters for the edit ctrl
	void SetValidEditCtrlCharacters(CString& rstrValidCharacters);

	// Sets the vertical scroll
	void EnableVScroll(bool bEnable = true);

	// Sets the horizontal scroll
	void EnableHScroll(bool bEnable = true);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	int m_nCurrentCol;
	int m_nCurrentRow;

	void EnableSorting( BOOL bEnable = TRUE )
	{
		m_bEnableSort = bEnable;
	}

	BOOL IsCopyEnabled(){ return m_bEnableCopy; }
	BOOL IsPasteEnabled(){ return m_bEnablePaste; }

	void EnableCopy( BOOL bEnable = TRUE )
	{
		m_bEnableCopy = bEnable;
	}
	void EnablePaste( BOOL bEnable = TRUE )
	{
		m_bEnablePaste = bEnable;
	}
	void EnableFreeze( BOOL bEnable = TRUE )
	{
		m_bEnableFreezeCol = bEnable;
	}

	void ClearFindMap()
	{
		m_FindMap.clear();
	}
	LONG GetFindCount()
	{
		return m_FindMap.size();
	}
	void SetFreezeCol( int nCol = 0 )
	{
		m_nFreezeCol = nCol;
	}
	int GetFreezeCol()
	{
		return m_nFreezeCol;
	}

private:
	
	int m_nFreezeCol;
	int m_FirstVisibleCol;//Freeze column

	int m_PrevIndex;
	BOOL m_bAscSort;

	BOOL m_bEnableSort;
	BOOL m_bEnableCopy;
	BOOL m_bEnablePaste;
	BOOL m_bEnableFreezeCol;

	int nMaxCol;
	std::vector<LPCTSTR> m_vRowData;
	std::vector< std::vector<std::string>  > m_RowCollection;// Data Structure used to store data
	std::vector<BOOL> m_vCheckTracker;
	std::vector<BOOL>::iterator m_vCheckItr;
	std::vector< std::vector<std::string>  >::iterator m_CollectionItr;
	
	CString csLastDel;

	BOOL IsCheckBoxesVisible();
	void ToggleCheckBox(int item);

	void SpiltWord( LPTSTR buffer , std::vector<std::string> &vStr ); 

	void SetValues( std::vector<std::string>&vStr , UINT uiItem , BOOL bInsert = FALSE);
	void SetValues( std::vector<std::string>&vStr , UINT uiItem , UINT uiCol ,BOOL bInsert = FALSE);

	std::map<LONG ,LONG> m_cmCheckedTrackMap;
	std::map<LONG,LONG>::iterator ItrCheckedTrackMap;

	std::map<CString ,BOOL> m_SortOrderMap;
	std::map<CString,BOOL>::iterator m_SortOrderMapItr;

	CString m_csPrevFind;
	multimap<LONG,LONG> m_FindMap;
	multimap<LONG,LONG>::iterator m_itrFind;
	//multimap<LONG,LONG>::iterator m_itrFind;

	// Returns the row & column index of the column on which mouse click event has occured
	bool HitTestEx(CPoint& rHitPoint, int* pRowIndex, int* pColumnIndex) const;

	// Creates and displays the in place combo box
	CInPlaceCombo* ShowInPlaceList(int iRowIndex, int iColumnIndex, CStringList& rComboItemsList, 
								   CString strCurSelecetion = "", int iSel = -1);

	// Creates and displays the in place edit control
	CInPlaceEdit* ShowInPlaceEdit(int iRowIndex, int iColumnIndex, CString& rstrCurSelection);

	// Calculates the cell rect
	void CalculateCellRect(int iColumnIndex, int iRowIndex, CRect& robCellRect);

	// Checks whether column supports in place combo box
	bool IsCombo(int iColumnIndex);

	// Checks whether column is read only
	bool IsReadOnly(int iColumnIndex);

	// Scrolls the list ctrl to bring the in place ctrl to the view
	void ScrollToView(int iColumnIndex, /*int iOffSet, */CRect& obCellRect);

// Attributes
	
	// List of columns that support the in place combo box
	CList<int, int> m_ComboSupportColumnsList;

	// List of columns that are read only
	CList<int, int> m_ReadOnlyColumnsList;

	// Valid characters
	CString m_strValidEditCtrlChars;

	// The window style of the in place edit ctrl
	DWORD m_dwEditCtrlStyle;

	// The window style of the in place combo ctrl
	DWORD m_dwDropDownCtrlStyle;

	
	// Generated message map functions
protected:
	
	DECLARE_MESSAGE_MAP()

public:
	
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnOdfinditem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult);
};


//////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLEX_H)

