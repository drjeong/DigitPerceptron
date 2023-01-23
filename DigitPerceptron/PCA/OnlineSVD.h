#if !defined(AFX_ONLINESVD_H__D92B51F8_A6D0_490B_B4CE_2EDE3A67DF08__INCLUDED_)
#define AFX_ONLINESVD_H__D92B51F8_A6D0_490B_B4CE_2EDE3A67DF08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OnlieSVD.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COnlineSVD window

class COnlineSVD
{
// Construction
public:
	COnlineSVD();

// Attributes
public:
	MatrixXd m_matPrevInputData;
	MatrixXd m_matPrevEigenvalue;
	MatrixXd m_matPrevLeigenvector;
	MatrixXd m_matPrevReigenvector;

// Operations
public:
	BOOL m_bInitialized;

// Implementation
public:
	virtual ~COnlineSVD();
	BOOL IsInitialized();
	void InitPCADATA(MatrixXd &InputData, MatrixXd &eigenvalue, MatrixXd &leigenvector, MatrixXd &reigenvector);
	void OnlineSVD(MatrixXd &InputData, MatrixXd &OutputData, 
						MatrixXd &eigenvalue, MatrixXd &leigenvector, MatrixXd &reigenvector);

	// Generated message map functions
protected:

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ONLINESVD_H__D92B51F8_A6D0_490B_B4CE_2EDE3A67DF08__INCLUDED_)
