// CDigitPatternInfoDlg.cpp : implementation file
//

#include "pch.h"
#include "DigitPerceptron.h"
#include "DigitPatternInfoDlg.h"
#include "afxdialogex.h"


// CDigitPatternInfoDlg dialog

IMPLEMENT_DYNAMIC(CDigitPatternInfoDlg, CDialogEx)

CDigitPatternInfoDlg::CDigitPatternInfoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DIGIT, pParent)
	, m_szDigit(_T(""))
{

}

CDigitPatternInfoDlg::~CDigitPatternInfoDlg()
{
}

void CDigitPatternInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DLGDIGIT_EDIT_DIGIT, m_szDigit);
}


BEGIN_MESSAGE_MAP(CDigitPatternInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDigitPatternInfoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDigitPatternInfoDlg message handlers


void CDigitPatternInfoDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	CDialogEx::OnOK();
}
