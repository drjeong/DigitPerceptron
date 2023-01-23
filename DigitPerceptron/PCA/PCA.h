// PCA.h: interface for the CPCA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCA_H__EA2A364A_96BB_4A0B_9C5C_0F78BFC21C23__INCLUDED_)
#define AFX_PCA_H__EA2A364A_96BB_4A0B_9C5C_0F78BFC21C23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SIGN(a, b) ( (b) < 0 ? -fabs(a) : fabs(a) )

#define ANALYSIS_CORR	20390	// Analysis of correlations
#define ANALYSIS_VARCOV	20391	// Analysis of variances-covariances
#define ANALYSIS_SOSCP	20392	// Analysis of sums-of-squares-cross-products

#include <vector>
#include <map>
#include "DPoint.h"
#include <PCA/PCASignFlipController.h>
#include <PCA/OnlineSVD.h>
#include "ConvexHull2.h"

using namespace std;

class CPCA  
{ 
public:
	CPCA();
	virtual ~CPCA();

	// PCA computation method
	typedef enum { TRADITIONAL, MATRIXCOMPUTATION, ONLINESVD }PCACOMPUTATIONMETHOD;

public:
	void Reset();
	void InitOnlineSVD();
	void SetData(Eigen::MatrixXd* pInput);
	BOOL IsFeedFowardCalcEnabled() { return m_bFeedForwardCalc;	}

	void ChangePCASignFlip() { m_bNoPCASignFlip ^= TRUE; };
	BOOL GetPCASignFlip() { return m_bNoPCASignFlip; };

	void RunPCA();
	void UpdatePCAOutput(int x_axis, int y_axis, BOOL bTrail);
	void RemovePCAOutputTrail();
	void ResetPCAOutput();
	void RestoreInitialData(int rows, int cols);

	void FlipPCAOutputVertically();
	void FlipPCAOutputHorizontally();

	void RecalculatePCAMatricesByEigenValueChanges();
	void SetPCAComputationMethod(PCACOMPUTATIONMETHOD method);
	PCACOMPUTATIONMETHOD GetPCAComputationMethod() { return m_nPCAComputationMethod; }

	void ComputeMeanCenteredMatrix();
	void UpdateCovarianceMatrixByDataValueChange(int row, int col, double val);
	void UpdateCovarianceMatrixByEigenVectorChange(int row, int col, double val);
	
	CPCASignFlipController *GetFlippingCtrl() { return &m_FlippingCtrl; };
	//MatrixXd *GetRightSingularVector() { return &m_MultiMatrix; };
	MatrixXd &GetPCAOutput() { return m_PCAOutput; };
	MatrixXd &GetEigenValue() { return m_eigenvalue; };
	MatrixXd &GetRightEigenVectorT() { return m_reigenvectorT; };
	MatrixXd &GetLeftEigenVector() { return m_leigenvector; };
	//MatrixXd *GetCovMatrix() { return &m_mean_centered_matrix; };
	vector<vector<CDPoint>> *Get2DPCAOutputTrails() { return &m_2DPCAOutputVector; };
	vector<CDPoint>& Get2DPCAOutput() { return m_2DPCAOutputVector.back(); };
	// get min & max of PC values
	C2DMinMax &GetPCAMinMax(int PCCoord = -1) { return (PCCoord == -1) ? m_PCAMinMax : m_PCAMinMax_PC[PCCoord]; };
	BOOL IsPCACalculated() { return m_bPCACalculated; };
		
	MatrixXd CalcInputMatrix(MatrixXd &covmatrix);
	MatrixXd CalcInputMatrix(MatrixXd *pcovmatrix);

	//int ComputePCA(size_t row, size_t col, int option, MatrixXd &input, MatrixXd &output,
	//						MatrixXd &eigenvalue, MatrixXd &leigenvector, 
	//						MatrixXd &reigenvector, MatrixXd &covmatrix);
	int RunSVD();
	
private:
	PCACOMPUTATIONMETHOD m_nPCAComputationMethod;
	BOOL	m_bNoPCASignFlip;
	BOOL	m_bFeedForwardCalc;

	CPCASignFlipController m_FlippingCtrl;
	COnlineSVD m_OnlineSVD;

	MatrixXd m_MeanMatrix;

	BOOL m_bPCACalculated; // indicator if PCA calculation is performed

	MatrixXd *m_pNormalizedInputData; // Input data
	MatrixXd m_PCAOutput;		// PCA output
	MatrixXd m_reigenvectorT;	// for displaying
	MatrixXd m_leigenvector;	// for displaying
	MatrixXd m_eigenvalue;
	MatrixXd m_mean_centered_matrix;
	MatrixXd m_MultiMatrix; // right singular vector (previously computed)

	// storing principal components (for displaying trails)
	vector<vector<CDPoint>> m_2DPCAOutputVector;

	// Min & Max value of PCA output per PC dimension
	map<int/*PC index*/, C2DMinMax/*min & max*/> m_PCAMinMax_PC;

	// Min & Max value of PCA output
	C2DMinMax m_PCAMinMax;

	// ConvexHull
	vector<CConvexHull2> m_ClusterCovexHull;
	int m_DistanceMeasureForClustering;
	vector<int> m_ClusterCidx;
};

#endif // !defined(AFX_PCA_H__EA2A364A_96BB_4A0B_9C5C_0F78BFC21C23__INCLUDED_)
