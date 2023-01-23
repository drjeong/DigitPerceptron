// FlippingControl.cpp : implementation file
//

#include "pch.h"
#include <PCA/PCASignFlipController.h>
#include <PCA/mathfunctions.h>
//#include "ap.h"


//#define _SIGN(a)    ((a >= 0) ? (1) : (-1))

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPCASignFlipController

CPCASignFlipController::CPCASignFlipController()
{
	m_bDimensionChanged = FALSE;
	m_pParent = NULL;
}

CPCASignFlipController::~CPCASignFlipController()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPCASignFlipController message handlers

void CPCASignFlipController::Initialize(int row, int col, BOOL *noflipping)
{
	m_nRow=row;
	m_nCol=col;
	m_bInit=FALSE;
	m_pbNoFlipping = noflipping;
}

void CPCASignFlipController::Update(size_t row, size_t col, BOOL *noflipping)
{
	m_nRow=row;
	m_nCol=col;
	m_bInit=FALSE;
	if (noflipping!=NULL) m_pbNoFlipping = noflipping;
}

MatrixXd subtract_otherfactors(int K, MatrixXd &InputData, 
							 MatrixXd &loads1, MatrixXd &loads2,
							 int mode, int factor)
{
	int i, j;
	MatrixXd newlv, newrv, newev;

	newlv.resize(loads1.rows(), loads1.cols()-1);
	newrv.resize(loads2.rows(), loads1.cols()-1);
	newev.resize(K, K);

	newlv.setZero();
	newrv.setZero();
	newev.setZero();

	for (i=0; i<newlv.rows(); i++)
	{
		int idx=0;
		for (j=0; j<loads1.cols(); j++)
		{
			if (j==factor) continue; // skip
			newlv(i, idx) = loads1(i, j);
			idx++;
		}		
	}

	for (i=0; i<newrv.rows(); i++)
	{
		int idx = 0;
		for (j=0; j<loads2.cols(); j++)
		{
			if (j==factor) continue; // skip
			newrv(i, idx) = loads2(i, j);
			idx++;
		}		
	}

//	for (i=0; i<K; i++)
//	{
//		newev(i, i) = eigenvalue(0, i);
//	}

	MatrixXd output;


	if (mode == 2) // left singular
	{
		output = InputData - newlv * newrv.transpose();
		//output = ~InputData;
	}
	else
	{
		output = (InputData - newlv * newrv.transpose()).transpose();
		//output = InputData;
	}


//	printout(newrv * ~newlv, "newrv * ~newlv");
//	printout(InputData, "InputData");
//	printout(loads1, "loads1");
//	printout(loads2, "loads2");
//	printout(output, "output");

	return output;	
}

void CPCASignFlipController::FlippingControl(MatrixXd &DataPCA, MatrixXd &eigenvalue, 
					   MatrixXd &leigenvector/*U*/, MatrixXd &reigenvectorT/*V^T*/)
{
	int i, j;
	int row = m_nRow;
	int col = m_nCol;

	// Initialize Data
	if (m_bInit == FALSE)
	{
		m_matPrevPCAData = DataPCA;
		m_matPrevEigenvalue = eigenvalue;
		m_matPrevLeigenvector = leigenvector;
		m_matPrevReigenvectorT = reigenvectorT;
		m_bInit = TRUE;
		return;
	}
	
	if (*m_pbNoFlipping==FALSE) return; // Flipping prevention disabled

	// Solution of Flipping problem when all data in the same dimension are changed
	for (i=0; i<col; i++)
	{
		//double dist1(0), dist2(0);
		//for (j = 0; j < row; j++)
		//	dist1 += m_matPrevLeigenvector(j, i) * leigenvector(j, i);
		//for (j = 0; j < col; j++)
		//	dist2 += m_matPrevReigenvectorT(i, j) * reigenvectorT(i, j);
		//cout << i << ": dis1: " << dist1 << " dis2: " << dist2 << endl;

		Eigen::VectorXd vec1 = m_matPrevReigenvectorT.row(i);
		Eigen::VectorXd vec2 = reigenvectorT.row(i);
		double out = vec1.dot(vec2); // checking the direction of two vectors
		
		if (out < 0.0)
		{ // sign flip is detected.
			if (i == 0)
				cout << endl;
			for (j=0; j<row; j++)
			{
				DataPCA(j, i) *= -1;
				leigenvector(j, i) *= -1;
			}
			for (j=0; j<col; j++)
			{
				reigenvectorT(i, j) *= -1;
			}
		}
	}

	// update for future reference
	m_matPrevPCAData = DataPCA;
	m_matPrevLeigenvector = leigenvector;
	m_matPrevReigenvectorT = reigenvectorT;
}



//void CPCASignFlipController::FlippingControl_SignFlip(MatrixXd &InputData, MatrixXd &OutputData, MatrixXd &eigenvalue, 
//										   MatrixXd &leigenvector, MatrixXd &reigenvector)
//{
//	int i, j, k, K;
//	MatrixXd Y, meigenvalue;
//	MatrixXd Sleft, Sright;
//	MatrixXd loads1, loads2;
//	Sleft.resize(1, m_nCol);
//	Sright.resize(1, m_nCol);
//	Sleft.setZero();
//	Sright.setZero();
//
////	printout(InputData, "InputData");
//
//	K = (m_nRow<m_nCol)?m_nRow:m_nCol;
//
//	meigenvalue.resize(m_nCol, m_nCol);
//	if (eigenvalue.rows() != eigenvalue.cols())
//	{
//		meigenvalue.setZero();
//		for (i=0; i<m_nCol; i++)
//			meigenvalue(i, i) = eigenvalue(0, i);
//	}
//	else
//	{
//		meigenvalue = eigenvalue;
//	}
//
//	loads1 = leigenvector*meigenvalue;
//	loads2 = reigenvector;
////	printout(loads2, "loads2");
////	printout(loads1, "loads1");
//
//	// Step 1
//	for (k=0; k<K; k++)
//	{
//		MatrixXd Uk(m_nRow, 1);
//		Uk.setZero();
//		for (j=0; j<m_nRow; j++)
//			Uk(j,0) = loads1(j, k);
////		printout(Uk, "Uk");
//
//		//if (Uk.isZero()==false) // not initialized
//		//	Uk = Uk /(Uk.transpose() * Uk);
////		printout(Uk, "Uk");
//
//		Y = subtract_otherfactors(K, InputData, loads1, loads2, 2, k);
////		printout(Y, 10, Y.cols());
//
//		for (i=0; i<m_nCol; i++)
//		{
//			MatrixXd Yj(m_nRow, 1);
//			Yj.setZero();
//
//			for (j=0; j<m_nRow; j++)
//				Yj(j,0) = Y(j, i);
//
//			printout(Uk, "Uk");
//			printout(Yj, "Yj");
//
//			MatrixXd value = Uk.transpose() * Yj;
////			printf("%f\n", value(0,0));
//
//			double x = _SIGN(value(0,0)) * ( value(0,0)*value(0,0) );
//			Sleft(0, k) += x;
//		}
////		printf("%f\n", Sleft(0, k));
//	}
////	printout(Sleft, "Sleft");
//
//	// Step 2
//	for (k=0; k<K; k++)
//	{
//		MatrixXd Vk;
//		Vk.resize(m_nCol, 1);
//		Vk.setZero();
//		for (j=0; j<m_nCol; j++)
//			Vk(j,0) = reigenvector(j, k);
//
//		//if (Vk.isZero()==false)
//		//	Vk = Vk /(Vk.transpose() * Vk);
//
//		Y = subtract_otherfactors(K, InputData, loads1, loads2, 1, k);
//
//		for (i=0; i<m_nRow; i++)
//		{
//			MatrixXd Yi;
//			Yi.resize(m_nCol, 1);
//			Yi.setZero();
//
//			for (j=0; j<m_nCol; j++)
//				Yi(j,0) = Y(j, i);
//
////			printout(Vk, m_nCol, 1);
////			printout(Yi, m_nCol, 1);
//
//			MatrixXd value = Vk.transpose() * Yi;
//
//			double x = _SIGN(value(0,0)) * ( value(0,0)*value(0,0) );
//			Sright(0, k) += x;
//		}
//	}
//
////	printout(Sright, "Sright");
//
//	// Step 3
//	MatrixXd mleigenvector, mreigenvector;
//	mleigenvector.resize(m_nRow, m_nCol);
//	mreigenvector.resize(m_nCol, m_nCol);
//	mleigenvector.setZero();
//	mreigenvector.setZero();
//
//	for (k=0; k<K; k++)
//	{
//		double left=Sleft(0,k);
//		double right=Sright(0,k);
//
//		if (left*right < 0)
//		{
//			if (abs(left)<abs(right))
//				left = -left;
//			else
//				right = -right;
//		}
//
//		for (j=0; j<m_nRow; j++)
//			mleigenvector(j,k) = _SIGN(left)*leigenvector(j, k);
//		for (j=0; j<m_nCol; j++)
//			mreigenvector(j,k) = _SIGN(right)*reigenvector(j, k);
//
////		printout(mleigenvector, "mleigenvector");
////		printout(mreigenvector, "mreigenvector");
//
//	}
//
////	printout(mleigenvector, "mleigenvector");
////	printout(meigenvalue, "meigenvalue");
//
//	for (i = 0; i < m_nRow; i++) 
//	{
//		for (j=0; j<m_nCol; j++)
//		{
//			OutputData(i,j) = double(mleigenvector(i,j)) * double(meigenvalue(j,j)); 
//		}
//	}
//	
////	printout(leigenvector, m_nRow, m_nCol);
//
////	printout(leigenvector, m_nRow, m_nCol);
////	printout(reigenvector, m_nCol, m_nCol);
//	
////	printout(mleigenvector, "mleigenvector");
////	printout(mreigenvector, "mreigenvector");
////	printout(OutputData, "OutputData");
//
//}
//
//void CPCASignFlipController::FlippingControlTest(MatrixXd &InputData, MatrixXd &OutputData, MatrixXd &eigenvalue, 
//										   MatrixXd &leigenvector, MatrixXd &reigenvector)
//{
//	int i, j;
//	
//	for (j=0; j<leigenvector.cols(); j++)	
//	{
//		MatrixXd U, data, output;
//		U.resize(leigenvector.rows(), 1);
//		data.resize(leigenvector.rows(), 1);
//
//		for (i = 0; i < leigenvector.rows(); i++)
//		{
//			U(i,0) = leigenvector(i,j);
//			data(i,0) = InputData(i,j);
//			//OutputData(i,j) = double(mleigenvector(i,j)) * double(meigenvalue(j,j)); 
//		}
//		output = U.transpose() * data;
//		printf("%.4f ", output(0,0));
////		printout(output, "output");
//	}
//	printf("\n");
//
//}
