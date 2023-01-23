// PCA.cpp: implementation of the CPCA class.
//
//////////////////////////////////////////////////////////////////////
#include "pch.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include <PCA/PCA.h>
#include <PCA/mathfunctions.h>
#include <Eigen3.3.7/SVD>

#include "ap.h"
#include "linalg.h"
//#include "OptionsDlg.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPCA::CPCA()
{
	m_reigenvectorT.resize(0, 0);	// for displaying
	m_leigenvector.resize(0, 0);	// for displaying
	m_eigenvalue.resize(0, 0);
	m_mean_centered_matrix.resize(0, 0);
	m_bPCACalculated = FALSE;
	m_bFeedForwardCalc = FALSE; // applied when eigenvalues are chagend by user
	m_nPCAComputationMethod = TRADITIONAL; // default
}

CPCA::~CPCA()
{
	Reset();
}

void CPCA::Reset()
{
	m_bNoPCASignFlip = TRUE;
	m_bPCACalculated = FALSE;
	m_MultiMatrix.resize(0, 0);
	m_PCAOutput.resize(0, 0);
	m_reigenvectorT.resize(0, 0);
	m_leigenvector.resize(0, 0);
	m_eigenvalue.resize(0, 0);
	m_mean_centered_matrix.resize(0, 0);
	m_2DPCAOutputVector.clear();
	m_PCAMinMax_PC.clear();
}

void CPCA::RestoreInitialData(int rows, int cols)
{
	m_MultiMatrix.resize(0, 0);
	m_PCAOutput.resize(0, 0);
	m_reigenvectorT.resize(0, 0);
	m_leigenvector.resize(0, 0);
	m_eigenvalue.resize(0, 0);
	m_mean_centered_matrix.resize(0, 0);
	m_2DPCAOutputVector.clear();
	m_FlippingCtrl.Update(rows, cols);
	ResetPCAOutput();

	ComputeMeanCenteredMatrix();
}

void CPCA::ResetPCAOutput()
{
	m_2DPCAOutputVector.clear();
}

void CPCA::SetPCAComputationMethod(PCACOMPUTATIONMETHOD method)
{
	m_nPCAComputationMethod = method; 

	if (m_nPCAComputationMethod == ONLINESVD)
		InitOnlineSVD();
}

void CPCA::SetData(Eigen::MatrixXd* pInput)
{
	m_pNormalizedInputData = pInput;
	m_FlippingCtrl.Initialize(m_pNormalizedInputData->rows(), // initialize PCA sign flip
		m_pNormalizedInputData->cols(), &m_bNoPCASignFlip);
}

MatrixXd CPCA::CalcInputMatrix(MatrixXd *pcovmatrix)
{
	size_t row = pcovmatrix->rows();
	size_t col = pcovmatrix->cols();
	MatrixXd input;
	input.resize(row, col);

	/* Center the column vectors. */
	for (size_t i = 0; i < row; i++)
		for (size_t j = 0; j < col; j++)
			input(i,j) = (*pcovmatrix)(i,j) + m_MeanMatrix(j, 0);

	return input;
}

MatrixXd CPCA::CalcInputMatrix(MatrixXd &covmatrix)
{
	return CalcInputMatrix(&covmatrix);
}

void CPCA::ComputeMeanCenteredMatrix()
{
	size_t rows = m_pNormalizedInputData->rows();
	size_t cols = m_pNormalizedInputData->cols();
	m_mean_centered_matrix.resize(rows, cols);

	/* Allocate storage for mean vector */
	m_MeanMatrix.resize(cols, 1);
	m_MeanMatrix.setZero();

	/* Determine mean of column vectors of input data matrix */
	for (int j = 0; j < cols; j++)
	{
		double ave = 0.0;
#pragma omp parallel for reduction (+:ave)
		for (int i = 0; i < rows; i++)
		{
			ave += (*m_pNormalizedInputData)(i,j);
		}
		m_MeanMatrix(j, 0) = ave / rows;
	}

	/* Center the column vectors. */
//#pragma omp parallel for collapse(2)
//	for (int i = 0; i < rows; i++)
//		for (int j = 0; j < cols; j++)
//			m_mean_centered_matrix(i,j) = ((*m_pNormalizedInputData)(i,j) - m_MeanMatrix(j, 0));
#pragma omp parallel for // collapse() is not working in vs 2019
	for (int ij = 0; ij < rows * cols; ij++)
	{
		int i = ij / cols; // finding ith row index
		int j = ij % cols; // finding jth column index
		m_mean_centered_matrix(i, j) = ((*m_pNormalizedInputData)(i, j) - m_MeanMatrix(j, 0));
	}
}

void CPCA::UpdateCovarianceMatrixByEigenVectorChange(int row, int col, double val)
{
	m_PCAOutput(row, col) = val; // update the changes into PCA output
	m_mean_centered_matrix = m_PCAOutput * m_reigenvectorT;
	(*m_pNormalizedInputData) = CalcInputMatrix(m_mean_centered_matrix);
}

void CPCA::UpdateCovarianceMatrixByDataValueChange(int row, int col, double val)
{
	(*m_pNormalizedInputData)(row, col) = val; // update the changed value
	
	size_t rows = m_pNormalizedInputData->rows();

	// Determine mean of column vectors of updated column of the input data matrix
	double tmp = 0;
	for (size_t i = 0; i < rows; i++)
		tmp += (*m_pNormalizedInputData)(i, col); // total sum
	m_MeanMatrix(col, 0) = tmp / rows;

	/* Center the column vectors. */
	for (size_t i = 0; i < rows; i++)
		m_mean_centered_matrix(i, col) = ((*m_pNormalizedInputData)(i, col) - m_MeanMatrix(col, 0));
}

int CPCA::RunSVD()
{
	VERIFY(!m_mean_centered_matrix.isZero());
	if (m_mean_centered_matrix.isZero()) return -1;
	
	TIMERSTART(BDCSVD);
	Eigen::BDCSVD<MatrixXd> svd(m_mean_centered_matrix, Eigen::ComputeThinU | Eigen::ComputeThinV);
	MatrixXd tmp = svd.singularValues();
	m_eigenvalue.resize(tmp.rows(), tmp.rows());
	m_eigenvalue.setZero();
	for (int i = 0; i < tmp.rows(); i++)
		m_eigenvalue(i, i) = tmp(i);
	m_leigenvector = svd.matrixU();
	m_reigenvectorT = svd.matrixV().transpose();
	m_PCAOutput = m_leigenvector * m_eigenvalue;
	TIMERSTOP(BDCSVD);

	return 1;
}

void CPCA::InitOnlineSVD()
{
	m_MultiMatrix = m_reigenvectorT.transpose();
	
	m_OnlineSVD.InitPCADATA(m_mean_centered_matrix, m_eigenvalue, 
		m_leigenvector, m_MultiMatrix);

	m_FlippingCtrl.FlippingControl(m_PCAOutput, m_eigenvalue, m_leigenvector, m_reigenvectorT);
}

void CPCA::RunPCA()
{
	switch (m_nPCAComputationMethod)
	{
	case TRADITIONAL: // PCA calculation
		
		if (RunSVD())
		{
			m_FlippingCtrl.FlippingControl(m_PCAOutput, m_eigenvalue, m_leigenvector, m_reigenvectorT);
			
			// backing up the multiplication matrix to be used in MATRIXCOMPUTATION
			m_MultiMatrix = m_reigenvectorT.transpose(); 
		}
		break;

	case MATRIXCOMPUTATION:	// MatrixXd multiplication
		if (m_MultiMatrix.isZero())
		{ // if no stored multiplication matrix, run SVD again
			if (RunSVD())
				m_MultiMatrix = m_reigenvectorT.transpose();
		}
		m_PCAOutput = m_mean_centered_matrix * m_MultiMatrix;
		break;

	case ONLINESVD:	// online SVD
		{
			MatrixXd reigenvector = m_reigenvectorT.transpose();

			m_OnlineSVD.OnlineSVD(m_mean_centered_matrix, m_PCAOutput, m_eigenvalue, m_leigenvector, reigenvector);

			m_reigenvectorT = reigenvector.transpose();
			
			m_FlippingCtrl.FlippingControl(m_PCAOutput, m_eigenvalue, m_leigenvector, m_reigenvectorT);

			// backing up the multiplication matrix
			m_MultiMatrix = m_reigenvectorT.transpose();
		}
	break;
	}

}

// principal components (x & y-axis) are changed
void CPCA::UpdatePCAOutput(int x_axis, int y_axis, BOOL bTrail)
{
	if (bTrail == FALSE)
		ResetPCAOutput(); // remove previously stored PCA output

	// allocating PCA output to list vector
	vector<CDPoint> tmpmap;
	for (int row = 0; row < m_PCAOutput.rows(); row++) {
		CDPoint pt;
		pt.x = m_PCAOutput(row, x_axis);
		pt.y = m_PCAOutput(row, y_axis);
		tmpmap.push_back(pt); // save pca output to the visible instance (row key)
	}
	m_2DPCAOutputVector.push_back(tmpmap);

	// if having more than 20 trails, remove the oldest one.
	while (m_2DPCAOutputVector.size()>20)
	{
		m_2DPCAOutputVector.erase(m_2DPCAOutputVector.begin());
	}

	// Determining min & max of PCA output
	//	if (m_PCAMinMax_PC.size()==0)
	{
		m_PCAMinMax_PC.clear();
		m_PCAMinMax.Null(); // set to null

		for (int row = 0; row < m_PCAOutput.rows(); row++)
		{
			for (int col = 0; col < m_PCAOutput.cols(); col++)
			{
				double val = m_PCAOutput(row, col);

				if (row == 0) // 1st row
				{// add default values
					m_PCAMinMax_PC[col] = C2DMinMax(val, val);
				}
				else
				{
					if (m_PCAMinMax_PC[col].GetMin() > val) m_PCAMinMax_PC[col].SetMin(val);
					if (m_PCAMinMax_PC[col].GetMax() < val) m_PCAMinMax_PC[col].SetMax(val);
				}

				if (m_PCAMinMax.IsEmpty())
				{
					m_PCAMinMax = C2DMinMax(val, val);
				}
				else
				{
					if (m_PCAMinMax.GetMin() > val) m_PCAMinMax.SetMin(val);
					if (m_PCAMinMax.GetMax() < val) m_PCAMinMax.SetMax(val);
				}
			}
		}
	}

	//CalcIndividualItemUncertainty();

	m_bPCACalculated = TRUE;
}


void CPCA::RemovePCAOutputTrail()
{
	// remove all trails
	while (m_2DPCAOutputVector.size()>1)
	{
		auto it = m_2DPCAOutputVector.begin();
		m_2DPCAOutputVector.erase(it);
	}
}

void CPCA::FlipPCAOutputVertically()
{
	if (m_bPCACalculated == FALSE) return;
	for (auto it = m_2DPCAOutputVector.begin(); it != m_2DPCAOutputVector.end(); ++it)
	{
		vector<CDPoint> *pData = &(*it);		
		for (int i = 0; i < pData->size(); i++)
		{
			CDPoint pt = (*pData)[i];
			pt.y *= -1.0; // flip horizontally
			(*pData)[i] = pt;
		}
	}
}

void CPCA::FlipPCAOutputHorizontally()
{
	if (m_bPCACalculated == FALSE) return;
	for (auto it = m_2DPCAOutputVector.begin(); it != m_2DPCAOutputVector.end(); ++it)
	{
		vector<CDPoint> *pData = &(*it);
		for (int i = 0; i < pData->size(); i++)
		{
			CDPoint pt = (*pData)[i];
			pt.x *= -1.0; // flip horizontally
			(*pData)[i] = pt;
		}
	}
}

void CPCA::RecalculatePCAMatricesByEigenValueChanges()
{
	m_PCAOutput = m_leigenvector * m_eigenvalue;
	m_mean_centered_matrix = m_PCAOutput * m_reigenvectorT;
	(*m_pNormalizedInputData) = CalcInputMatrix(m_mean_centered_matrix);
}




//
//int CPCA::ComputePCA(size_t row, size_t col, int option, MatrixXd &input, MatrixXd &output,
//					MatrixXd &eigenvalue, MatrixXd &leigenvector, MatrixXd &reigenvector, MatrixXd &covmatrix)
//{
////			m_pca.ComputePCA(pDataPCA->row, pDataPCA->col, ANALYSIS_VARCOV, m_pInputData->data, pDataPCA->data,
////				m_eigenvalue, m_leigenvector, m_reigenvectorT, m_mean_centered_matrix);
//
//	Reset();
//
//	covmatrix= CalcCOVMatrix(input);
//
//    alglib::real_2d_array covout;
//    covout.setlength(row, col);
//
//    for(size_t i = 0; i < row; i++)
//    {
//        for(size_t j = 0; j < col; j++)
//        {
//            covout(i,j) = covmatrix(i,j);
//        }
//    }
//
//
//    alglib::real_2d_array u;
//    alglib::real_2d_array vt;
//    alglib::real_1d_array w;
//    alglib::real_1d_array diag_w;
//
//    //
//    // Call SVDDecomposition subroutine
//    //
//    if( !alglib::rmatrixsvd(covout, row, col, 2, 2, 2, w, u, vt) )
//    {
//        printf("SVD decomposition failed!\n");
//        return -1;
//    }
//
//
//	leigenvector.resize(row, row);
//	for (size_t i = 0; i < row; i++)
//		for (size_t j = 0; j < row; j++) 
//			leigenvector(i, j) = u(i,j);
//
//	for (size_t i = 0; i < row; i++)
//		for (size_t j = 0; j < col; j++) 
//			output(i,j) = double(u(i,j)) * double(w(j)); 
//
//
//	eigenvalue.resize(row, col);
//	eigenvalue.setZero();
//	reigenvector.resize(col, col);
//
//	for (size_t i = 0; i < col; i++) 
//	{
//		eigenvalue(i, i) = w(i);
//		for (size_t j = 0; j < col; j++) 
//		{
//			reigenvector(i, j) = vt(i, j);
//		}
//	}
//
//	return 1;
//}
