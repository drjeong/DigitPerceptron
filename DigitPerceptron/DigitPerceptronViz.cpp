// DigitPerceptronViz.cpp : implementation file
//

#include "pch.h"
#include "DigitPerceptron.h"
#include "DigitPerceptronViz.h"
#include "afxdialogex.h"
#include "DigitPerceptronDlg.h"


// CDigitPerceptronViz dialog

IMPLEMENT_DYNAMIC(CDigitPerceptronViz, CDialogEx)

CDigitPerceptronViz::CDigitPerceptronViz(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_VIZ, pParent)
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif

}

CDigitPerceptronViz::~CDigitPerceptronViz()
{
}

void CDigitPerceptronViz::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDigitPerceptronViz, CDialogEx)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CDigitPerceptronViz message handlers


BOOL CDigitPerceptronViz::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString className = AfxRegisterWndClass(
		CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		NULL,
		(HBRUSH)GetStockObject(BLACK_BRUSH),
		NULL);

	CRect rect;
	GetDlgItem(IDC_VISUALIZATION_REGION)->GetWindowRect(rect);
	ScreenToClient(rect);

	m_GLPCA.CreateEx(
		0,
		className,
		_T("PCA"),
		WS_CHILD | WS_VISIBLE,
		rect,
		this,
		0);

	m_cControlPos.SetParent(this);
	m_cControlPos.AddControl(m_GLPCA,
		CP_RESIZE_HORIZONTAL | CP_RESIZE_VERTICAL, 1, 1, 1, 1);

	SetViews();

	((CDigitPerceptronDlg*)AfxGetMainWnd())->GetDataManager()->InitGLCallList();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDigitPerceptronViz::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	m_cControlPos.MoveControls();
	if (GetGLPCA())
	{
		if (IsWindow(GetGLPCA()->GetSafeHwnd()) == FALSE) return;
		GetGLPCA()->InitViews();

		m_GLPCA.Redraw();
	}
}


void CDigitPerceptronViz::OnPaint()
{
	//m_GLPCA.Redraw();
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialogEx::OnPaint() for painting messages

}


HCURSOR CDigitPerceptronViz::OnQueryDragIcon()
{
	// TODO: Add your message handler code here and/or call default
	return CDialogEx::OnQueryDragIcon();
}

void CDigitPerceptronViz::SetViews()
{
	if (GetGLPCA())
	{
		if (IsWindow(GetGLPCA()->GetSafeHwnd()) == FALSE) return;
		GetGLPCA()->InitViews();

		m_GLPCA.Redraw();
	}
}


BOOL CDigitPerceptronViz::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (::GetKeyState(VK_CONTROL) & 0x8000) m_GLPCA.SpecialKeyPressed(VK_CONTROL);
		if (::GetKeyState(VK_SHIFT) & 0x8000) m_GLPCA.SpecialKeyPressed(VK_SHIFT);
		if (::GetKeyState(VK_MENU) & 0x8000) m_GLPCA.SpecialKeyPressed(VK_MENU);
		break;
	case WM_KEYUP:
		m_GLPCA.SpecialKeyReleased();
		break;
	}


	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE; // Do not process further (not close the dialog window)
		}

		CDataManager* pDM = ((CDigitPerceptronDlg*)AfxGetMainWnd())->GetDataManager();
		int ctrlValue = ::GetKeyState(VK_CONTROL);
		if (ctrlValue)
		{
			switch (pMsg->wParam)
			{
			case VK_UP:
				pDM->GlyphSizeIncrease();
				if (GetGLPCA()) GetGLPCA()->Redraw();
				break;
			case VK_DOWN:
				pDM->GlyphSizeDecrease();
				if (GetGLPCA()) GetGLPCA()->Redraw();
				break;
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
