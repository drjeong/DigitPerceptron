#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

//#include "ap.h"
#include <PCA/PCA.h>

void erhand(char err_msg[]);
void printout(double **data, int row, int col);
//void printout(alglib::real_2d_array &data, int row, int col);
//void printout(alglib::real_1d_array &data, char* title);
//void printout(alglib::integer_1d_array &data, char* title);
//void printout(alglib::real_2d_array &data, char* title);
//void printout(alglib::complex_2d_array &data, int row, int col);

//void printout(MatrixXd& data, int row, int col);
//void printout(MatrixXd& data);
//void printout(MatrixXd *data);
//void printout(MatrixXd& data, char* title);

void fprintout(char* fname,double **data, int row, int col);
void fprintout(char* fname,float *data, int row);
//void fprintout(char* fname,alglib::real_2d_array &data, int row, int col);
//void fprintout(char* fname, alglib::real_1d_array &data, int row);
//void fprintout(char* fname, MatrixXd& data);
//void fprintout(char* fname, Eigen::MatrixXi& data);
//void fprintout(char* fname, MatrixXd& data, char* title);

void calc_distmatrix(double **data, int row, int col, double **distmatrix);

//// Correlation matrix
//void corcol(alglib::real_2d_array &data, int row, int col, alglib::real_2d_array &symmatrix);
////  Variance-covariance matrix
//void covcol(alglib::real_2d_array &data, int r, int c, alglib::real_2d_array &symmatrix);
////  Sums-of-squares-and-cross-products matrix
//void scpcol(alglib::real_2d_array &data, int row, int col, alglib::real_2d_array &symmatrix);

//  Allocation of vector storage
double *alloc_vector(int row);
double **alloc_matrix(int row, int col);

float *alloc_fvector(int row);
float **alloc_fmatrix(int row, int col);

int *alloc_nvector(int row);
int **alloc_nmatrix(int row, int col);

void free_vector(double *v, int row);
void free_matrix(double **mat, int row, int col);

void free_vector(float *v, int row);
void free_matrix(float **mat, int row, int col);

void free_vector(int *v, int row);
void free_matrix(int **mat, int row, int col);

//void PWM(double eps,double dta,int m,int n, alglib::real_2d_array &A, int *it,
//		 double *gamma, alglib::real_1d_array &X1);
//void power(MatrixXd& a, int n, MatrixXd& z, MatrixXd& q, double *lamsub);
//void powermethod(MatrixXd& x, int n, int mode, MatrixXd& y, double &w);


//alglib::complex complexdeterminantlu(const alglib::complex_2d_array& a,
//     const alglib::integer_1d_array& pivots,
//     int n);
//alglib::complex complexdeterminant(alglib::complex_2d_array a, int m, int n);
//void complexludecomposition(alglib::complex_2d_array& a, int m, int n,
//     alglib::integer_1d_array& pivots);


bool isFloat(char* s);
bool IsFloat(const std::string& szstring);
bool IsFloat(const char* pszchar);
bool isNumeric( const char* pszchar, int nNumberBase );
int ConvertChar2Double(const char* pszchar, double &value);

// Calculate P-value from t-value
double TtoP(double t, double df);




