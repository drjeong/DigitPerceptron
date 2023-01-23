#include "pch.h"

#include <PCA/mathfunctions.h>
#include <sstream>



void calc_distmatrix(double **data, int row, int col, double **distmatrix)
{
	int i1, i2, j;

	for (i1 = 1; i1 <= row; i1++)
	{
		for (i2 = i1; i2 <= row; i2++)
		{
			if (i1 == i2)
			{
				distmatrix[i2][i1] = distmatrix[i1][i2] = 0;
				continue;
			}

			double total=0;
			for (j = 1; j <= col; j++)
			{
				total += ((data[i1][j] - data[i2][j])*(data[i1][j] - data[i2][j]));
			}
			distmatrix[i1][i2] = sqrt(total);
			distmatrix[i2][i1] = distmatrix[i1][i2];
		}
	}
}

/**  Correlation matrix: creation  ***********************************/
//void corcol(alglib::real_2d_array &data, int row, int col, alglib::real_2d_array &symmatrix)
//{	/* Create col * col correlation matrix from given row * col data matrix. */
//	double eps = (double)0.005;
//	double x, *mean, *stddev;
//	int i, j, j1, j2;
//
//	/* Allocate storage for mean and std. dev. vectors */
//
//	mean = alloc_vector(col);
//	stddev = alloc_vector(col);
//
//	/* Determine mean of column vectors of input data matrix */
//
//	for (j = 1; j <= col; j++)
//		{
//		mean[j] = 0.0;
//		for (i = 1; i <= row; i++)
//			{
//			mean[j] += data(i,j);
//			}
//		mean[j] /= (double)row;
//		}
//
//	printf("\nMeans of column vectors:\n");
//	for (j = 1; j <= col; j++)  {
//		printf("%7.1f",mean[j]);  }   printf("\n");
//
//	/* Determine standard deviations of column vectors of data matrix. */
//
//	for (j = 1; j <= col; j++)
//		{
//		stddev[j] = 0.0;
//		for (i = 1; i <= row; i++)
//			{
//			stddev[j] += (   ( data(i,j) - mean[j] ) *
//							 ( data(i,j) - mean[j] )  );
//			}
//			stddev[j] /= (double)row;
//			stddev[j] = (double)sqrt(stddev[j]);
//			/* The following in an inelegant but usual way to handle
//			near-zero std. dev. values, which below would cause a zero-
//			divide. */
//			if (stddev[j] <= eps) stddev[j] = 1.0;
//		}
//
//	printf("\nStandard deviations of columns:\n");
//	for (j = 1; j <= col; j++) { printf("%7.1f", stddev[j]); }
//	printf("\n");
//
//	/* Center and reduce the column vectors. */
//
//	for (i = 1; i <= row; i++)
//		{
//		for (j = 1; j <= col; j++)
//			{
//			data(i,j) -= mean[j];
//			x = (double)sqrt((double)row);
//			x *= stddev[j];
//			data(i,j) /= x;
//			}
//		}
//
//	/* Calculate the col * col correlation matrix. */
//	for (j1 = 1; j1 <= col-1; j1++)
//		{
//		symmatrix(j1,j1) = 1.0;
//		for (j2 = j1+1; j2 <= col; j2++)
//			{
//			symmatrix(j1,j2) = 0.0;
//			for (i = 1; i <= row; i++)
//				{
//				symmatrix(j1,j2) += ( data(i,j1) * data(i,j2));
//				}
//			symmatrix(j2,j1) = symmatrix(j1,j2);
//			}
//		}
//		symmatrix(col,col) = 1.0;
//
//	return;
//}

/**  Variance-covariance matrix: creation  *****************************/
//void covcol(alglib::real_2d_array &data, int r, int c, alglib::real_2d_array &symmatrix)
///* Create col * col covariance matrix from given row * col data matrix. */
//{
//	double *mean;
//	int i, j, j1, j2;
//
//	/* Allocate storage for mean vector */
//	mean = alloc_vector(c);
//
//	/* Determine mean of column vectors of input data matrix */
//
//	for (j = 1; j <= c; j++)
//	{
//		mean[j] = 0.0;
//		for (i = 1; i <= r; i++)
//			{
//			mean[j] += data(i,j);
//			}
//		mean[j] /= (double)r;
//	}
//
////	printf("\nMeans of column vectors:\n");
////	for (j = 1; j <= c; j++)  {
////		printf("%7.1f",mean[j]);  }   printf("\n");
//
//	/* Center the column vectors. */
//	for (i = 1; i <= r; i++)
//		for (j = 1; j <= c; j++)
//			data(i,j) = (data(i,j) - mean[j]);
//
////	printout(data, r, c);
//
//	/* Calculate the c * c covariance matrix. */
//	for (j1 = 1; j1 <= c; j1++)
//	{
//		for (j2 = j1; j2 <= c; j2++)
//		{
//			symmatrix(j1,j2) = 0.0;
//			for (i = 1; i <= r; i++)
//			{
//				symmatrix(j1,j2) += data(i,j1) * data(i,j2);
//			}
//			symmatrix(j1,j2) /= (r-1);
//			symmatrix(j2,j1) = symmatrix(j1,j2);
//		}
//	}
////	printout(c, c, symmatrix);
//
//	return;
//}

/**  Sums-of-squares-and-cross-products matrix: creation  **************/
//void scpcol(alglib::real_2d_array &data, int row, int col, alglib::real_2d_array &symmatrix)
///* Create col * col sums-of-cross-products matrix from row * col data matrix. */
//{
//	int i, j1, j2;
//
//	/* Calculate the col * col sums-of-squares-and-cross-products matrix. */
//
//	for (j1 = 1; j1 <= col; j1++)
//		{
//		for (j2 = j1; j2 <= col; j2++)
//			{
//			symmatrix(j1,j2) = 0.0;
//			for (i = 1; i <= row; i++)
//				{
//				symmatrix(j1,j2) += data(i,j1) * data(i,j2);
//				}
//			symmatrix(j2,j1) = symmatrix(j1,j2);
//			}
//		}
//
//	return;
//}

/**  Allocation of vector storage  ***********************************/
double *alloc_vector(int row)
/* Allocates a double vector with range [1..row]. */
{
    double *v;

    v = (double *) malloc ((unsigned) row*sizeof(double));
    if (!v) erhand("Allocation failure in vector().");
    return v-1;

}
float *alloc_fvector(int row)
/* Allocates a double vector with range [1..row]. */
{
    float *v;

    v = (float *) malloc ((unsigned) row*sizeof(float));
    if (!v) erhand("Allocation failure in vector().");
    return v-1;

}
int *alloc_nvector(int row)
/* Allocates a double vector with range [1..row]. */
{
    int *v;

    v = (int *) malloc ((unsigned) row*sizeof(int));
    if (!v) erhand("Allocation failure in vector().");
    return v-1;

}

/**  Allocation of double matrix storage  *****************************/
double **alloc_matrix(int row, int col)
/* Allocate a double matrix with range [1..row][1..col]. */
{
    int i;
    double **mat;

    /* Allocate pointers to rows. */
    mat = (double **) malloc((unsigned) (row)*sizeof(double*));
    if (!mat) erhand("Allocation failure 1 in matrix().");
    mat -= 1;

    /* Allocate rows and set pointers to them. */
    for (i = 1; i <= row; i++)
	{
        mat[i] = (double *) malloc((unsigned) (col)*sizeof(double));
        if (!mat[i]) erhand("Allocation failure 2 in matrix().");
        mat[i] -= 1;
    }

     /* Return pointer to array of pointers to rows. */
     return mat;
}
float **alloc_fmatrix(int row, int col)
/* Allocate a double matrix with range [1..row][1..col]. */
{
    int i;
    float **mat;

    /* Allocate pointers to rows. */
    mat = (float **) malloc((unsigned) (row)*sizeof(float*));
    if (!mat) erhand("Allocation failure 1 in matrix().");
    mat -= 1;

    /* Allocate rows and set pointers to them. */
    for (i = 1; i <= row; i++)
	{
        mat[i] = (float *) malloc((unsigned) (col)*sizeof(float));
        if (!mat[i]) erhand("Allocation failure 2 in matrix().");
        mat[i] -= 1;
    }

     /* Return pointer to array of pointers to rows. */
     return mat;
}
int **alloc_nmatrix(int row, int col)
/* Allocate a double matrix with range [1..row][1..col]. */
{
    int i;
    int **mat;

    /* Allocate pointers to rows. */
    mat = (int **) malloc((unsigned) (row)*sizeof(int*));
    if (!mat) erhand("Allocation failure 1 in matrix().");
    mat -= 1;

    /* Allocate rows and set pointers to them. */
    for (i = 1; i <= row; i++)
	{
        mat[i] = (int *) malloc((unsigned) (col)*sizeof(int));
        if (!mat[i]) erhand("Allocation failure 2 in matrix().");
        mat[i] -= 1;
    }

     /* Return pointer to array of pointers to rows. */
     return mat;
}
/**  Deallocate vector storage  *********************************/
void free_vector(double *v, int row)
/* Free a double vector allocated by vector(). */
{
   free((char*) (v+1));
}
void free_vector(float *v, int row)
/* Free a double vector allocated by vector(). */
{
   free((char*) (v+1));
}
void free_vector(int *v, int row)
/* Free a double vector allocated by vector(). */
{
   free((char*) (v+1));
}

/**  Deallocate double matrix storage  ***************************/
void free_matrix(double **mat, int row, int col)
/* Free a double matrix allocated by matrix(). */
{
   int i;

   for (i = row; i >= 1; i--)
   {
       free ((char*) (mat[i]+1));
   }
   free ((char*) (mat+1));
}
void free_matrix(float **mat, int row, int col)
/* Free a double matrix allocated by matrix(). */
{
   int i;

   for (i = row; i >= 1; i--)
   {
       free ((char*) (mat[i]+1));
   }
   free ((char*) (mat+1));
}
void free_matrix(int **mat, int row, int col)
/* Free a double matrix allocated by matrix(). */
{
   int i;

   for (i = row; i >= 1; i--)
   {
       free ((char*) (mat[i]+1));
   }
   free ((char*) (mat+1));
}

/**  Error handler  **************************************************/
void erhand(char err_msg[])
/* Error handler */
{
    fprintf(stderr,"Run-time error:\n");
    fprintf(stderr,"%s\n", err_msg);
    fprintf(stderr,"Exiting to system.\n");
    exit(1);
}

void printout(double **data, int row, int col)
{
	if (col>5) col = 5;

	printf("\nPrinting Matrix.............\n");
     for (int i = 1; i <= row; i++)
	 {
       for (int j = 1; j <= col; j++)
          printf("%12.4f ", data[i][j]);
	   printf("\n");
	 }
}

//void printout(alglib::real_2d_array &data, int row, int col)
//{
//	if (col>5) col = 5;
//
//	printf("\nPrinting Matrix.............\n");
//     for (int i = 0; i < row; i++)
//	 {
//       for (int j = 0; j < col; j++)
//          printf("%12.4f ", data(i,j));
//	   printf("\n");
//	 }
//}

//void printout(alglib::complex_2d_array &data, int row, int col)
//{
//	printf("\nPrinting Matrix.............\n");
//     for (int i = 0; i < row; i++)
//	 {
//       for (int j = 0; j < col; j++)
//          printf("%12.4f ", data(i,j));
//	   printf("\n");
//	 }
//}

//void printout(alglib::integer_1d_array &data, char* title)
//{
//	printf("\nPrinting Matrix.............: %s \n", title);
//	for (int i = 0; i < data.length(); i++)
//	{
//		printf("%d ", data(i));
//	}
//	printf("\n");
//}


//void printout(alglib::real_1d_array &data, char* title)
//{
//	printf("\nPrinting Matrix.............: %s \n", title);
//    for (int i = 0; i < data.length(); i++)
//	{
//		printf("%12.4f ", data(i));
//	}
//    printf("\n");
//}

//void printout(alglib::real_2d_array &data, char* title)
//{
//	printf("\nPrinting Matrix.............: %s \n", title);
//    for (int i = 0; i < data.rows() ; i++) //row
//	{
//		for (int j = 0; j < data.cols(); j++)
//			printf("%12.4f ", data(i, j));
//		printf("\n");
//	}
//    printf("\n");
//}

void printout(MatrixXd& data, int row, int col)
{
	printf("\nPrinting Matrix.............\n");
     for (int i = 0; i < row; i++)
	 {
       for (int j = 0; j < col; j++)
          printf("%12.4f ", data(i,j));
	   printf("\n");
	 }
}

void printout(MatrixXd& data)
{
	printf("\nPrinting Matrix.............\n");
     for (size_t i = 0; i < data.rows(); i++)
	 {
       for (size_t j = 0; j < data.cols(); j++)
          printf("%12.4f ", data(i,j));
	   printf("\n");
	 }
}

void printout(MatrixXd& data, char* title)
{
	printf("\nPrinting Matrix.............: %s \n", title);
     for (size_t i = 0; i < data.rows(); i++)
	 {
       for (size_t j = 0; j < data.cols(); j++)
          printf("%6.3f ", data(i,j));
	   printf("\n");
	 }
}

//void fprintout(char* fname, Eigen::MatrixXi& data)
//{
//	FILE* fp = fopen(fname, "w+");
//	if (fp == NULL) {
//		printf("ERROR - File Creation Fail!!");
//		return;
//	}
//
//	for (size_t i = 0; i < data.rows(); i++)
//	{
//		for (size_t j = 0; j < data.cols(); j++)
//			fprintf(fp, "%3d ", data(i, j));
//		fprintf(fp, "\n");
//	}
//	fprintf(fp, "\n");
//	fclose(fp);
//}


//void fprintout(char* fname, MatrixXd& data)
//{
//	FILE *fp = fopen(fname, "w+");
//	if (fp == NULL) {
//		printf("ERROR - File Creation Fail!!");
//		return;
//	}
//
//	for (size_t i = 0; i < data.rows(); i++)
//	{
//		for (size_t j = 0; j < data.cols(); j++)
//			fprintf(fp, "%6.3f ", data(i, j));
//		fprintf(fp, "\n");
//	}
//	fprintf(fp, "\n");
//	fclose(fp);
//}

//void fprintout(char* fname, MatrixXd& data, char* title)
//{
//	FILE *fp = fopen(fname, "a+");
//	if (fp == NULL) {
//		printf("ERROR - File Creation Fail!!");
//		return;
//	}
//
//	fprintf(fp, "%s\n", title);
//	for (size_t i = 0; i < data.rows(); i++)
//	{
//		for (size_t j = 0; j < data.cols(); j++)
//			fprintf(fp, "%6.3f ", data(i, j));
//		fprintf(fp, "\n");
//	}
//	fprintf(fp, "\n");
//	fclose(fp);
//}

//void fprintout(char* fname, alglib::real_1d_array &data, int row)
//{
//	FILE *fp = fopen(fname, "w+");
//	if (fp == NULL) {
//		printf("ERROR - File Creation Fail!!");
//		return;
//	}
//
//	fprintf(fp, "\nPrinting Matrix.............\n");
//	for (int i = 0; i < row; i++)
//		fprintf(fp, "%6.3f ", data(i));
//	fprintf(fp, "\n");
//	fclose(fp);
//}

void fprintout(char* fname,double **data, int row, int col)
{
	FILE* fp = NULL;
	fopen_s(&fp, fname, "w+");
	if (fp == NULL) {
		printf("ERROR - File Creation Fail!!");
		return;
	}

	fprintf(fp, "\nPrinting Matrix.............\n");
     for (int i = 0; i < row; i++)
	 {
       for (int j = 0; j < col; j++)
          fprintf(fp, "%6.3f ", data[i][j]);
	   fprintf(fp, "\n");
	 }
	 fclose(fp);
}

//void fprintout(char* fname,alglib::real_2d_array &data, int row, int col)
//{
//	FILE *fp = fopen(fname, "w+");
//	if (fp == NULL) {
//		printf("ERROR - File Creation Fail!!");
//		return;
//	}
//
//	fprintf(fp, "\nPrinting Matrix.............\n");
//     for (int i = 0; i < row; i++)
//	 {
//       for (int j = 0; j < col; j++)
//          fprintf(fp, "%6.3f ", data(i,j));
//	   fprintf(fp, "\n");
//	 }
//	 fclose(fp);
//}

void fprintout(char* fname,float *data, int row)
{
	FILE* fp = NULL;
	fopen_s(&fp, fname, "w+");
	if (fp == NULL) {
		printf("ERROR - File Creation Fail!!");
		return;
	}

    for (int i = 0; i < row; i++)
	{
         fprintf(fp, "%d %6.3f\n", i, data[i]);
	}
	 fclose(fp);
}


   
//void PWM(double eps,double dta,int m,int n, alglib::real_2d_array &A, int *it,
//		 double *gamma, alglib::real_1d_array &X1)  
//{
//	int i,j,l;
//	double phi,s;
//	alglib::real_1d_array X0;
//	X0.setlength(n);
//	for (i=1; i<=n; i++) 
//		X0(i)=1.0/sqrt((float)i);
//
//	*it=-1; l=1;
//	while (*it==-1 && l<=m) 
//	{
//	*gamma=0.0;
//		for (i=1; i<=n; i++) 
//		{
//			X1(i)=0.0;
//			for (j=1; j<=n; j++) 
//				X1(i) += A(i,j)*X0(j);
//
//			if (fabs(X1(i))>fabs(*gamma))  
//				*gamma=X1(i);
//		}
//		
//		if (fabs(*gamma) < eps) 
//			*it=0;
//		else 
//		{
//			for (i=1; i<=n; i++)  
//				X1(i) /= *gamma;
//			phi=0.0;
//	
//			for (i=1; i<=n; i++)  
//			{
//				s=fabs(X1(i)-X0(i));
//				if (s>phi) phi=s;
//			}
//
//			if (phi<dta) 
//				*it=1;
//			else 
//			{
//				for (i=1; i<=n; i++) 
//					X0(i)=X1(i);
//				l++;
//			}
//		}
//	}
//} //PWM()



void power(MatrixXd& a, int n, MatrixXd& z, MatrixXd& q, double *lamsub)
{
    int i,j,k;
    double norm, lambda;
    
    for (i=0; i<100/*MAX*/; i++){
       for (j=0;j<n;j++) {
            z(j,0) = 0.0;
            for (k=0; k<n;k++) 
				z(j,0) += a(j,k)*q(k,0);
        }
      
        /* find the norm of vector z */
        norm = 0.0;
        for (j=0;j<n;j++) {
            norm = norm + z(j,0)*z(j,0);
        }
        norm = sqrt(norm);

        /* scale vector z by norm */
        for (j=0;j<n;j++) q(j,0) = z(j,0)/norm;
        lambda = 0.0;
        for (j=0;j<n;j++) lambda = lambda + q(j,0) * z(j,0);
    }
    *lamsub = lambda;
}

/* entrance routine from Matlab mex-function */
void powermethod(MatrixXd& x, int n, int mode, MatrixXd& y, double &w)
{
	MatrixXd a, b, q, z;
	a.resize(n, n);
	b.resize(n, n);

    double lambda,lambdamin;
    int i,j;

	/* fill in C-stroage matrix elements */
	for (i=0;i<n;i++) {
	    for(j=0;j<n;j++) {
	        a(i,j) = x(i,j);
	    }
	    printf("\n");
	}    
    
    /* initialize the iterated eigenvector and working vector*/
	z.resize(n, 1);
	q.resize(n, 1);
    for (i=0;i<n;i++) q(i,0) = 0.0;
    q(0,0) = 1.0;
    
    /* find the maximum eigenvale/vector of matrix a */
    power(a,n,z,q,&lambda);
    
    /* mode ==0 is for maximum eigenvalue/vector */
    if (mode == 0) {
        for (i=0;i<n;i++) {
            y(i,0) = q(i,0);
        }
       w = lambda;
    }
    /* else mode ~= 0, for minimum eigenvalue/vector */
    else {
//        b = (double **) malloc(n * sizeof(double *));
//	    if (b == NULL) {
//           printf("matrix a does not have enogh memory\n");
//           return;
//        }
//        b[0] = (double *) malloc(n  * n * sizeof(double));
//        if (b[0] == NULL) {
//            printf("matrix b does not have enogh memory\n");
//            return;
//        }    
//    	for (i=1;i<n;i++) b[i] = b[i-1]+n;
     
    for (i=0;i<n;i++) {
        for(j=0;j<n;j++) b(i,j) = -a(i,j);
    }
    for (i=0;i<n;i++) b(i,i)=lambda-a(i,i);
    
    /* perform one more time power iteration method */
    	for (i=0;i<n;i++) q(i,0) = 0.0;
        q(0,0) = 1.0;
            
    /* iterative method to find maximum eigenvalue */
    power(b,n,z,q,&lambdamin);
    
    for (i=0;i<n;i++) y(i,0)=-q(i,0);
    w = lambda - lambdamin;
    } 
}


////////////////////////////////////////////////////////////////////////
//alglib::complex complexdeterminantlu(const alglib::complex_2d_array& a,
//     const alglib::integer_1d_array& pivots,
//     int n)
//{
//    alglib::complex result;
//    int i;
//    int s;
//
//    result = 1;
//    s = 1;
//    for(i = 1; i <= n; i++)
//    {
//        result = result*a(i,i);
//        if( pivots(i)!=i )
//        {
//            s = -s;
//        }
//    }
//    result = result*s;
//    return result;
//}

//alglib::complex complexdeterminant(alglib::complex_2d_array a, int m, int n)
//{
//    alglib::complex result;
//    alglib::integer_1d_array pivots;
//
//    complexludecomposition(a, m, n, pivots);
//
////	for (int i=1; i<=n; i++)
////		printf("%f ", pivots(i));
//
//    result = complexdeterminantlu(a, pivots, n);
//    return result;
//}


//void complexludecomposition(alglib::complex_2d_array& a,
//     int m,
//     int n,
//     alglib::integer_1d_array& pivots)
//{
//    int i;
//    int j;
//    int jp;
//    alglib::complex_1d_array t1;
//    alglib::complex s;
//    int i_;
//
//    pivots.setlength(alglib::minint(m, n));
//    t1.setlength(alglib::maxint(m, n));
//    alglib::ap_error::make_assertion(m>=0&&n>=0);
//    
//    //
//    // Quick return if possible
//    //
//    if( m==0||n==0 )
//    {
//        return;
//    }
//    for(j = 1; j <= alglib::minint(m, n); j++)
//    {
//        
//        //
//        // Find pivot and test for singularity.
//        //
//        jp = j;
//        for(i = j+1; i <= m; i++)
//        {
//            if( alglib::abscomplex(a(i,j))>alglib::abscomplex(a(jp,j)) )
//            {
//                jp = i;
//            }
//        }
//        pivots(j) = jp;
//        if( a(jp,j)!=0 )
//        {
//            
//            //
//            //Apply the interchange to rows
//            //
//            if( jp!=j )
//            {
//                for(i_=1; i_<=n;i_++)
//                {
//                    t1(i_) = a(j,i_);
//                }
//                for(i_=1; i_<=n;i_++)
//                {
//                    a(j,i_) = a(jp,i_);
//                }
//                for(i_=1; i_<=n;i_++)
//                {
//                    a(jp,i_) = t1(i_);
//                }
//            }
//            
//            //
//            //Compute elements J+1:M of J-th column.
//            //
//            if( j<m )
//            {
//                jp = j+1;
//                s = 1/a(j,j);
//                for(i_=jp; i_<=m;i_++)
//                {
//                    a(i_,j) = s*a(i_,j);
//                }
//            }
//        }
//        if( j<alglib::minint(m, n) )
//        {
//            
//            //
//            //Update trailing submatrix.
//            //
//            jp = j+1;
//            for(i = j+1; i <= m; i++)
//            {
//                s = a(i,j);
//                for(i_=jp; i_<=n;i_++)
//                {
//                    a(i,i_) = a(i,i_) - s*a(j,i_);
//                }
//            }
//        }
//    }
//}

bool checkStringIsNumber(std::string s) 
{
	//Eliminate obvious irritants that could spoil the party
	//Handle special cases here, e.g. return true for "+", "-", "" if they are acceptable as numbers to you
	if (s == "" || s == "." || s == "+" || s == "-" || s == "+." || s == "-.") return false;

	//Remove leading / trailing spaces **IF** they are acceptable to you
	while (s.size() > 0 && s[0] == ' ') s = s.substr(1, s.size() - 1);
	while (s.size() > 0 && s[s.size() - 1] == ' ') s = s.substr(0, s.size() - 1);


	//Remove any leading + or - sign
	if (s[0] == '+' || s[0] == '-')
		s = s.substr(1, s.size() - 1);

	//Remove decimal points
	long prevLength = s.size();

	size_t start_pos = 0;
	while ((start_pos = s.find(".", start_pos)) != std::string::npos)
		s.replace(start_pos, 1, "");

	//If the string had more than 2 decimal points, return false.
	if (prevLength > s.size() + 1) return false;

	//Check that you are left with numbers only!!
	//Courtesy selected answer by Charles Salvia above
	std::string::const_iterator it = s.begin();
	while (it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();

}

bool is_valid(std::string src) 
{
	std::stringstream ss;
	ss << src;
	double d = 0;
	ss >> d;
	if (ss) {
		return true;
	}
	else {
		return false;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Determining if a string value "qualifies" as a floating-point number? 
// Almost perfect solution.
// it check both numeric and floating values (ex: 70527E-06).
// but, it is somewhat slow.
bool isFloat(char* s) 
{
	if (*s == '-' || *s == '+') {
		if (!isdigit(*++s)) return false;
	}
	if (!isdigit(*s)) { return false; }
	while (isdigit(*s)) s++;
	if (*s == '.') {
		if (!isdigit(*++s)) return false;
	}
	while (isdigit(*s)) s++;
	if (*s == 'e' || *s == 'E') {
		s++;
		if (*s == '+' || *s == '-') {
			s++;
			if (!isdigit(*s)) return false;
		}
		else if (!isdigit(*s)) {
			return false;
		}
	}
	while (isdigit(*s)) s++;
	if (*s == '\0') return true;
	return false;
}


// it check both numeric and floating values (ex: 70527E-06).
// ex) 100.22.22 and 1_ratio is considered as float
bool IsFloat(const std::string& szstring)
{
	if (is_valid(szstring))
	{
		try
		{
			std::stof(szstring);
			return true;
		}
		catch(...)
		{
			return false;
		}
	}
	return false;
}

// it check both numeric and floating values (ex: 70527E-06).
// ex) 100.22.22 and 1_ratio is considered as float
bool IsFloat(const char* pszchar)
{
	if (is_valid(pszchar))
	{
		try
		{
			std::stof(pszchar);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	return false;
}

int ConvertChar2Double(const char* pszchar, double &value)
{
	value = 0; // default
	try
	{
		value = std::stod(pszchar);
	}
	catch (...)
	{
		return -1;
	}

	return 1;
}

// it only check numeric values (CAUTION: floating value (ex: 70527E-06) cannot be checked)
bool isNumeric( const char* pszchar, int nNumberBase )
{
	std::string base = "-.0123456789";
	std::string input = pszchar;

	return (::strspn(input.substr(0, nNumberBase).c_str(), base.c_str()) == input.length());
}

#define PI			3.14159265358
double TtoZ(double t, double df)
{
	double A9 = df - 0.5;
	double B9 = 48 * A9*A9;
	double T9 = t*t / df, Z8, P7, B7, z;

	if (T9 >= 0.04)
	{
		Z8 = A9*log(1 + T9);
	}
	else
	{
		Z8 = A9*(((1 - T9*0.75)*T9 / 3 - 0.5)*T9 + 1)*T9;
	}
	P7 = ((0.4*Z8 + 3.3)*Z8 + 24)*Z8 + 85.5;
	B7 = 0.8*pow(Z8, 2) + 100 + B9;
	z = (1 + (-P7 / B7 + Z8 + 3) / B9)*sqrt(Z8);
	return z;
}

double Norm_p(double z)
{
	double absz = abs(z);
	double a1 = 0.0000053830;
	double a2 = 0.0000488906;
	double a3 = 0.0000380036;
	double a4 = 0.0032776263;
	double a5 = 0.0211410061;
	double a6 = 0.0498673470;
	double p = (((((a1*absz + a2)*absz + a3)*absz + a4)*absz + a5)*absz + a6)*absz + 1;
	p = pow(p, -16);
	return p;
}

// Calculate P-value from t-value
double TtoP(double t, double df)
{
	double abst = abs(t), tsq = t*t, p;
	if (df == 1)
	{
		p = 1 - 2 * atan(abst) / PI;
	}
	else if (df == 2)
	{
		p = 1 - abst / sqrt(tsq + 2);
	}
	else if (df == 3)
	{
		p = 1 - 2 * (atan(abst / sqrt(3.0)) + abst*sqrt(3.0) / (tsq + 3)) / PI;
	}
	else if (df == 4)
	{
		p = 1 - abst*(1 + 2 / (tsq + 4)) / sqrt(tsq + 4);
	}
	else
	{
		double z = TtoZ(abst, df);
		if (df>4)
		{
			p = Norm_p(z);
		}
		else
		{
			p = Norm_p(z);
		}
	}
	return p;
}