//------------------------------------------------------------------------------
// ControlPos.cpp
// 
//	CControlPos 
//	Position controls on a form's resize 
// 
// 
#include "pch.h"
#include "ControlPos.h"

//------------------------------------------------------------------------------
// CControlPos::CControlPos
// 
//	default constructor 
// 
//	Access: public
// 
//	Args:
//		CWnd* pParent				=	pointer to parent window
// 
//	Return:
//		none
// 
CControlPos::CControlPos(CWnd* pParent /* = NULL */)
{
	m_pParent = pParent;
	UpdateParentSize();

	m_nOldParentHeight = 0;
	m_nOldParentWidth = 0;
	
	SetNegativeMoves(FALSE);
//	SetNegativeMoves(true);
	ResetControls();
}

//------------------------------------------------------------------------------
// CControlPos::~CControlPos
// 
//	default destructor -- It deletes all controls. 
// 
//	Access: public
// 
//	Args:
//		none
// 
//	Return:
//		none
// 
CControlPos::~CControlPos()
{
	ResetControls();
}

//------------------------------------------------------------------------------
// CControlPos::SetParent
// 
//	This sets the parent window. It should be called from a CWnd's 
//    post-constructor function, like OnInitdialog or InitialUpdate. 
// 
//	Access: public
// 
//	Args:
//		CWnd* pParent	=	parent window
// 
//	Return:
//		none
// 
void CControlPos::SetParent(CWnd* pParent)
{
	CRect rcParentOriginalSize;

	m_pParent = pParent;

	m_pParent->GetClientRect(rcParentOriginalSize);
	m_nOriginalParentWidth = rcParentOriginalSize.right;
	m_nOriginalParentHeight = rcParentOriginalSize.bottom;
	m_nOldParentHeight = m_nOriginalParentHeight;
	m_nOldParentWidth = m_nOriginalParentWidth;
	m_pParent->GetWindowRect(m_nOriginalParentRounds);
	
	UpdateParentSize();
}

void CControlPos::SetChangedInitialSize(CWnd* pParent)
{
	CRect rcParentOriginalSize;

	m_pParent->GetClientRect(rcParentOriginalSize);
	m_nOriginalParentWidth = rcParentOriginalSize.right;
	m_nOriginalParentHeight = rcParentOriginalSize.bottom;
	m_nOldParentHeight = m_nOriginalParentHeight;
	m_nOldParentWidth = m_nOriginalParentWidth;
	m_pParent->GetWindowRect(m_nOriginalParentRounds);
	
	UpdateParentSize();
}

//------------------------------------------------------------------------------
// CControlPos::AddControl
// 
//	This adds a control to the internal list of controls in CControlPos.  
// 
//	Access: public
// 
//	Args:
//		CWnd* pControl				=	pointer to the control to be added
//		const DWORD& dwStyle		=  how the window should be moved -- see #define's
//                               in the header file
// 
//	Return:
//		BOOL 	=	TRUE if the control was added successfully, FALSE otherwise
// 
BOOL CControlPos::AddControl(CWnd* pControl, const DWORD& dwStyle ,UINT vPos,UINT hPos,
							 UINT vNum,UINT hNum/* = CP_MOVE_HORIZONTAL */, BOOL bUpperBound /*FALSE*/)
{
	BOOL fReturnValue = TRUE;

	if (pControl && m_pParent)
	{
		LPCONTROLDATACHART pstControl = new CONTROLDATACHART;
		pstControl->hControl = pControl->GetSafeHwnd();
		pControl->GetWindowRect(pstControl->OriRect);
		pstControl->dwStyle = dwStyle;
		pstControl->vPos = vPos;
		pstControl->hPos = hPos;
		pstControl->vNum = vNum;
		pstControl->hNum = hNum;
		pstControl->bUpperBound = bUpperBound;
		m_awndControls.Add(((CObject*)pstControl));
	}
	else
	{
		fReturnValue = FALSE;
	}

	return (fReturnValue);
}

//------------------------------------------------------------------------------
// CControlPos::AddControl
// 
//	This adds a control the internal list of controls in CControlPos. 
// 
//	Access: public
// 
//	Args:
//		const UINT& unId			=	ID of the control to add
//		const DWORD& dwStyle		=	how the window should be moved -- see #define's
//                               in the header file
// 
//	Return:
//		BOOL 	=	TRUE if the control was added successfully, FALSE otherwise
// 
BOOL CControlPos::AddControl(const UINT& unId, const DWORD& dwStyle ,UINT vPos,UINT hPos,
							 UINT vNum,UINT hNum/* = CP_MOVE_HORIZONTAL */, BOOL bUpperBound /*FALSE*/)
{
	CWnd* pControl;

	if (m_pParent)
	{
		pControl = m_pParent->GetDlgItem(unId);
		return (AddControl(pControl, dwStyle,vPos,hPos,vNum,hNum, bUpperBound));
	}
	else
	{
		return (FALSE);
	}
}

BOOL CControlPos::AddControl(HWND hWnd, const DWORD& dwStyle ,UINT vPos,UINT hPos,
							UINT vNum,UINT hNum/* CP_MOVE_HORIZONTAL */, BOOL bUpperBound /*FALSE*/)
{
	BOOL fReturnValue = TRUE;

	if (hWnd && m_pParent)
	{
		LPCONTROLDATACHART pstControl = new CONTROLDATACHART;
		pstControl->hControl = hWnd;
		GetWindowRect(hWnd, pstControl->OriRect);
		pstControl->dwStyle = dwStyle;
		pstControl->vPos = vPos;
		pstControl->hPos = hPos;
		pstControl->vNum = vNum;
		pstControl->hNum = hNum;
		pstControl->bUpperBound = bUpperBound;
		m_awndControls.Add(((CObject*)pstControl));
	}
	else
	{
		fReturnValue = FALSE;
	}

	return (fReturnValue);
}

//------------------------------------------------------------------------------
// CControlPos::RemoveControl
// 
//	If a client ever wants to remove a control programmatically, this 
//    function will do it. 
// 
//	Access: public
// 
//	Args:
//		CWnd* pControl	=	pointer of the window who should be removed from 
//								the internal control list [ie: will not be repositioned]
// 
//	Return:
//		BOOL 	=	TRUE if the control was found [and deleted], FALSE otherwise
// 
BOOL CControlPos::RemoveControl(CWnd* pControl)
{
	BOOL fReturnValue = FALSE;

	for (int i = 0; i < m_awndControls.GetSize(); i++)
	{
		LPCONTROLDATACHART pstControl = ((LPCONTROLDATACHART)m_awndControls.GetAt(i));
		
		if (pstControl->hControl == pControl->GetSafeHwnd())
		{
			m_awndControls.RemoveAt(i);
			delete pstControl;
			pstControl = NULL;
			fReturnValue = TRUE;
			break;
		}
	}

	return (fReturnValue);
}

//------------------------------------------------------------------------------
// CControlPos::RemoveControl
// 
//	If a client ever wants to remove a control programmatically, this 
//    function will do it. 
// 
//	Access: public
// 
//	Args:
//		const UINT& unId  =  ID of the control that should be removed from the
//                         internal control list [ie: will not be repositioned]
// 
//	Return:
//		BOOL 	=	TRUE if the control was found [and deleted], FALSE otherwise
// 
BOOL CControlPos::RemoveControl(const UINT& unId)
{
	CWnd* pControl;

	if (m_pParent)
	{
		pControl = m_pParent->GetDlgItem(unId);
		return (RemoveControl(pControl));
	}
	else
	{
		return (FALSE);
	}
}

//------------------------------------------------------------------------------
// CControlPos::ResetControls
// 
//	This function removes all controls from the CControlPos object 
// 
//	Access: public
// 
//	Args:
//		none
// 
//	Return:
//		none
// 
void CControlPos::ResetControls(void)
{
	while (m_awndControls.GetSize() > 0)
	{
		int   nHighIdx = m_awndControls.GetUpperBound();
		LPCONTROLDATACHART pstControl = ((LPCONTROLDATACHART)m_awndControls.GetAt(nHighIdx));
		if (pstControl)
		{
			m_awndControls.RemoveAt(nHighIdx);
			delete pstControl;
			pstControl = NULL;
		}
	}
}

//------------------------------------------------------------------------------
// CControlPos::MoveControls
// 
//	This function takes care of moving all controls that have been added to 
//    the object [see AddControl].  This function should be called from the 
//    WM_SIZE handler-function [typically OnSize]. 
// 
//	Access: public
// 
//	Args:
//		none
// 
//	Return: 
//		none
// 
void CControlPos::MoveControls(void)
{
	if (m_pParent)
	{
		//--------------------------------------------------------------------
		// for each control that has been added to our object, we want to
		// check its style and move it based off of the parent control's
		// movements.
		// the thing to keep in mind is that when you resize a window, you
		// can resize by more than one pixel at a time. this is important
		// when, for example, you start with a width smaller than the
		// original width and you finish with a width larger than the
		// original width. you know that you want to move the control, but
		// by how much? that is why so many if's and calculations are made
		//
		CRect rcParentFrameBounds;
		m_pParent->GetWindowRect(rcParentFrameBounds);
		//m_pParent->ShowWindow(SW_HIDE);
		for (int i = 0; i < m_awndControls.GetSize(); i++)
		{
			LPCONTROLDATACHART pstControl = ((LPCONTROLDATACHART)m_awndControls.GetAt(i));
			CRect rcParentBounds;
			CRect rcBounds;
			CWnd* pControl = m_pParent->FromHandle(pstControl->hControl);
			
			pControl->GetWindowRect(rcBounds);
			if(m_nOriginalParentRounds.top != rcParentFrameBounds.top)
			{
				pstControl->OriRect.bottom += rcParentFrameBounds.top-m_nOriginalParentRounds.top;
				pstControl->OriRect.top += rcParentFrameBounds.top-m_nOriginalParentRounds.top;
			}
			if(m_nOriginalParentRounds.left != rcParentFrameBounds.left)
			{
				pstControl->OriRect.right += rcParentFrameBounds.left-m_nOriginalParentRounds.left;
				pstControl->OriRect.left += rcParentFrameBounds.left-m_nOriginalParentRounds.left;
			}
			rcBounds = pstControl->OriRect;
			m_pParent->GetClientRect(rcParentBounds);

			if ((pstControl->dwStyle & (CP_RESIZE_VERTICAL)) == (CP_RESIZE_VERTICAL))
			{
				if (pstControl->bUpperBound == FALSE)
				rcBounds.bottom += (long)((double)(rcParentBounds.bottom - m_nOldParentHeight)/pstControl->vNum);
				else if (rcParentBounds.bottom < m_nOriginalParentHeight)
					rcBounds.bottom += (long)((double)(rcParentBounds.bottom - m_nOldParentHeight)/pstControl->vNum);
			}

			if ((pstControl->dwStyle & (CP_RESIZE_HORIZONTAL)) == (CP_RESIZE_HORIZONTAL))
			{
				if(pstControl->hNum)
				rcBounds.right += (long)((double)(rcParentBounds.right - m_nOldParentWidth)/pstControl->hNum);
			}

			if ((pstControl->dwStyle & (CP_MOVE_VERTICAL)) == (CP_MOVE_VERTICAL))
			{
				rcBounds.bottom += (long)((double)(rcParentBounds.bottom - m_nOldParentHeight)*(pstControl->vPos)/pstControl->vNum);
				rcBounds.top += (long)((double)(rcParentBounds.bottom - m_nOldParentHeight)*(pstControl->vPos)/pstControl->vNum);
			}

			if ((pstControl->dwStyle & (CP_MOVE_HORIZONTAL)) == (CP_MOVE_HORIZONTAL))
			{
				if(pstControl->hNum)
				{
					rcBounds.right += (long)((double)(rcParentBounds.right - m_nOldParentWidth)*(pstControl->hPos)/pstControl->hNum);
					rcBounds.left += (long)((double)(rcParentBounds.right - m_nOldParentWidth)*(pstControl->hPos)/pstControl->hNum);
				}
			}
			m_pParent->ScreenToClient(rcBounds);
			pControl->MoveWindow(rcBounds);
			pControl->Invalidate(FALSE);
		}
		m_pParent->ShowWindow(SW_SHOW);
		if(m_nOriginalParentRounds.top != rcParentFrameBounds.top)
		{
			m_nOriginalParentRounds.top = rcParentFrameBounds.top;
		}
		if(m_nOriginalParentRounds.left != rcParentFrameBounds.left)
		{
			m_nOriginalParentRounds.left = rcParentFrameBounds.left;
		}
//		UpdateParentSize();
	}
}

//------------------------------------------------------------------------------
// CControlPos::SetNegativeMoves
// 
//	This sets the NegativeMoves boolean parameter of the object. When the 
//    parent window becomes smaller than it started, setting this to FALSE 
//    will not allow controls to be moved; the parent size may change, but 
//    it'll just force the controls to go off of the 
//    This parameter defaults to FALSE on object creation.
// 
//	Access: public
// 
//	Args:
//		const BOOL& fNegativeMoves /* = TRUE */	=	value to set
// 
//	Return:
//		none
// 
void CControlPos::SetNegativeMoves(const BOOL& fNegativeMoves /* = TRUE */)
{
	m_fNegativeMoves = fNegativeMoves;
}

//------------------------------------------------------------------------------
// CControlPos::GetNegativeMoves
// 
//	This function returns whether or not negative moves are enabled. 
// 
//	Access: public
// 
//	Args:
//		none
// 
//	Return:
//		BOOL 	=	TRUE if negative moves are enabled, FALSE otherwise
// 
BOOL CControlPos::GetNegativeMoves(void) const
{
	return (m_fNegativeMoves);
}

//------------------------------------------------------------------------------
// CControlPos::UpdateParentSize
// 
//	Since CControlPos keeps track of the parent's size, it gets updated 
//    every time it tells us to size the controls. We keep track so we know 
//    how much it changed from the last WM_SIZE message. 
// 
//	Access: protected
// 
//	Args:
//		none
// 
//	Return:
//		none
// 
void CControlPos::UpdateParentSize(void)
{
	if (m_pParent)
	{
		CRect rcBounds;
		m_pParent->GetClientRect(rcBounds);

		m_nOldParentWidth = rcBounds.Width();
		m_nOldParentHeight = rcBounds.Height();
	}
}


RECT CControlPos::GetControlRect(HWND hWnd)
{
	MoveControls();
	RECT rect;
	for (int i = 0; i < m_awndControls.GetSize(); i++)
	{
		LPCONTROLDATACHART pstControl = ((LPCONTROLDATACHART)m_awndControls.GetAt(i));
		
		if (hWnd == pstControl->hControl)
		{
			rect = pstControl->ChangedRect;
//			m_pParent->ScreenToClient( &rect );
		}
	}
	return rect;
}
