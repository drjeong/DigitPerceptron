
// DigitPerceptronDlg.h : header file
//

#pragma once
#include "DigitPerceptronViz.h"
#include "DataManager.h"
#include "GraphObject.h"

#include <map>
#include <vector>

#if _MSC_VER > 1020 // if VC++ version is > 4.2
using namespace std; // std c++ libs implemented in std
#endif


// CDigitPerceptronDlg dialog
class CDigitPerceptronDlg : public CDialogEx
{
// Construction
public:
	CDigitPerceptronDlg(CWnd* pParent = nullptr);	// standard constructor
	void DrawBlock();
	void RedrawGridBlocks();
	void ResetGridBlocks();

	void NNRunRecognition();
	void NNProcessingAllPatterns();
	void InsertPatternsIntoList();
	void UpdatePatternsCntIntoList();
	void LoadPatternFile(CString& szfilePathName);
	void SavePatternFile(CString& szfilePathName);

	void LoadPatternCSVFormatFile(CString& szfilePathName);
	void SavePatternCSVFormatFile(CString& szfilePathName);
	void SaveNNToFile();

	void CreatePatternResultsGraphs();
	
	CGraphObject *m_pGraphObjects;

	CDigitPerceptronViz *m_pVisualRepresentation;
	CDigitPerceptronViz* GetVisualRepresentation() {
		return m_pVisualRepresentation;
	};
	void UpdateVisualRepresentation();
	void TimeEvaluation_DS2400();
	void TimeEvaluation_TTMNIST();
	void MNISTDataConversion();
	void ResetGraph();

	CDataManager m_DataManager; // Data Manager
	CDataManager* GetDataManager() { return &m_DataManager; };

	double	m_dGridWidth;
	double	m_dGridHeight;
	MatrixXb	m_mtrxGridBlocks;	
	BOOL	m_bPatternDataModified;	// indicator to check if the pattern data has been modified.
	BOOL	m_bDrawingActivated;
	CString m_szDigitPatternFilePathName;

	UINT m_nPCAInputDataType;

	typedef struct {
		double NNOutput;
		int Digit;
	}DIGITWEIGHT;

	// Neural Network
	BOOL m_NeuralNetworkProcessed;


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLGDIGIPATTERNGENERATOR_DIALOG};
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedRunNNProcessing();
	afx_msg void OnHelpAbout();
	int m_nNumInputNode;
	int m_nNumHiddenNode;
	int m_nNumOutputNode;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnFileOpenPatternFile();
	int m_nTotalPatterns;
	CString m_szDigitForPattern;
	CString m_szDigitPatternFileName;
	afx_msg void OnFileSavePatternFile();
	CListCtrl m_ctrlPatternsGrid;
	CListCtrl m_ctrlPatternsCntGrid;
	afx_msg void OnBnClickedAddCurrentpattern();
	afx_msg void OnBnClickedButtonClearscreen();
	afx_msg void OnFileExit();
	afx_msg void OnDblclkListPatterns(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonDigitrecognition();
	CProgressCtrl m_ctrlNNProgress;
	CString m_szMSE;
	afx_msg void OnVisualRepresentation();
	afx_msg void OnItemchangedListPatterns(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedRemoveSelectedpattern();
	afx_msg void OnClickListPatterns(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusListPatterns(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPcainputdataInputnodes();
	afx_msg void OnPcainputdataInputandhiddennodes();
	afx_msg void OnPcainputdataInputhiddenoutputnodes();
	afx_msg void OnDestroy();
	double m_dNumErrorRate;
};
