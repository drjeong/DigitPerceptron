#if !defined(AFX_FLIPPINGCONTROL_H__B51A35EA_CA1A_4D38_A8CA_3EE15E7AD751__INCLUDED_)
#define AFX_FLIPPINGCONTROL_H__B51A35EA_CA1A_4D38_A8CA_3EE15E7AD751__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PCASignFlipController.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPCASignFlipController window

class CPCASignFlipController
{
// Construction
public:
	CPCASignFlipController();

// Attributes
public:
	MatrixXd m_matPrevPCAData;
	MatrixXd m_matPrevEigenvalue;
	MatrixXd m_matPrevLeigenvector;
	MatrixXd m_matPrevReigenvectorT;

	size_t		m_nRow;
	size_t		m_nCol;
	BOOL	m_bInit;
	BOOL	m_bDimensionChanged;
	BOOL	*m_pbNoFlipping;
	void	*m_pParent;

// Operations
public:
	void SetDimensionChanged(BOOL val){m_bDimensionChanged=val;};
	void FlippingControl();
	void Initialize(int row, int col, BOOL *noflipping);
	void Update(size_t row, size_t col, BOOL *noflipping=NULL);
	void FlippingControl(MatrixXd &DataPCA, MatrixXd &eigenvalue, 
					   MatrixXd &leigenvector/*U*/, MatrixXd &reigenvectorT/*V^T*/);
	//void FlippingControlTest(MatrixXd &InputData, MatrixXd &OutputData, 
	//					MatrixXd &eigenvalue, MatrixXd &leigenvector, MatrixXd &reigenvector);
	//void FlippingControl_SignFlip(MatrixXd &InputData, MatrixXd &OutputData, 
	//					MatrixXd &eigenvalue, MatrixXd &leigenvector, MatrixXd &reigenvector);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPCASignFlipController)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPCASignFlipController();

	// Generated message map functions
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLIPPINGCONTROL_H__B51A35EA_CA1A_4D38_A8CA_3EE15E7AD751__INCLUDED_)
