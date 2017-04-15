// ------------------------------------------------------------------------------------------
//
// Copyright © 1999 - 2008 eGrabber Inc. All rights reserved
//
// -------------------------------------------------------------------------------------------

//////////////////////////////////////////////
// *.cpp
// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////
// CListCtrlEx

CListCtrlEx::CListCtrlEx()
{
	nMaxCol = 0;
	m_RowCollection.clear();
	m_vCheckTracker.clear();
	m_HeaderToComboMap.clear();
	m_ComboSupportColumnsList.RemoveAll();
	m_ReadOnlyColumnsList.RemoveAll();
	m_strValidEditCtrlChars.Empty();
	m_dwEditCtrlStyle = ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_NOHIDESEL;
	m_dwDropDownCtrlStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | 
							CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;

	m_bAscSort = TRUE;
	m_PrevIndex = -1;
	m_bEnableSort = FALSE;
	m_bEnableCopy = TRUE;
	m_bEnablePaste = FALSE;
	m_bEnableFreezeCol = FALSE;
	
	m_FirstVisibleCol = 0;
	m_nFreezeCol = 0;

	
}

CListCtrlEx::~CListCtrlEx()
{
	CInPlaceCombo::DeleteInstance();
	CInPlaceEdit::DeleteInstance();
}

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlEx)
	//}}AFX_MSG_MAP
	
	
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClickList)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydownList)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)


	ON_NOTIFY(HDN_ITEMCLICKA, 0, OnHdnItemclick)
	ON_NOTIFY(HDN_ITEMCLICKW, 0, OnHdnItemclick)
END_MESSAGE_MAP()

size_t CListCtrlEx::GetRecordCount()
{
	return m_RowCollection.size();
	
}
int CListCtrlEx::SetItemText(int nRow , int nCol , LPCTSTR lpcText)
{
/*	if(GetRecordCount() > 0)
	{
			*/
		if ( nCol >= m_RowCollection[nRow].size())
		{
			m_RowCollection[nRow].push_back( lpcText );
			return 1;
		}
		m_RowCollection[nRow][nCol] = lpcText;
		return 1;
/*	}
	else
	{
		//insert row and then add
		InsertItem(nRow, "");
		int nSize = m_RowCollection[nRow].size();
		if ( nCol >= m_RowCollection[nRow].size())
		{
			m_RowCollection[nRow].push_back( lpcText );
			return 1;
		}
	}
	return 1;			*/
}
int CListCtrlEx::InsertColumn(int nCol, LPCTSTR lpcText, int nFormat, int nWidth, int nSubItem)
{
	nMaxCol++;
	for ( m_CollectionItr = m_RowCollection.begin() ; m_CollectionItr != m_RowCollection.end(); m_CollectionItr++ )
	{
		m_CollectionItr->push_back("");
	}
	return (CListCtrl::InsertColumn( nCol , lpcText , nFormat , nWidth , nSubItem));
}
BOOL CListCtrlEx::DeleteAllItems()
{
	m_RowCollection.clear(); 
	m_vCheckTracker.clear();
	m_vRowData.clear();
	m_cmCheckedTrackMap.clear();
	m_HeaderToComboMap.clear();
	m_SortOrderMap.clear();
	return ( CListCtrl::DeleteAllItems());
}
BOOL CListCtrlEx::DeleteColumn(int nCol)
{
	if ( nMaxCol > 0 )
		nMaxCol--;
	m_HeaderToComboMap.erase( nCol );

	return( CListCtrl::DeleteColumn(nCol));
}

int CListCtrlEx::InsertItem( int nRow , LPCTSTR lpcText)
{
	if ( nRow > m_RowCollection.size() )
	{
		nRow = m_RowCollection.size();
	}
	std::vector<std::string> vDummyRow;
	vDummyRow.push_back( lpcText );
	for ( int nIndex = 1 ; nIndex < nMaxCol ; nIndex++ )
	{
		vDummyRow.push_back( "" ); 
	}
	m_RowCollection.insert( (m_RowCollection.begin() + nRow ) , vDummyRow );
	m_vCheckTracker.push_back( FALSE );
	return 1;
}
int CListCtrlEx::GetItemCount()
{
	return m_RowCollection.size();
}
BOOL CListCtrlEx::GetCheck( int nRow )
{
	if ( m_vCheckTracker.size() > nRow)
		return m_vCheckTracker[nRow];
	else
		return FALSE;
}
BOOL CListCtrlEx::SetCheck(int nRow, BOOL fCheck )
{
	if ( fCheck )
	{
		m_vCheckTracker[nRow] = fCheck;
		m_cmCheckedTrackMap[nRow] = nRow;
		
	}
	else
	{	
		m_vCheckTracker[nRow] = fCheck;
		ItrCheckedTrackMap = m_cmCheckedTrackMap.find(nRow);
		if ( ItrCheckedTrackMap!=m_cmCheckedTrackMap.end())
			m_cmCheckedTrackMap.erase( ItrCheckedTrackMap );
	}
	return TRUE;
}
int CListCtrlEx::GetColumnIndex( LPCTSTR lpcColumnName , BOOL bSubstr )
{
	CHeaderCtrl *pHdr = GetHeaderCtrl();
	int nMaxCol =  pHdr->GetItemCount();
	if ( nMaxCol < 1 )
		return -1;

	CString csHeader;
	LPSTR pszColHeader; 
	LV_COLUMN pColumn;
	char buffer[100];

	int nSourceFile = -1;

	for ( int nCol = 0; nCol < nMaxCol ; nCol++ )
	{
		memset(&pColumn,0,sizeof(pColumn));
		pColumn.mask = LVCF_TEXT;
		pColumn.cchTextMax =100;
		pColumn.pszText = buffer;
		GetColumn(nCol, &pColumn);
		pszColHeader = pColumn.pszText;
		csHeader = pszColHeader;
		if ( bSubstr )
		{
			if ( csHeader.Find( lpcColumnName ) == 0 )
			{
				nSourceFile = nCol;
				return nSourceFile;
			}
		}
		else
		{
			if ( csHeader.CompareNoCase( lpcColumnName ) == 0 )
			{
				nSourceFile = nCol;
				return nSourceFile;
			}
		}
	}
	return nSourceFile;

}

//Checks the row based on condition
int CListCtrlEx::CheckRecord( LPCTSTR lpcColumn , LPCTSTR lpcContent )
{
	int nHdrInx = GetColumnIndex( lpcColumn );
	
	if ( nHdrInx == -1 )
		return -1;

	LONG lMaxRow = GetItemCount();
	for ( LONG lRowIdx = 0 ; lRowIdx < lMaxRow ; lRowIdx++ )
	{
		if ( GetItemTextEx( lRowIdx , nHdrInx ).CompareNoCase( lpcContent ) == 0 )
			SetCheck( lRowIdx );
	}
	return 1;
}

BOOL CListCtrlEx::DeleteItem( int nRow )
{
	ItrCheckedTrackMap = m_cmCheckedTrackMap.find(nRow);
	if ( ItrCheckedTrackMap!=m_cmCheckedTrackMap.end())
		m_cmCheckedTrackMap.erase( ItrCheckedTrackMap );
	
	m_CollectionItr = m_RowCollection.begin();
	m_CollectionItr += nRow;
	m_vCheckItr = m_vCheckTracker.begin();
	m_vCheckItr += nRow;


	if ( m_CollectionItr != m_RowCollection.end())
	{
		if ( !m_CollectionItr->empty() )
		{
			csLastDel = m_CollectionItr->at(0).c_str();
			GetParent()->SendMessage( WM_DELETE_ROW_NOTIFY , (WPARAM)atol(csLastDel) );
			m_RowCollection.erase( m_CollectionItr );
			m_vCheckTracker.erase( m_vCheckItr );
		}
	}
	else
		return FALSE;

 	return CListCtrl::DeleteItem( nRow );

}
CString  CListCtrlEx::GetItemTextEx( int nRow , int nCol )
{
	CString csText;
	if ( ( m_RowCollection.size() > nRow ) && ( m_RowCollection[nRow].size() > nCol ) )
		csText = m_RowCollection[nRow][nCol].c_str();
	return csText;
}
void CListCtrlEx::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	LV_ITEM &Item = pDispInfo->item;

	//Create a pointer to the item
	LV_ITEM* pItem= &(pDispInfo)->item;

	//Which item number?
	int itemid = pItem->iItem;

	int iItemIndx = Item.iItem;
	UINT nColIndex = Item.iSubItem;

	if ( m_RowCollection.size() <= 0 )
	{
		*pResult = 0;
		return;
	}
	if(Item.mask & LVIF_TEXT)
	{
		vector<std::string>vTemp;
		string strTemp;
		if ( iItemIndx >= m_RowCollection.size() )
		{
			*pResult = 0;
			return;
		}
		vTemp = m_RowCollection[iItemIndx];

		if ( vTemp.size() <= 0 )
			strTemp = "";
		else if ( nColIndex >= vTemp.size() )
		{
			*pResult = 0;
			return;
		}
		else
			strTemp = vTemp[nColIndex];


		// YourArray contains the text data
		// make sure that your data is not longer than Item.cchTextMax
		//strcpy_s(Item.pszText , strTemp.length() + 1 , strTemp.c_str());
		strcpy( Item.pszText , strTemp.c_str() );
	}

	if( pItem->mask & LVIF_IMAGE) 
	{
		//Set which image to use
		pItem->iImage=m_vCheckTracker[itemid];

		//Show check box?
		if(IsCheckBoxesVisible())
		{
			//To enable check box, we have to enable state mask...
			pItem->mask |= LVIF_STATE;
			pItem->stateMask = LVIS_STATEIMAGEMASK;

			if(m_vCheckTracker[itemid])
			{
				//Turn check box on..
				pItem->state = INDEXTOSTATEIMAGEMASK(2);
			}
			else
			{
				//Turn check box off
				pItem->state = INDEXTOSTATEIMAGEMASK(1);
			}
		}
	}

	*pResult = 0;
}

//This is called when user clicks on the list. 
//If you had check box you have to write code to make them possible to toggle.
void CListCtrlEx::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	LVHITTESTINFO hitinfo;
	//Copy click point
	hitinfo.pt = pNMListView->ptAction;
	CPoint dcpoint( pNMListView->ptAction);
	int m_nLastRow;
	int m_nLastCol;

	HitTestEx( dcpoint , &m_nLastRow , &m_nLastCol );
	//Make the hit test...
	int item = CListCtrl::HitTest(&hitinfo); 
	m_nCurrentRow = item;
	m_nCurrentCol = pNMListView->iSubItem ; 

	if(item != -1 && m_nCurrentCol == 0)
	{
		//We hit one item... did we hit state image (check box)?
		//This test only works if we are in list or report mode.
		if( (hitinfo.flags & LVHT_ONITEMSTATEICON) != 0)
		{
			ToggleCheckBox(item);
		}
	}

	////////////////////////////////////////////////////////////////
	// If column is not read only then
	// If the SHIFT or CTRL key is down call the base class
	// Check the high bit of GetKeyState to determine whether SHIFT or CTRL key is down
	int iColumnIndex = pNMListView->iSubItem;
	int iRowIndex = item;
	if ((GetKeyState(VK_SHIFT) & 0x80) || (GetKeyState(VK_CONTROL) & 0x80))
	{
		return;
	}

	// Get the current selection before creating the in place combo box
	CString strCurSelection = GetItemTextEx(iRowIndex, iColumnIndex);

	if (-1 != iRowIndex)
	{
		UINT flag = LVIS_FOCUSED;
		
		if ((GetItemState(iRowIndex, flag ) & flag) == flag)
		{
			// Add check for LVS_EDITLABELS
			if (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
			{
				// If combo box is supported
				// Create and show the in place combo box
				if (IsCombo(iColumnIndex))
				{
					CStringList obComboItemsList;
					GetParent()->SendMessage(WM_SET_ITEMS, (WPARAM)iColumnIndex, (LPARAM)&obComboItemsList);  
					
				//	CInPlaceCombo* pInPlaceComboBox = ShowInPlaceList(iRowIndex, iColumnIndex, obComboItemsList, strCurSelection);
					CInPlaceCombo* pInPlaceComboBox = ShowInPlaceList(m_nLastRow, m_nLastCol, obComboItemsList, strCurSelection);
					ASSERT(pInPlaceComboBox); 
					
					// Set the selection to previous selection
					pInPlaceComboBox->SelectString(-1, strCurSelection);
				}
				// If combo box is not read only
				// Create and show the in place edit control
				else if (!IsReadOnly(iColumnIndex))
				{
					//CInPlaceEdit* pInPlaceEdit = ShowInPlaceEdit(iRowIndex, iColumnIndex, strCurSelection);
					CInPlaceEdit* pInPlaceEdit = ShowInPlaceEdit(m_nLastRow, m_nLastCol, strCurSelection);
				}
			}
		}
	}

	*pResult = 0;
}
void CListCtrlEx::OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;

	//If user press space, toggle flag on selected item
	if( pLVKeyDown->wVKey == VK_SPACE )
	{
		//Check if check boxes are visible
		if( IsCheckBoxesVisible() )
		{
			//Toggle if some item is selected
			if( CListCtrl::GetSelectionMark() != -1)
				ToggleCheckBox(  CListCtrl::GetSelectionMark() );
		}
	}

	*pResult = 0;
}
BOOL CListCtrlEx::IsCheckBoxesVisible()
{
	DWORD style =  CListCtrl::GetStyle();

	if( ((style & LVS_TYPEMASK) == LVS_LIST) ||
		((style & LVS_TYPEMASK) == LVS_REPORT) )
		return TRUE;

	return FALSE;
}
void CListCtrlEx::ToggleCheckBox(int item)
{
	//Change check box
	if ( item < m_vCheckTracker.size() )
		m_vCheckTracker[item] = !m_vCheckTracker[item];
	else
		CListCtrl::RedrawItems(item, item);

	if ( m_vCheckTracker[item] == TRUE)
		m_cmCheckedTrackMap[ item ]= item ;
	else
	{	
		ItrCheckedTrackMap = m_cmCheckedTrackMap.find(item);
		if ( ItrCheckedTrackMap!=m_cmCheckedTrackMap.end())
			m_cmCheckedTrackMap.erase( ItrCheckedTrackMap );
	}

	//And redraw
	RedrawItems(item, item);
}

CInPlaceCombo* CListCtrlEx::ShowInPlaceList(int iRowIndex, int iColumnIndex, CStringList& rComboItemsList, 
											   CString strCurSelecetion /*= ""*/, int iSel /*= -1*/)
{
	// The returned obPointer should not be saved
	
	// Make sure that the item is visible
	if (!EnsureVisible(iRowIndex, TRUE))
	{
		return NULL;
	}

	// Make sure that iColumnIndex is valid 
	CHeaderCtrl* pHeader = static_cast<CHeaderCtrl*> (GetDlgItem(FIRST_COLUMN));

	int iColumnCount = pHeader->GetItemCount();

	if (iColumnIndex >= iColumnCount || GetColumnWidth(iColumnIndex) < MIN_COLUMN_WIDTH) 
	{
		return NULL;
	}

	// Calculate the rectangle specifications for the combo box
	CRect obCellRect(0, 0, 0, 0);
	CalculateCellRect(iColumnIndex, iRowIndex, obCellRect);

	int iHeight = obCellRect.Height();  
	int iCount = rComboItemsList.GetCount();

	iCount = (iCount < MAX_DROP_DOWN_ITEM_COUNT) ? 
		iCount + MAX_DROP_DOWN_ITEM_COUNT : (MAX_DROP_DOWN_ITEM_COUNT + 1); 

	obCellRect.bottom += iHeight * iCount; 

	// Create the in place combobox
	CInPlaceCombo* pInPlaceCombo = CInPlaceCombo::GetInstance();
	pInPlaceCombo->ShowComboCtrl(m_dwDropDownCtrlStyle, obCellRect, this, 0, iRowIndex, iColumnIndex, &rComboItemsList, 
								 strCurSelecetion, iSel);
	
	return pInPlaceCombo;
}

CInPlaceEdit* CListCtrlEx::ShowInPlaceEdit(int iRowIndex, int iColumnIndex, CString& rstrCurSelection)
{
	// Create an in-place edit control
	CInPlaceEdit* pInPlaceEdit = CInPlaceEdit::GetInstance();
		
	CRect obCellRect(0, 0, 0, 0);
	CalculateCellRect(iColumnIndex, iRowIndex, obCellRect);
			
	pInPlaceEdit->ShowEditCtrl(m_dwEditCtrlStyle, obCellRect, this, 0, 
							   iRowIndex, iColumnIndex,
							   m_strValidEditCtrlChars, rstrCurSelection);

	return pInPlaceEdit;
}

void CListCtrlEx::OnHScroll(UINT iSBCode, UINT iPos, CScrollBar* pScrollBar) 
{
	if ( !m_bEnableFreezeCol )
	{
		CListCtrl::OnHScroll(iSBCode, iPos, pScrollBar);
		return;
	}
	if( GetFocus() != this ) SetFocus();


	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	CRect rcScroll;
	GetClientRect(&rcScroll);
	rcScroll.right = GetColumnWidth(0);
	rcScroll.left = 0;
	BOOL bUpdate = FALSE;
	int minpos;
	int maxpos;
	GetScrollRange(SB_HORZ, &minpos, &maxpos);
	maxpos = GetScrollLimit(SB_HORZ);

	// Get the current position of scroll box.
	int curpos = GetScrollPos(SB_HORZ);

	switch (iSBCode)
	{
	case SB_LEFT: // Scroll to far left.
		break;

	case SB_RIGHT: // Scroll to far right.

		break;

	case SB_ENDSCROLL: // End scroll.
		break;

	case SB_LINELEFT: // Scroll left.
		if (curpos > minpos)
		{
			m_FirstVisibleCol--;
			LVCOLUMN m_Column1, m_Column2;
			char Text1[20], Text2[20];
			// Make size large enough to hold your column header text.
			m_Column1.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
			m_Column1.pszText = Text1;
			m_Column1.cchTextMax = sizeof(Text1);
			GetColumn(m_nFreezeCol, &m_Column1);//Rajith
			m_FirstVisibleCol = Header_OrderToIndex( pHeader->m_hWnd, m_Column1.iOrder-1);
			m_Column2.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
			m_Column2.pszText = Text2;
			m_Column2.cchTextMax = sizeof(Text2);
			GetColumn(m_FirstVisibleCol, &m_Column2);
			int order = m_Column2.iOrder;
			m_Column2.iOrder = m_Column1.iOrder;
			SetColumn(m_FirstVisibleCol, &m_Column2);
			m_Column1.iOrder = order;
			SetColumn(m_nFreezeCol, &m_Column1);//rajith
			bUpdate = TRUE;
		}
		break;

	case SB_LINERIGHT: // Scroll right.
		if (curpos < maxpos)
		{
			LVCOLUMN m_Column1, m_Column2;
			char Text1[50], Text2[50];
			// Make size large enough to hold your column header text.
			m_Column1.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
			m_Column1.pszText = Text1;
			m_Column1.cchTextMax = sizeof(Text1);
			GetColumn(m_nFreezeCol, &m_Column1);//rajith

			m_FirstVisibleCol = Header_OrderToIndex( pHeader->m_hWnd, m_Column1.iOrder+1);

			m_Column2.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
			m_Column2.pszText = Text2;
			m_Column2.cchTextMax = sizeof(Text2);
			GetColumn(m_FirstVisibleCol, &m_Column2);
			int order = m_Column2.iOrder;
			m_Column2.iOrder = m_Column1.iOrder;
			SetColumn(m_FirstVisibleCol, &m_Column2);
			m_Column1.iOrder = order;
			SetColumn(m_nFreezeCol, &m_Column1);//rajith
			bUpdate = TRUE;
		}
		break;

	case SB_PAGELEFT: // Scroll one page left.
		break;

	case SB_PAGERIGHT: // Scroll one page right.
		break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		break;

	case SB_THUMBTRACK: // Drag scroll box to specified position. nPos is
		break;
	}

	int iShift = GetColumnWidth(m_FirstVisibleCol);
	switch (iSBCode)
	{
	case SB_LEFT: // Scroll to far left.
		Scroll(CSize(iShift, 0));
		break;

	case SB_RIGHT: // Scroll to far right.
		Scroll(CSize(-iShift, 0));
		break;

	case SB_ENDSCROLL: // End scroll.
		break;

	case SB_LINELEFT: // Scroll left.
		if (curpos > minpos)
		{
			iShift = GetColumnWidth(m_FirstVisibleCol);
			Scroll(CSize(-iShift, 0));
			rcScroll.right += GetColumnWidth(m_FirstVisibleCol);
			rcScroll.left += GetColumnWidth(0);

			//SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ)-iShift);
		}
		break;

	case SB_LINERIGHT: // Scroll right.
		if (curpos < maxpos)
		{
			iShift = GetColumnWidth(m_FirstVisibleCol);
			m_FirstVisibleCol++;
			if(curpos+iShift > maxpos)
				SetColumnWidth(19, GetColumnWidth(19)+ ( curpos + iShift - maxpos));
			Scroll(CSize(iShift, 0));
			//SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ)+iShift);
		}
		break;

	case SB_PAGELEFT: // Scroll one page left.
		break;

	case SB_PAGERIGHT: // Scroll one page right.
		break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		break;

	case SB_THUMBTRACK: // Drag scroll box to specified position. nPos is
		break;
	}

	if(bUpdate)
		InvalidateRect(NULL, TRUE);

}

void CListCtrlEx::OnVScroll(UINT iSBCode, UINT iPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	if (GetFocus() != this)
	{
		SetFocus();
	}

	CListCtrl::OnVScroll(iSBCode, iPos, pScrollBar);
}

/*void CListCtrlEx::OnLButtonDown(UINT iFlags, CPoint obPoint) 
{
	if (!HitTestEx(obPoint, &m_nLastRow, &m_nLastCol))
	{
		return;
	}
	// TODO: Add your message handler code here and/or call default


	// Get the current column and row
	CHeaderCtrl *pHdr = GetHeaderCtrl();
	int nMax = pHdr->GetItemCount();

	for ( int iColIx = 0 ; iColIx < nMax ; iColIx++ )
	{
		if ( iColIx < m_FirstVisibleCol )
			SetReadOnlyColumns( iColIx , TRUE );
		else
			SetReadOnlyColumns( iColIx , FALSE );

	}
	if (!HitTestEx(obPoint, &iRowIndex, &iColumnIndex))
	{
		return;
	}
	
	CListCtrl::OnLButtonDown(iFlags, obPoint);
	
	// If column is not read only then
	// If the SHIFT or CTRL key is down call the base class
	// Check the high bit of GetKeyState to determine whether SHIFT or CTRL key is down
	if ((GetKeyState(VK_SHIFT) & 0x80) || (GetKeyState(VK_CONTROL) & 0x80))
	{
		return;
	}

	// Get the current selection before creating the in place combo box
	CString strCurSelection = GetItemTextEx(iRowIndex, iColumnIndex);
	
	if (-1 != iRowIndex)
	{
		UINT flag = LVIS_FOCUSED;
		
		if ((GetItemState(iRowIndex, flag ) & flag) == flag)
		{
			// Add check for LVS_EDITLABELS
			if (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
			{
				// If combo box is supported
				// Create and show the in place combo box
				if (IsCombo(iColumnIndex))
				{
					CStringList obComboItemsList;
					GetParent()->SendMessage(WM_SET_ITEMS, (WPARAM)iColumnIndex, (LPARAM)&obComboItemsList);  
					
					CInPlaceCombo* pInPlaceComboBox = ShowInPlaceList(iRowIndex, iColumnIndex, obComboItemsList, strCurSelection);
					ASSERT(pInPlaceComboBox); 
					
					// Set the selection to previous selection
					pInPlaceComboBox->SelectString(-1, strCurSelection);
				}
				// If combo box is not read only
				// Create and show the in place edit control
				else if (!IsReadOnly(iColumnIndex))
				{
					CInPlaceEdit* pInPlaceEdit = ShowInPlaceEdit(iRowIndex, iColumnIndex, strCurSelection);
				}
			}
		}
	}  
}*/

bool CListCtrlEx::HitTestEx(CPoint &obPoint, int* pRowIndex, int* pColumnIndex) const
{
	if (!pRowIndex || !pColumnIndex)
	{
		return false;
	}

	// Get the row index
	*pRowIndex = HitTest(obPoint, NULL);

	if (pColumnIndex)
	{
		*pColumnIndex = 0;
	}

	// Make sure that the ListView is in LVS_REPORT
	if ((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
	{
		return false;
	}

	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);

	int iColumnCount = pHeader->GetItemCount();

	// Get bounding rect of item and check whether obPoint falls in it.
	CRect obCellRect;
	GetItemRect(*pRowIndex, &obCellRect, LVIR_BOUNDS);
	
	if (obCellRect.PtInRect(obPoint))
	{
		// Now find the column
		for (*pColumnIndex = 0; *pColumnIndex < iColumnCount; (*pColumnIndex)++)
		{
			int iColWidth = GetColumnWidth(*pColumnIndex);
			
			if (obPoint.x >= obCellRect.left && obPoint.x <= (obCellRect.left + iColWidth))
			{
				return true;
			}
			obCellRect.left += iColWidth;
		}
	}
	return false;
}

void CListCtrlEx::SetComboColumns(int iColumnIndex, bool bSet /*= true*/)
{
	// If the Column Index is not present && Set flag is false
	// Then do nothing 
	// If the Column Index is present && Set flag is true
	// Then do nothing
	POSITION Pos = m_ComboSupportColumnsList.Find(iColumnIndex);

	// If the Column Index is not present && Set flag is true
	// Then Add to list
	if ((NULL == Pos) && bSet) 
	{
		m_ComboSupportColumnsList.AddTail(iColumnIndex); 
	}

	// If the Column Index is present && Set flag is false
	// Then Remove from list
	if ((NULL != Pos) && !bSet) 
	{
		m_ComboSupportColumnsList.RemoveAt(Pos); 
	}
}

void CListCtrlEx::SetReadOnlyColumns(int iColumnIndex, bool bSet /*= true*/)
{
	// If the Column Index is not present && Set flag is false
	// Then do nothing 
	// If the Column Index is present && Set flag is true
	// Then do nothing
	POSITION Pos = m_ReadOnlyColumnsList.Find(iColumnIndex);

	// If the Column Index is not present && Set flag is true
	// Then Add to list
	if ((NULL == Pos) && bSet) 
	{
		m_ReadOnlyColumnsList.AddTail(iColumnIndex); 
	}

	// If the Column Index is present && Set flag is false
	// Then Remove from list
	if ((NULL != Pos) && !bSet) 
	{
		m_ReadOnlyColumnsList.RemoveAt(Pos); 
	}
}

bool CListCtrlEx::IsReadOnly(int iColumnIndex)
{
	if (m_ReadOnlyColumnsList.Find(iColumnIndex))
	{
		return true;
	}
	
	return false;
}

bool CListCtrlEx::IsCombo(int iColumnIndex)
{
	if (m_ComboSupportColumnsList.Find(iColumnIndex))
	{
		return true;
	}

	return false;
}

void CListCtrlEx::CalculateCellRect(int iColumnIndex, int iRowIndex, CRect& robCellRect)
{
	GetItemRect(iRowIndex, &robCellRect, LVIR_BOUNDS);
	
	CRect rcClient;
	GetClientRect(&rcClient);

	if (robCellRect.right > rcClient.right) 
	{
		robCellRect.right = rcClient.right;
	}

	ScrollToView(iColumnIndex, robCellRect); 
}

void CListCtrlEx::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	// Update the item text with the new text
	SetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem, pDispInfo->item.pszText);

	GetParent()->SendMessage(WM_VALIDATE, GetDlgCtrlID(), (LPARAM)pDispInfo); 
	
	*pResult = 0;
}

void CListCtrlEx::SetValidEditCtrlCharacters(CString &rstrValidCharacters)
{
	m_strValidEditCtrlChars = rstrValidCharacters;
}

void CListCtrlEx::EnableHScroll(bool bEnable /*= true*/)
{
	if (bEnable)
	{
		m_dwDropDownCtrlStyle |= WS_HSCROLL;
	}
	else
	{
		m_dwDropDownCtrlStyle &= ~WS_HSCROLL;
	}	
}

void CListCtrlEx::EnableVScroll(bool bEnable /*= true*/)
{
	if (bEnable)
	{
		m_dwDropDownCtrlStyle |= WS_VSCROLL;
	}
	else
	{
		m_dwDropDownCtrlStyle &= ~WS_VSCROLL;
	}
}

void CListCtrlEx::ScrollToView(int iColumnIndex, /*int iOffSet, */CRect& robCellRect)
{
	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);

	int iColumnWidth = GetColumnWidth(iColumnIndex);

	// Get the column iOffset
	int iOffSet = 0;
	for (int iIndex_ = 0; iIndex_ < iColumnIndex; iIndex_++)
	{
		iOffSet += GetColumnWidth(iIndex_);
	}

	// If x1 of cell rect is < x1 of ctrl rect or
	// If x1 of cell rect is > x1 of ctrl rect or **Should not ideally happen**
	// If the width of the cell extends beyond x2 of ctrl rect then
	// Scroll

	CSize obScrollSize(0, 0);

	if (((iOffSet + robCellRect.left) < rcClient.left) || 
		((iOffSet + robCellRect.left) > rcClient.right))
	{
		obScrollSize.cx = iOffSet + robCellRect.left;
	}
	else if ((iOffSet + robCellRect.left + iColumnWidth) > rcClient.right)
	{
		obScrollSize.cx = iOffSet + robCellRect.left + iColumnWidth - rcClient.right;
	}

	Scroll(obScrollSize);
	robCellRect.left -= obScrollSize.cx;
	
	// Set the width to the column width
	robCellRect.left += iOffSet;
	robCellRect.right = robCellRect.left + iColumnWidth;
}

void CListCtrlEx::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (IsReadOnly(pDispInfo->item.iSubItem))
	{
		*pResult = 1;
		return;
	}

	*pResult = 0;
}

void CListCtrlEx::SortAscending( int iIndex )
{
	if ( !m_bEnableSort )
		return;
	std::multimap<std::string , std::vector<std::string> >m_sortMap;
	std::multimap<std::string , std::vector<std::string> >::iterator m_sortMapfItr;
	std::multimap<std::string , std::vector<std::string> >::reverse_iterator m_sortMaprItr;

	CHeaderCtrl *pCtrl = this->GetHeaderCtrl();

	if ( pCtrl->GetItemCount() < ( iIndex + 1 ) )
		return;

	if ( iIndex < 1 )
		return;
	if ( m_RowCollection.size() < 1 )
		return;
	m_sortMap.clear();

	for ( m_CollectionItr = m_RowCollection.begin(); m_CollectionItr != m_RowCollection.end() ; m_CollectionItr++ )
	{
		//m_sortMap.insert( m_CollectionItr->at( iIndex ) ,  *m_CollectionItr );
		m_sortMap.insert(pair<std::string , std::vector<std::string> >( m_CollectionItr->at( iIndex ) ,  *m_CollectionItr ) );

	}

	m_RowCollection.clear();

	for ( m_sortMapfItr = m_sortMap.begin() ; m_sortMapfItr != m_sortMap.end() ; m_sortMapfItr++ )
	{
		m_RowCollection.push_back( m_sortMapfItr->second );
	}
	m_sortMap.clear();
	CListCtrl::Invalidate( TRUE );

}

void CListCtrlEx::SortDescending( int iIndex )
{
	if ( !m_bEnableSort )
		return;

	std::multimap<std::string , std::vector<std::string> >m_sortMap;
	std::multimap<std::string , std::vector<std::string> >::iterator m_sortMapfItr;
	std::multimap<std::string , std::vector<std::string> >::reverse_iterator m_sortMaprItr;

	CHeaderCtrl *pCtrl = this->GetHeaderCtrl();

	if ( pCtrl->GetItemCount() < ( iIndex + 1 ) )
		return;
	if ( iIndex < 0 )
		return;
	if ( m_RowCollection.size() < 1 )
		return;
	m_sortMap.clear();

	for ( m_CollectionItr = m_RowCollection.begin(); m_CollectionItr != m_RowCollection.end() ; m_CollectionItr++ )
	{
		//m_sortMap[m_CollectionItr->at( iIndex )] = *m_CollectionItr;
		m_sortMap.insert(pair<std::string , std::vector<std::string> >( m_CollectionItr->at( iIndex ) ,  *m_CollectionItr ) );
	}

	m_RowCollection.clear();

	for ( m_sortMaprItr = m_sortMap.rbegin() ; m_sortMaprItr != m_sortMap.rend() ; m_sortMaprItr++ )
	{
		m_RowCollection.push_back( m_sortMaprItr->second );
	}

	m_sortMap.clear();
	CListCtrl::Invalidate( TRUE );
 
}
/*
void CListCtrlEx::OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	int nItem = phdr->iItem;

	if ( m_PrevIndex != -1 )
	{
		if ( m_PrevIndex == nItem )
		{
			m_bAscSort = !m_bAscSort;
		}
		else
		{
			m_bAscSort = TRUE;
		}
	}
	if ( m_bAscSort )
	{
		SortAscending( nItem );
	}
	else
	{
		SortDescending( nItem );
	}

	m_PrevIndex = nItem;
	
	*pResult = 0;
	
}*/

int CListCtrlEx::CopyToClipboard()
{
	if ( !m_bEnableCopy )
	{
		AfxMessageBox("Copy not supported.");
		return 0;
	}
	CWaitCursor obj;

	ASSERT(this);
	LONG lItem = -1;
	int nCol = -1;
	std::string strData;

	CHeaderCtrl *pCtrl = this->GetHeaderCtrl();
	nCol = pCtrl->GetItemCount();

	POSITION pos;
	pos = this->GetFirstSelectedItemPosition();
	while ( pos )
	{
		lItem = this->GetNextSelectedItem( pos );
		for ( int iIx = 0 ; iIx < nCol ; iIx++ )
		{
			strData += GetItemTextEx( lItem , iIx ) + "\t" ;
		}
		strData = strData.substr( 0 , strData.size() - 1 );
		strData += "\r\n";
	}
	if ( strData.empty() )
	{
		return 0;
	}
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, strData.size() +1 );
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, strData.c_str() );
	//	strcpy_s( buffer, strData.size() + 1 , strData.c_str() );
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}

	return 1;
}

int CListCtrlEx::PasteFromClipboard( BOOL bInsert )
{

	if ( !m_bEnablePaste )
	{
		AfxMessageBox("Paste not supported.");
		return 0;
	}
	SetRedraw( FALSE );
	CWaitCursor obj;
	char * buffer;
	UINT uiItem = 0; 
	vector<string> vStr;

	LPTSTR  lpTemp = NULL;
	TCHAR	ch	   = NULL;
	TCHAR	chPrev = NULL;
	TCHAR	cNull  = '\0';


	POSITION pos = GetFirstSelectedItemPosition();

	if ( pos == NULL )
	{
		uiItem = 0;
		if( bInsert )
		{
			uiItem = GetItemCount();
		}
	}
	else
	{
		uiItem = GetNextSelectedItem( pos );
		if ( GetItemCount() >  0 )
			uiItem++;
	}

	if(OpenClipboard())
	{
		buffer = (char*)GetClipboardData(CF_TEXT);
		while ( buffer && *buffer )
		{
			lpTemp = buffer;
			while ( buffer && *buffer ) 
			{ 
				ch = *buffer;
				if ( chPrev == 13 && ch == 10 )
				{
					break;
				}
				else if ( ch < 0 )
				{
					*buffer = 0;
				}
				chPrev = ch;
				buffer++;
			}
			if ( *lpTemp )
			{
				ch = *buffer;
				*buffer = cNull;
				SpiltWord( lpTemp , vStr );
				SetValues( vStr , uiItem , bInsert);
				vStr.clear();
				uiItem++;
				*buffer = ch;
				
			}
			if ( *buffer )
				buffer++;
		}
		
	}
	CloseClipboard(); 
	SetRedraw( TRUE );
	//Invalidate( TRUE );
	return 0;
}

void CListCtrlEx::SetValues( vector<string> &vStr , UINT uiItem , BOOL bInsert)
{
	CHeaderCtrl *pCtrl = GetHeaderCtrl();
	int nColmax = pCtrl->GetItemCount();
	vector<string>::iterator vItr;

	if ( vStr.size() > nColmax )
	{
		for (  ; nColmax < vStr.size() ; nColmax++ )
		{
			InsertColumn( nColmax , "" , LVCFMT_LEFT , 150 , -1 );
		}
	}
	
	if ( bInsert || ( uiItem >= (GetItemCount() ) ) || ( GetItemCount() == -1 ) )
	{
		InsertItem( uiItem , "");
	}
		
	vItr = vStr.begin();
	for ( int nIx = 0 ; ( nIx < nColmax ) && ( vItr != vStr.end() ) ; nIx++ , vItr++ )
	{
		SetItemText( uiItem , nIx , vItr->c_str() );
	}
	SetItemCountEx( GetItemCount());
}

void CListCtrlEx::SpiltWord( LPTSTR buffer , vector<string> &vStr )
{
	LPTSTR  lpTemp = NULL;
	TCHAR	ch	   = NULL;
	TCHAR	cNull  = '\0';
	string	sTmp;
	while ( buffer && *buffer )
	{
		lpTemp = buffer;
		while ( buffer && *buffer ) 
		{ 
			ch = *buffer;
			if ( ch == '\t')
			{
				break;
			}
			else if ( ch < 0 )
			{
				*buffer = 0;
			}
			buffer++;
		}
		if ( *lpTemp )
		{
			ch = *buffer;
			*buffer = cNull;
			sTmp = lpTemp; 
			*buffer = ch;

			if ( sTmp.size() > 500  )
				sTmp = sTmp.substr( 0 , 500 );

			vStr.push_back( sTmp );
		}
		if ( *buffer )
			buffer++;
	}

}

BOOL CListCtrlEx::SelectAll()
{
	size_t lMax = m_RowCollection.size();
	for ( size_t uiIx = 0 ; uiIx < lMax ; uiIx++ )
	{
		this->SetItemState(uiIx, LVIS_SELECTED, LVIS_SELECTED);
	}
	Invalidate( FALSE );
	return TRUE;
}

BOOL CListCtrlEx::DeselectAll()
{
	size_t lMax = m_RowCollection.size();
	for ( size_t uiIx = 0 ; uiIx < lMax ; uiIx++ )
	{
		this->SetItemState(uiIx, 0, LVIS_SELECTED);
	}
	Invalidate( FALSE );
	return TRUE;
}

int CListCtrlEx::ExportGridToCSV( CString &csSavePath ,BOOL bExportSel )
{
	CString csHeader;
	CString csResultRow;
	
	int nOmitCol = -1;
	CMap<int,int,int,int> cmOmitMap;
	CArray<int,int> caColOrder;

	if ( bExportSel )
	{
		POSITION pos = GetFirstSelectedItemPosition();
		if ( !pos )
		{
			AfxMessageBox("No records where selected.");
			return 0;
		}
	}

	if ( csSavePath.IsEmpty() )
	{
		CFileDialog fileDlg(FALSE,NULL,NULL,OFN_OVERWRITEPROMPT,"Comma Separated Values(*.csv)|*.csv||");;
		if ( fileDlg.DoModal() != IDOK )
			return 0;
		csSavePath = fileDlg.GetPathName(); 
	}
	CWaitCursor wait;

	csSavePath.MakeLower();
	if ( csSavePath.Find(".csv") == -1 )
		csSavePath += ".csv";

	CStdioFile cfCsvWriter;
	
	if ( !cfCsvWriter.Open( csSavePath ,CFile::modeWrite | CFile::modeCreate ) )
	{
		AfxMessageBox("The file is open. Please close the file and try again.");
		return -1;
	}
	CHeaderCtrl *pHeader = NULL;

	pHeader = GetHeaderCtrl();
	
	int nHeaderCount = pHeader->GetItemCount();
	if ( nHeaderCount == 0 ) 
	{
		cfCsvWriter.Close();
		return -1; 
	}  

	LPINT pnOrder = (LPINT) malloc(nHeaderCount*sizeof(int));
	ASSERT(pnOrder != NULL);
	GetColumnOrderArray(pnOrder, nHeaderCount);
	for ( int iIx=0 ; iIx < nHeaderCount ; iIx++ )
	{
		caColOrder.Add( pnOrder[iIx] );
	}
	free(pnOrder);

	for ( int nInitializeColumn = 0 ; nInitializeColumn <  nHeaderCount ; nInitializeColumn++ )
	{
		LPSTR pszColHeader; 
		LV_COLUMN pColumn;
		char buffer[100];
		memset(&pColumn,caColOrder.ElementAt(nInitializeColumn),sizeof(pColumn));
		pColumn.mask = LVCF_TEXT;
		pColumn.cchTextMax =100;
		pColumn.pszText = buffer;
		GetColumn(caColOrder.ElementAt(nInitializeColumn), &pColumn);
		pszColHeader = pColumn.pszText;
		csResultRow = pszColHeader;
		if ( csResultRow.Find("RecId") == 0 || csResultRow.Find("#") == 0)
		{
			cmOmitMap.SetAt( nInitializeColumn , nInitializeColumn );
			continue;
		}
		if ( csResultRow == "Source_File")
		{
			cmOmitMap.SetAt( nInitializeColumn , nInitializeColumn );
			continue; 
		}
		csHeader += "\"" + csResultRow + "\",";
	}
	csHeader.TrimLeft(",");
	csHeader.TrimRight(",");
	csHeader += "\n";
	cfCsvWriter.WriteString( csHeader );
	int nMaxRow = 0;
	int nDummy = 0;
	nMaxRow = GetItemCount();
	if ( bExportSel ) 
	{
		POSITION pos = GetFirstSelectedItemPosition();
		UINT iItem = 0;
		while ( pos )
		{
			csHeader = "";
			iItem = GetNextSelectedItem( pos );
			for (  int nInitializeColumn = 0;  nInitializeColumn <  nHeaderCount ; nInitializeColumn++ )
			{
				if ( cmOmitMap.Lookup( nInitializeColumn , nDummy ) == TRUE )
					continue;

				csResultRow = GetItemText( iItem , caColOrder.ElementAt(nInitializeColumn) );
				csResultRow.Replace("\"","'");
				csHeader += "\"" + csResultRow + "\",";
			}
			csHeader.TrimLeft(",");
			csHeader += "\n";
			cfCsvWriter.WriteString( csHeader );
		}
	}
	else
	{
		for ( int nRow = 0 ; nRow <  nMaxRow ; nRow++ )
		{ 
			csHeader = "";
			for ( int nInitializeColumn = 0 ; nInitializeColumn <  nHeaderCount ; nInitializeColumn++ )
			{
				if ( cmOmitMap.Lookup( nInitializeColumn , nDummy ) == TRUE )
					continue;
				csResultRow = GetItemText(nRow , caColOrder.ElementAt(nInitializeColumn) );
				csResultRow.Replace("\"","'");
				csHeader += "\"" + csResultRow + "\",";
			}
			csHeader.TrimLeft(",");
			csHeader += "\n";
			cfCsvWriter.WriteString( csHeader );
		} 
	}
	cfCsvWriter.Close();
	return 1;
}

BOOL CListCtrlEx::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch ( pMsg->message )
	{
	case(WM_KEYDOWN):
		if ( (GetKeyState( VK_CONTROL ) & 0x800 ) && pMsg->lParam == 0x2E0001 ) //Ctrl + C
		{
			CopyToClipboard();
			
		}
		else if ( (GetKeyState( VK_CONTROL ) & 0x800 ) && pMsg->lParam == 0x1E0001 ) //Ctrl + A
		{
			SelectAll();
		}
		else if ( (GetKeyState( VK_CONTROL ) & 0x800 ) && pMsg->lParam == 0x2F0001 ) //Ctrl + V
		{
			PasteFromClipboard();
		}
		else if ( (GetKeyState( VK_CONTROL ) & 0x800 ) && pMsg->lParam == 0x210001 ) //Ctrl + F
		{
			GetParent()->SendMessage(WM_FIND_GRID, 0, 0);
		}
		else if ( pMsg->wParam == VK_DELETE )
		{
			DeleteSelected();
		}
		else if ( pMsg->wParam == VK_ESCAPE) 
		{
			DeselectAll();
		}
		break;
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}

int CListCtrlEx::CheckAll()
{

	int nCount = GetItemCount();
	for(int nItem = 0; nItem < nCount; nItem++)
	{
		SetCheck( nItem , TRUE );
	}
	Invalidate( FALSE );
	return 1;
}

int CListCtrlEx::UnCheckAll()
{
	int nCount = GetItemCount();
	for(int nItem = 0; nItem < nCount; nItem++)
	{
		SetCheck( nItem , FALSE );
	}
	Invalidate( FALSE );
	return 1;
}
void CListCtrlEx::ClearGrid()
{
	DeleteAllItems();
	CHeaderCtrl *pHeader = NULL;
	pHeader = GetHeaderCtrl();
	int nMaxCol = pHeader->GetItemCount();
	while ( nMaxCol != 0 )
	{
		for ( int nCol = 0; nCol < nMaxCol; nCol++)
		{
			DeleteColumn( nCol );
		}
		nMaxCol = pHeader->GetItemCount();
	}
	Invalidate( FALSE );

}

int CListCtrlEx::PasteFromClipboardAsNewCol()
{
	char * buffer;
	UINT uiItem = 0;
	vector<string> vStr;

	LPTSTR  lpTemp = NULL;
	TCHAR	ch	   = NULL;
	TCHAR	chPrev = NULL;
	TCHAR	cNull  = '\0';
	BOOL bFirst = TRUE;
	
	CHeaderCtrl *pHdr = GetHeaderCtrl();

	int nCol = pHdr->GetItemCount();

	POSITION pos = GetFirstSelectedItemPosition();

	if ( pos == NULL )
	{
		uiItem = 0;
	}
	else
	{
		uiItem = GetNextSelectedItem( pos );
	}

	if(OpenClipboard())
	{
		buffer = (char*)GetClipboardData(CF_TEXT);
		while ( buffer && *buffer )
		{
			lpTemp = buffer;
			while ( buffer && *buffer ) 
			{ 
				ch = *buffer;
				if ( chPrev == 13 && ch == 10 )
				{
					break;
				}
				else if ( ch < 0 )
				{
					*buffer = 0;
				}
				chPrev = ch;
				buffer++;
			}
			if ( *lpTemp )
			{
				ch = *buffer;
				*buffer = cNull;
				SpiltWord( lpTemp , vStr );

				if ( bFirst )
				{
					SetValues( vStr , uiItem , nCol , TRUE );
					bFirst = FALSE;
				}
				else
				{
					SetValues( vStr , uiItem , nCol , FALSE );
				}
				vStr.clear();
				uiItem++;
				*buffer = ch;
				
			}
			if ( *buffer )
				buffer++;
		}
		
	}
	CloseClipboard();
	SetRedraw( TRUE );
	//Invalidate( TRUE );
	return 1;
}

// Insert as New Col
void CListCtrlEx::SetValues( std::vector<std::string>&vStr , UINT uiItem , UINT uiCol , BOOL bInsert )
{
	CHeaderCtrl *pCtrl = GetHeaderCtrl();
	int nColmax = 0;
	vector<string>::iterator vItr;

	if ( bInsert )
	{
		nColmax = pCtrl->GetItemCount();

		for (  int i = 0; i < vStr.size() ; i++ )
		{
			InsertColumn( nColmax  + i, "" , LVCFMT_LEFT , 150 , -1 );
		}
		
	}
	
	if ( ( uiItem >= (GetItemCount() ) ) || ( GetItemCount() == -1 ) )
	{
		InsertItem( uiItem , "");
	}
	
	nColmax = pCtrl->GetItemCount();

	vItr = vStr.begin();
	for ( int nIx = uiCol ; ( nIx < nColmax ) && ( vItr != vStr.end() ) ; nIx++ , vItr++ )
	{
		SetItemText( uiItem , nIx , vItr->c_str() );
	}
	SetItemCountEx( GetItemCount() );
	
}

int CListCtrlEx::DeleteSelected()
{
	SetRedraw( FALSE );
	UINT iItem = 0; 
	UINT iDelCount = 0;
	POSITION pos;
	pos = this->GetFirstSelectedItemPosition();
	map< UINT , UINT> mapDelRows;
	map< UINT , UINT>::iterator itrMap;
	while ( pos )
	{
		iItem = this->GetNextSelectedItem( pos );
		mapDelRows[ iItem ] = iItem;
	}
	for ( itrMap = mapDelRows.begin() ; itrMap != mapDelRows.end() ; itrMap++ )
	{
		DeleteItem( (itrMap->first) - iDelCount );
		iDelCount++;
	}
	SetRedraw( TRUE );
	Invalidate( TRUE );
	return 1;
}

int CListCtrlEx::GetAllColumns( CStringArray &csaArray )
{
	csaArray.RemoveAll();
	CHeaderCtrl *pHdr = GetHeaderCtrl();
	int nMaxCol =  pHdr->GetItemCount();
	if ( nMaxCol < 1 )
		return -1;

	CString csHeader;
	LPSTR pszColHeader; 
	LV_COLUMN pColumn;
	char buffer[100];

	int nSourceFile = -1;

	for ( int nCol = 0; nCol < nMaxCol ; nCol++ )
	{
		memset(&pColumn,0,sizeof(pColumn));
		pColumn.mask = LVCF_TEXT;
		pColumn.cchTextMax =100;
		pColumn.pszText = buffer;
		GetColumn(nCol, &pColumn);
		pszColHeader = pColumn.pszText;
		csHeader = pszColHeader;
		csaArray.Add( csHeader );
	}
	return  nMaxCol;

}


int CListCtrlEx::GetColumnHeader( int nCol , CString &csHeader)
{
	if ( nCol > ( GetHeaderCtrl()->GetItemCount() - 1 ) )
		return -1;

	LPSTR pszColHeader; 
	LV_COLUMN pColumn;
	char buffer[100];
	memset(&pColumn,0,sizeof(pColumn));
	pColumn.mask = LVCF_TEXT;
	pColumn.cchTextMax =100;
	pColumn.pszText = buffer;
	GetColumn(nCol, &pColumn);
	pszColHeader = pColumn.pszText;
	csHeader = pszColHeader;

	return 1;
}
int CListCtrlEx::FindNext( CString csText , BOOL bSubString  , BOOL bCaseSensitive )
{
	if ( csText.IsEmpty() )
	{
		GetParent()->SendMessage( WM_FIND_ROW_COL , -1 , -1 );
		return -1;
	}
	if ( csText != m_csPrevFind )
	{
		ClearFindMap();
		FillFindMap( csText , bSubString , bCaseSensitive );
		m_itrFind = m_FindMap.begin();
	}
	m_csPrevFind = csText;
	if ( m_FindMap.size() == 0 )
	{
		FillFindMap( csText , bSubString , bCaseSensitive );
		m_itrFind = m_FindMap.begin();
	}
	if ( m_itrFind != m_FindMap.end() )
	{
		CRect rect;
		GetItemRect( m_itrFind->first , &rect , LVIR_BOUNDS );
		for ( int iColIx = 0 ; iColIx < m_itrFind->second ; iColIx++ )
		{
			rect.left += GetColumnWidth( iColIx );
		}
		CSize size( rect.left , rect.top );
		GetParent()->SendMessage( WM_FIND_ROW_COL , (WPARAM)m_itrFind->first , (LPARAM)m_itrFind->second );
		Scroll( size );
		EnsureVisible( m_itrFind->first , FALSE );
		Invalidate( TRUE );
		m_itrFind++;
	}
	else
	{
		GetParent()->SendMessage( WM_FIND_ROW_COL , -1 , -1 );
		Invalidate( TRUE );
		return 1;
		
	}
		
	return 1;
}
int CListCtrlEx::FindPrev( CString csText , BOOL bSubString , BOOL bCaseSensitive )
{
	if ( csText.IsEmpty() )
	{
		GetParent()->SendMessage( WM_FIND_ROW_COL , -1 , -1 );
		Invalidate( TRUE );
		return -1;
	}
	if ( m_FindMap.size() < 1 )
	{
		GetParent()->SendMessage( WM_FIND_ROW_COL , -1 , -1 );
		Invalidate( TRUE );
		return -1;
	}
 
	if ( csText != m_csPrevFind )
	{
		ClearFindMap();
		FillFindMap( csText , bSubString , bCaseSensitive );
		m_itrFind = m_FindMap.begin();
	}
	m_csPrevFind = csText;

	if ( m_itrFind == m_FindMap.begin() )
	{
		GetParent()->SendMessage( WM_FIND_ROW_COL , -1 , -1 );
		Invalidate( TRUE );
		return 1;
	}
	else if ( m_itrFind == m_FindMap.end() )
	{
		std::multimap<LONG , LONG>::iterator prvItr;
		for ( m_itrFind = m_FindMap.begin(); m_itrFind != m_FindMap.end() ; m_itrFind++ )
		{
			prvItr = m_itrFind;
		}
		m_itrFind = prvItr;
	}
	else
	{
		m_itrFind--;
	}

	CRect rect;
	GetItemRect( m_itrFind->first , &rect , LVIR_BOUNDS );
	for ( int iColIx = 0 ; iColIx < m_itrFind->second ; iColIx++ )
	{
		rect.left += GetColumnWidth( iColIx );
	}
	CSize size( rect.left , rect.top );
	GetParent()->SendMessage( WM_FIND_ROW_COL , (WPARAM)m_itrFind->first , (LPARAM)m_itrFind->second );
	Scroll( size );
	EnsureVisible( m_itrFind->first , FALSE );
	Invalidate( TRUE );
	return 1;
}

int CListCtrlEx::FillFindMap( CString csText , BOOL bSubString , BOOL bCaseSensitive )
{
	CString csData;
	LONG lRow = 0;
	LONG lCol = 0;
	if ( !bCaseSensitive )
	{
		csText.MakeLower();
	}
	for ( m_CollectionItr = m_RowCollection.begin() ; m_CollectionItr != m_RowCollection.end() ; m_CollectionItr++ , lRow++ )
	{
		lCol = 0;

		for ( std::vector<std::string>::iterator itrStr = m_CollectionItr->begin() ; itrStr != m_CollectionItr->end() ; itrStr++ , lCol++ )
		{
			csData = itrStr->c_str() ;
			if ( bSubString == TRUE && bCaseSensitive == TRUE )
			{
				if ( csData.Find( csText ) > -1 )
				{
					m_FindMap.insert(pair<LONG ,LONG >(lRow , lCol ) );
					
				}
			}
			else if ( bSubString == FALSE && bCaseSensitive == FALSE )
			{
				csData.MakeLower();
				if ( csText == csData )
				{
					m_FindMap.insert(pair<LONG ,LONG >(lRow , lCol ) );
				}
			}
			else if ( bSubString == TRUE && bCaseSensitive == FALSE )
			{
				csData.MakeLower();
				if ( csData.Find( csText ) > -1 )
				{
					m_FindMap.insert(pair<LONG ,LONG >(lRow , lCol ) );
				}

			}
			else if ( bSubString == FALSE && bCaseSensitive == TRUE )
			{
				if ( csText == csData  )
				{
					m_FindMap.insert(pair<LONG ,LONG >(lRow , lCol ) );
				}

			}

		}

	}
	m_itrFind = m_FindMap.begin();
	return 1;
}
void CListCtrlEx::OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMHEADER* pNMHead = (NMHEADER*)pNMHDR;
	// TODO: Add your control notification handler code here
	int iItem = pNMHead->iItem;
	CString csHeader;
	GetColumnHeader( iItem ,  csHeader );
	m_SortOrderMapItr = m_SortOrderMap.find( csHeader );

	if ( m_SortOrderMapItr == m_SortOrderMap.end() )
	{
		m_SortOrderMap[csHeader] = FALSE;
		SortAscending( iItem );
		int nMax = GetItemCount();
		CString csSNo;
		for ( int nIdx = 0 ; nIdx < nMax ; nIdx++ )
		{
			csSNo.Format("%d" , nIdx + 1 );
			SetItemText( nIdx , 0 , csSNo );
		}
	}
	else
	{
		if ( m_SortOrderMapItr->second )
		{
			m_SortOrderMap[csHeader] = FALSE;
			SortAscending( iItem );
			int nMax = GetItemCount();
			CString csSNo;
			for ( int nIdx = 0 ; nIdx < nMax ; nIdx++ )
			{
				csSNo.Format("%d" , nIdx + 1 );
				SetItemText( nIdx , 0 , csSNo );
			}
		}
		else
		{
			m_SortOrderMap[csHeader] = TRUE;
			SortDescending( iItem );
			int nMax = GetItemCount();
			CString csSNo;
			for ( int nIdx = 0 ; nIdx < nMax ; nIdx++ )
			{
				csSNo.Format("%d" , nIdx + 1 );
				SetItemText( nIdx , 0 , csSNo );
			}
		}
	}

	*pResult = 0;
}
