// OnlieSVD.cpp : implementation file
//

#include "pch.h"
#include "OnlineSVD.h"
#include "ap.h"
#include "linalg.h"
#include "mathfunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COnlineSVD

COnlineSVD::COnlineSVD()
{
	m_bInitialized = FALSE;
}

COnlineSVD::~COnlineSVD()
{
}

/////////////////////////////////////////////////////////////////////////////
// COnlineSVD message handlers

BOOL COnlineSVD::IsInitialized()
{
	return (m_bInitialized);
}

void COnlineSVD::InitPCADATA(MatrixXd &InputData, MatrixXd &eigenvalue, MatrixXd &leigenvector, MatrixXd &reigenvector)
{
	m_matPrevInputData = InputData;
	m_matPrevEigenvalue = eigenvalue; 
	m_matPrevLeigenvector = leigenvector/*U*/;
	m_matPrevReigenvector = reigenvector/*V*/;
	m_bInitialized = TRUE;
}

void COnlineSVD::OnlineSVD(MatrixXd &InputData, MatrixXd &OutputData, MatrixXd &eigenvalue, 
										   MatrixXd &leigenvector/*U*/, MatrixXd &reigenvector/*V*/)
{
	int nRow=InputData.rows();
	size_t nCol=InputData.cols();

//	int rank = m_matPrevInputData.cols()-1;

	int i, j;
	int changedcpos;
	MatrixXd Diff, y;

	// Finding modified column
	Diff = m_matPrevInputData - InputData;
	changedcpos = -1;
	y.resize(Diff.rows(), 1);
	for (i=0; i<Diff.rows(); i++)
	{
		for (j=0; j<Diff.cols(); j++)
		{
			if (Diff(i,j)!=0)
			{
				changedcpos = j;
				break;
			}
		}
		if (changedcpos>0) break;
	}

//	printf("changedcpos : %d\n", changedcpos);
	if (changedcpos<0)
	{
		eigenvalue = m_matPrevEigenvalue;
		leigenvector = m_matPrevLeigenvector;
		reigenvector = m_matPrevReigenvector;

		for (i = 0; i < nRow; i++) 
		{
			for (j=0; j<nCol; j++)
			{
				OutputData(i,j) = double(m_matPrevLeigenvector(i,j)) * double(m_matPrevEigenvalue(j,j)); 
			}
		}

		return;
	}
	else
	{ // changes are made on a certain column
		for (i=0; i<InputData.rows(); i++)
		{
			y(i,0) = InputData(i,changedcpos);
		}
	}

	MatrixXd m, n, p, q, a, b, tmp, P, Q;
	double ip, iq;

	// calculating n and Q
	b.resize(Diff.cols(), 1);
	b.setZero();
	b(changedcpos, 0) = 1;

	n = m_matPrevReigenvector.transpose() * b;
	q = b - m_matPrevReigenvector*n;
	tmp = q.transpose()*q;
	iq = sqrt(tmp(0,0));
	Q = q;
	for (i=0; i<Q.rows(); i++)
	{
		for (j=0; j<Q.cols(); j++)
		{
			if (iq==0)
				Q(i,j) = 0;
			else
				Q(i,j) = q(i,j) / iq;
		}
	}

	// Calculating m and P
	MatrixXd meigenvalue = m_matPrevEigenvalue;
	a = y - m_matPrevLeigenvector*meigenvalue*n;
	m = m_matPrevLeigenvector.transpose()*y - meigenvalue*n;
	p = y - m_matPrevLeigenvector*(m_matPrevLeigenvector.transpose()*y);

	tmp = p.transpose()*p;
	ip = sqrt(tmp(0,0));
	P = p;
	for (i=0; i<P.rows(); i++)
	{
		for (j=0; j<P.cols(); j++)
		{
			if (ip == 0)
				P(i,j) = 0;
			else
				P(i,j) = p(i,j) / ip;
		}
	}

	MatrixXd newm, newn, newmn, outmat;
	newm.resize(m.rows(),1);
	newn.resize(n.rows(),1);
	newm = m;
	newn = n;

	// adding new row to make it rank+1
	newm.conservativeResize(m.rows()+1,1);
	newn.conservativeResize(n.rows()+1,1);

	// add values to the new row
	newm(newm.rows()-1, 0) = ip;
	newn(newn.rows()-1, 0) = iq;

	newmn = newm * newn.transpose();

	meigenvalue.conservativeResizeLike(
		Eigen::MatrixXd::Zero(meigenvalue.rows() + 1, meigenvalue.cols() + 1)
	); //change to rank+1

	outmat = meigenvalue + newmn;

	size_t row, col;
	row = outmat.rows();
	col = outmat.cols();
	alglib::real_2d_array svdout;
    svdout.setlength(row, col);
    for(i = 0; i < row; i++)
    {
        for(j = 0; j < col; j++)
        {
            svdout(i,j) = outmat(i,j);
        }
    }
	
    alglib::real_2d_array u;
    alglib::real_2d_array vt;
    alglib::real_1d_array w;
    alglib::real_1d_array diag_w;

    //
    // Call SVDDecomposition subroutine
    //
    if( !rmatrixsvd(svdout, row, col, 2, 2, 2, w, u, vt) )
    {
        printf("SVD decomposition failed!\n");
//        return -1;
    }

	MatrixXd U_prime, U, U_2prime, V, V_prime, V_2prime;
	U_prime.resize(u.rows(), u.cols());
	V_prime.resize(vt.cols(), vt.rows());
    for(i = 0; i < U_prime.rows(); i++)
        for(j = 0; j < U_prime.cols(); j++)
            U_prime(i,j) = u(i,j);

    for(i = 0; i < V_prime.rows(); i++)
        for(j = 0; j < V_prime.cols(); j++)
			V_prime(i,j) = vt(j,i); // change V^T -> V

	U = m_matPrevLeigenvector;
	V = m_matPrevReigenvector;

	// add a new column to the existing matrix
	U.conservativeResizeLike(Eigen::MatrixXd::Zero(U.rows(), U.cols()+1));
	V.conservativeResizeLike(Eigen::MatrixXd::Zero(V.rows(), V.cols()+1));

	
	for (i=0; i<U.rows(); i++)
		U(i, U.cols()-1) = P(i,0);

	for (i=0; i<V.rows(); i++)
		V(i, V.cols()-1) = Q(i,0);


	U_2prime = U * U_prime;
	V_2prime = V * V_prime;

	MatrixXd S_2prime(meigenvalue.rows(), meigenvalue.cols());
	S_2prime.setZero();
	for (i=0; i<meigenvalue.cols(); i++)
		S_2prime(i, i) = w(i);

	// Reducing the dimension
	U_2prime.conservativeResize(U_2prime.rows(), U_2prime.cols()-1);
	V_2prime.conservativeResize(V_2prime.rows(), V_2prime.cols()-1);
	S_2prime.conservativeResize(S_2prime.rows()-1, S_2prime.cols()-1);

	
	for (i = 0; i < nRow; i++) 
	{
		for (j=0; j<nCol; j++)
		{
			OutputData(i,j) = double(U_2prime(i,j)) * double(S_2prime(j,j)); 
		}
	}

	m_matPrevInputData = InputData;
	m_matPrevEigenvalue = S_2prime;
	m_matPrevLeigenvector = U_2prime/*U*/;
	m_matPrevReigenvector = V_2prime/*V*/;

	eigenvalue = m_matPrevEigenvalue;
	leigenvector = U_2prime;
	reigenvector = V_2prime;

}
