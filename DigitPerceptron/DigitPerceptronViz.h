#pragma once
#include "GLPCA.h"
#include "ControlPos.h"

// CDigitPerceptronViz dialog

class CDigitPerceptronViz : public CDialogEx
{
	DECLARE_DYNAMIC(CDigitPerceptronViz)

public:
	CDigitPerceptronViz(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDigitPerceptronViz();

	void SetViews();

	CGLPCA* GetGLPCA() { return &m_GLPCA; };
	CControlPos m_cControlPos;
	CGLPCA m_GLPCA;


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_VIZ };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
