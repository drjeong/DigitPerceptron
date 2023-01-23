#include "pch.h"
#include "InteractionManager.h"
#include "DigitPerceptronDlg.h"
#include <algorithm>    // std::find

CInteractionManager::CInteractionManager(void *parent)
{
	m_pParent = parent;
	m_nCheckedPCACoordX = 0; // x coordinate in PCA space
	m_nCheckedPCACoordY = 1; // y coordinate in PCA space
	m_nHighlightedDimension = -1;
	m_nHighlightedObject = -1;
	m_nSelectedDimension = -1;
	m_nDataManipulationItem = -1;
	m_bShowLabel = TRUE;
	m_bEnableTrail = FALSE;
	m_bItemSelection = FALSE;
}


CInteractionManager::~CInteractionManager(void)
{
}


BOOL CInteractionManager::IsItemSelected(int rkey)
{
	if (m_nSelectedItems.size()==0) return FALSE;
	
	auto it = std::find(m_nSelectedItems.begin(), m_nSelectedItems.end(), rkey); // search
	if (it != m_nSelectedItems.end()) return TRUE;

	return FALSE;
}

void CInteractionManager::SetSelectUnselectDimension(int ckey)
{
	if (ckey==-1) return; // None of the key selected

	if (m_nSelectedDimension==ckey)
	{ // disable
		m_nSelectedDimension = -1;
	}
	else
	{ // enable
		m_nSelectedDimension = ckey;
	}
}

void CInteractionManager::SetSelectItem(int rkey)
{
	if (rkey==-1) return; // None of the key selected

	// rkey selection
	auto it = std::find(m_nSelectedItems.begin(), m_nSelectedItems.end(), rkey); // search
	
	// if not found, add rkey to the list
	if (it == m_nSelectedItems.end()) m_nSelectedItems.push_back(rkey);
}

void CInteractionManager::SetSelectUnselectItem(int rkey)
{
	if (rkey==-1) return; // None of the key selected

	// rkey search in the list
	auto it = std::find(m_nSelectedItems.begin(), m_nSelectedItems.end(), rkey);
	
	if (it != m_nSelectedItems.end()) // item is selected already
	{ // disable
		 m_nSelectedItems.erase(it);  // remove from the list

		//if (((CDigitPerceptronDlg*)AfxGetApp()->m_pMainWnd)->GetDetailViewDlg())
		//	((CDigitPerceptronDlg*)AfxGetApp()->m_pMainWnd)->GetDetailViewDlg()->PostMessage(WM_UNSELECTITEM, (WPARAM)rkey, 0);
	}
	else
	{ // enable
		m_nSelectedItems.push_back(rkey); // add to the list

		//if (((CDigitPerceptronDlg*)AfxGetApp()->m_pMainWnd)->GetDetailViewDlg())
		//	((CDigitPerceptronDlg*)AfxGetApp()->m_pMainWnd)->GetDetailViewDlg()->PostMessage(WM_SELECTITEM, (WPARAM)rkey, 0);
	}
}

void CInteractionManager::SetDeleteItem(int rkey)
{
	if (rkey==-1) return; // None of rkey selected

	vector<int>::iterator it = find(m_nDeletedItems.begin(), m_nDeletedItems.end(), rkey);
	if (it == m_nDeletedItems.end()) m_nDeletedItems.push_back(rkey);
}


BOOL CInteractionManager::ResetSelectedItems()
{
	if (m_nSelectedItems.size()>0)
	{
		m_nSelectedItems.clear();
		//RefreshViews();
		return TRUE;
	}
	return FALSE;
}

BOOL CInteractionManager::ResetDeletedItems()
{
	if (m_nDeletedItems.size()>0)
	{
		m_nDeletedItems.clear();
		//RefreshViews();
		return TRUE;
	}
	return FALSE;
}

void CInteractionManager::Init()
{
	m_nCheckedPCACoordX = 0; // 1st variable
	m_nCheckedPCACoordY = 1; // 2nd variable
	m_nHighlightedDimension = -1;
	m_nHighlightedObject = -1;
	m_nDataManipulationItem = -1;
	m_nSelectedDimension = -1;
	m_bShowLabel = FALSE;
	m_bEnableTrail = FALSE;

	ResetSelectedItems();
	ResetDeletedItems();
}

void CInteractionManager::RefreshViews()
{
	((CDigitPerceptronDlg*)AfxGetMainWnd())->Invalidate(FALSE);

	if (((CDigitPerceptronDlg*)AfxGetMainWnd())->GetVisualRepresentation() != NULL)
		((CDigitPerceptronDlg*)AfxGetMainWnd())->GetVisualRepresentation()->Invalidate(FALSE);

}
