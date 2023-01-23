#pragma once


// CDigitPatternInfoDlg dialog

class CDigitPatternInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDigitPatternInfoDlg)

public:
	CDigitPatternInfoDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDigitPatternInfoDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DIGIT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_szDigit;
};
