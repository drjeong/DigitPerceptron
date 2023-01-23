#include "pch.h"
#include "DigitPattern.h"

#define DEBUG_SCREEN


std::size_t GenerateHash(CString& label)
{
	std::string s = stdext::cvt::wstring_convert<cvt_t>().to_bytes(label);
	std::size_t hash = std::hash<std::string>{}(s);
	return hash;
}

std::size_t GenerateHash(CString& label, std::vector<int>& vec)
{
	std::size_t seed = vec.size();
	for (auto& i : vec) {
		seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	std::string s = stdext::cvt::wstring_convert<cvt_t>().to_bytes(label);
	std::size_t hash_label = std::hash<std::string>()(s);
	return seed ^ (hash_label << 1);
}

CDigitPattern::CDigitPattern() :
	nDigit(-1),
	szPattern(_T("")) 
{
}

CDigitPattern::~CDigitPattern()
{
}

void CDigitPattern::GenerateDigitTargetPattern(int nOutputNode)
{
	vecTargetPattern.clear();

	// generate target pattern format based on the digit information.
	for (int k = 0; k < nOutputNode; k++) {
		if (nDigit == k)
			vecTargetPattern.push_back(1);
		else
			vecTargetPattern.push_back(0);
	}
}

void CDigitPattern::ApplyPreprocessing(MatrixXb	*pmtrxGridBlocks)
{
	int SmallX = -1, SmallY = -1;
	int LargeX = -1, LargeY = -1;
	CString temp, ctemp;
	MatrixXb mtrixTempBlocks, mtrixScaledBlocks;
	mtrixTempBlocks.resize(BLOCK_Y, BLOCK_X);
	mtrixScaledBlocks.resize(BLOCK_Y, BLOCK_X);

	// Initialize
	mtrixTempBlocks.setZero();
	mtrixScaledBlocks.setZero();


#ifdef DEBUG_SCREEN

	if (pmtrxGridBlocks) {
		// Backup the original grid block information
		for (int row = 0; row < BLOCK_Y; row++) {
			for (int column = 0; column < BLOCK_X; column++) {
				(*pmtrxGridBlocks)(row, column) = vecOriginalPattern[row * (int)(BLOCK_X)+column];
			}
		}
		AfxGetMainWnd()->Invalidate(FALSE);
		AfxMessageBox(_T("Current Pattern"));
	}
	
#endif

	// Finding the boundary of the digit.
	// It determines the region.
	for (int row = 0; row < BLOCK_Y; row++) {	// row
		for (int column = 0; column < BLOCK_X; column++) {	// column
			int IdxPos = row * (int)(BLOCK_X)+column;
			if (vecOriginalPattern[IdxPos] == TRUE) {
				if (SmallX == -1 && SmallY == -1) // default initialize
				{
					SmallX = column; SmallY = row; LargeX = column; LargeY = row;
				}
				else {
					if (SmallX > column) SmallX = column;
					if (SmallY > row) SmallY = row;
					if (LargeX < column) LargeX = column;
					if (LargeY < row) LargeY = row;
				}
			}
		}
	}

#ifdef DEBUG_SCREEN
	if (pmtrxGridBlocks) {
		temp.Format(_T("SmallX : %d, SmallY: %d, LargeX : %d, LargeY : %d"),
			SmallX, SmallY, LargeX, LargeY);
		AfxGetMainWnd()->Invalidate(FALSE);
		AfxMessageBox(temp);
	}
#endif

	// Move Pattern to (0,0) coordinate
	for (int row = SmallY; row <= LargeY; row++) {
		for (int column = SmallX; column <= LargeX; column++) {
			int y_newPos = row - SmallY;
			int x_newPos = column - SmallX;

			if (x_newPos >= BLOCK_X) x_newPos = BLOCK_X - 1;
			if (x_newPos < 0) x_newPos = 0;
			if (y_newPos >= BLOCK_Y) y_newPos = BLOCK_Y - 1;
			if (y_newPos < 0) y_newPos = 0;

			int IdxPos = row * (int)(BLOCK_X) + column;
			mtrixTempBlocks(y_newPos, x_newPos) = vecOriginalPattern[IdxPos];
		}
	}

#ifdef DEBUG_SCREEN
	if (pmtrxGridBlocks) {
		// Update the drawing
		for (int row = 0; row < BLOCK_Y; row++) {
			for (int column = 0; column < BLOCK_X; column++) {
				(*pmtrxGridBlocks)(row, column) = mtrixTempBlocks(row, column);
			}
		}

		AfxGetMainWnd()->Invalidate(FALSE);
		AfxMessageBox(_T("Pattern is moved to 0.0"));
	}
#endif


	//////////////////////// Scaling ////////////////////////////////////
	double ratio = 0;
	double lvalue = 0; //remainder
	int sourcePos = 0, targetPos = 0;

	// Scaling horizontally
	ratio = BLOCK_X / (double)(LargeX - SmallX + 1);
	for (int row = 0; row < BLOCK_Y; row++) {
		targetPos = 0; sourcePos = 0;
		for (int column = 0; column < (LargeX - SmallX + 1); column++) {
			for (int z = 1; z <= ratio; z++) {
				mtrixScaledBlocks(row, targetPos++) = mtrixTempBlocks(row, sourcePos);
				if (targetPos >= BLOCK_X) targetPos = BLOCK_X - 1;
				lvalue += (ratio - (int)ratio);
			}
			if (lvalue > 1) {
				mtrixScaledBlocks(row, targetPos++) = mtrixTempBlocks(row, sourcePos);
				if (targetPos >= BLOCK_X) targetPos = BLOCK_X - 1;
				lvalue = (lvalue - (int)lvalue);
			}
			sourcePos++;
			if (sourcePos >= BLOCK_X) sourcePos = BLOCK_X - 1;
		}
	}


#ifdef DEBUG_SCREEN
	if (pmtrxGridBlocks) {
		for (int row = 0; row < BLOCK_Y; row++)
			for (int column = 0; column < BLOCK_X; column++)
				(*pmtrxGridBlocks)(row, column) = mtrixScaledBlocks(row, column);

		AfxGetMainWnd()->Invalidate(FALSE);
		AfxMessageBox(_T("X Scaled Pattern"));
	}
	//return;
#endif

	for (int row = 0; row < BLOCK_Y; row++)		// Copy Pattern to TempBlock
		for (int column = 0; column < BLOCK_X; column++)
			mtrixTempBlocks(row, column) = mtrixScaledBlocks(row, column);

	// Scaling vertically
	targetPos = 0; sourcePos = 0; lvalue = 0;
	ratio = BLOCK_Y / (float)(LargeY - SmallY + 1);
	for (int column = 0; column < BLOCK_X; column++) {		// Scaling via Y
		targetPos = 0; sourcePos = 0;
		for (int row = 0; row < (LargeY - SmallY + 1); row++) {
			for (int z = 1; z <= ratio; z++) {
				mtrixScaledBlocks(targetPos++, column) = mtrixTempBlocks(sourcePos, column);
				if (sourcePos >= BLOCK_Y) sourcePos = BLOCK_Y - 1;
				if (targetPos >= BLOCK_Y) targetPos = BLOCK_Y - 1;
			}
			lvalue += (ratio - (int)ratio);
			if (lvalue > 1) {
				mtrixScaledBlocks(targetPos++, column) = mtrixTempBlocks(sourcePos, column);
				lvalue = (lvalue - (int)lvalue);
			}
			sourcePos++;
			if (sourcePos >= BLOCK_Y) sourcePos = BLOCK_Y - 1;
			if (targetPos >= BLOCK_Y) targetPos = BLOCK_Y - 1;
		}
	}

#ifdef DEBUG_SCREEN
	if (pmtrxGridBlocks) {
		for (int row = 0; row < BLOCK_Y; row++)
			for (int column = 0; column < BLOCK_X; column++)
				(*pmtrxGridBlocks)(row, column) = mtrixScaledBlocks(row, column);

		AfxGetMainWnd()->Invalidate(FALSE);
		AfxMessageBox(_T("Y Scaled Pattern"));
	}
#endif

	// duplicate patterns
	vecPreprocessedPattern.clear();
	for (int row = 0; row < BLOCK_Y; row++) {
		for (int column = 0; column < BLOCK_X; column++) {
			if (mtrixScaledBlocks(row, column))
				vecPreprocessedPattern.push_back(TRUE);
			else
				vecPreprocessedPattern.push_back(FALSE);
		}
	}


#ifdef DEBUG_SCREEN
	if (pmtrxGridBlocks) {
		*pmtrxGridBlocks = mtrixScaledBlocks;

		AfxGetMainWnd()->Invalidate(FALSE);
		AfxMessageBox(_T("Mofied Pattern"));
	}
#endif
}

void CDigitPattern::ApplyPreprocessing2(MatrixXb* pmtrxGridBlocks)
{
	int SmallX = -1, SmallY = -1;
	int LargeX = -1, LargeY = -1;
	CString temp, ctemp;
	MatrixXi mtrixTempBlocks, mtrixScaledBlocks;
	int MNIST_BLOCK_SIZE = 28;

	mtrixTempBlocks.resize(MNIST_BLOCK_SIZE, MNIST_BLOCK_SIZE);
	mtrixScaledBlocks.resize(MNIST_BLOCK_SIZE, MNIST_BLOCK_SIZE);

	FILE* fp = NULL;

	// Initialize
	mtrixTempBlocks.setZero();
	mtrixScaledBlocks.setZero();

	// Finding the boundary of the digit.
	// It determines the region.
	for (int row = 0; row < MNIST_BLOCK_SIZE; row++) {	// row
		for (int column = 0; column < MNIST_BLOCK_SIZE; column++) {	// column
			int IdxPos = row * (int)(MNIST_BLOCK_SIZE)+column;
			if (vecOriginalPattern[IdxPos] > 0) {
				if (SmallX == -1 && SmallY == -1) // default initialize
				{
					SmallX = column; SmallY = row; LargeX = column; LargeY = row;
				}
				else {
					if (SmallX > column) SmallX = column;
					if (SmallY > row) SmallY = row;
					if (LargeX < column) LargeX = column;
					if (LargeY < row) LargeY = row;
				}
			}
		}
	}


	// Move Pattern to (0,0) coordinate
	for (int row = SmallY; row <= LargeY; row++) {
		for (int column = SmallX; column <= LargeX; column++) {
			int y_newPos = row - SmallY;
			int x_newPos = column - SmallX;

			if (x_newPos >= MNIST_BLOCK_SIZE) x_newPos = MNIST_BLOCK_SIZE - 1;
			if (x_newPos < 0) x_newPos = 0;
			if (y_newPos >= MNIST_BLOCK_SIZE) y_newPos = MNIST_BLOCK_SIZE - 1;
			if (y_newPos < 0) y_newPos = 0;

			int IdxPos = row * (int)(MNIST_BLOCK_SIZE)+column;
			mtrixTempBlocks(y_newPos, x_newPos) = vecOriginalPattern[IdxPos];
		}
	}

	//fp = NULL;
	//fopen_s(&fp, "process1.txt", "w+");
	//for (int row = 0; row < mtrixTempBlocks.rows(); row++) {
	//	for (int column = 0; column < mtrixTempBlocks.cols(); column++) {
	//		fprintf(fp, "%3d\t", mtrixTempBlocks(row, column));
	//	}
	//	fprintf(fp, "\n");
	//}
	//fclose(fp);


	//////////////////////// Scaling down ////////////////////////////////////
	double ratio = 0;
	double lvalue = 0; //remainder
	int sourcePos = 0, targetPos = 0;

	// Scaling down horizontally
	double ratioX = (double)(LargeX - SmallX + 1) / BLOCK_X;
	double ratioY = (float)(LargeY - SmallY + 1) / BLOCK_Y;
	for (int row = 0; row < BLOCK_Y; row++) {
		for (int col = 0; col < BLOCK_X; col++) {
			int val = 0;
			int item = 0;
			for (int sourceX = 0; sourceX < ratioX; sourceX++) {
				for (int sourceY = 0; sourceY < ratioY; sourceY++) {
					item++;
					val += mtrixTempBlocks(row * ratioY + sourceY,
						col * ratioX + sourceX);
				}
			}
			mtrixScaledBlocks(row, col) = val / (double)item;
		}
	}

	//fp = NULL;
	//fopen_s(&fp, "process2.txt", "w+");
	//for (int row = 0; row < mtrixScaledBlocks.rows(); row++) {
	//	for (int column = 0; column < mtrixScaledBlocks.cols(); column++) {
	//		fprintf(fp, "%.2d\t", mtrixScaledBlocks(row, column));
	//	}
	//	fprintf(fp, "\n");
	//}
	//fclose(fp);

	// duplicate patterns
	for (int row = 0; row < BLOCK_Y; row++) {
		for (int column = 0; column < BLOCK_X; column++) {
			if (mtrixScaledBlocks(row, column) > (256 / 4.))
				vecPreprocessedPattern.push_back(TRUE);
			else
				vecPreprocessedPattern.push_back(FALSE);
		}
	}

	//if (pmtrxGridBlocks) {
	//	*pmtrxGridBlocks = mtrixScaledBlocks;

	//	AfxGetMainWnd()->Invalidate(FALSE);
	//	AfxMessageBox(_T("Mofied Pattern"));
	//}
}



/*/
// old method
void CDigitPattern::ApplyPreprocessing(MatrixXb* pmtrxGridBlocks)
{
	int SmallX = -1, SmallY = -1;
	int LargeX = -1, LargeY = -1;
	CString temp, ctemp;
	MatrixXb mtrixTempBlocks, mtrixScaledBlocks;
	mtrixTempBlocks.resize(LBLOCK_Y, LBLOCK_X);
	mtrixScaledBlocks.resize(LBLOCK_Y, LBLOCK_X);

	// Initialize
	mtrixTempBlocks.setZero();
	mtrixScaledBlocks.setZero();


#ifdef DEBUG_SCREEN

	if (pmtrxGridBlocks) {
		// Backup the original grid block information
		for (int row = 0; row < BLOCK_Y; row++) {
			for (int column = 0; column < BLOCK_X; column++) {
				(*pmtrxGridBlocks)(row, column) = vecOriginalPattern[row * (int)(BLOCK_X)+column];
			}
		}
		AfxGetMainWnd()->Invalidate(FALSE);
		AfxMessageBox(_T("Current Pattern"));
	}

#endif

	// Finding the boundary of the digit.
	// It determines the region.
	for (int row = 0; row < BLOCK_Y; row++) {	// row
		for (int column = 0; column < BLOCK_X; column++) {	// column
			int IdxPos = row * (int)(BLOCK_X)+column;
			if (vecOriginalPattern[IdxPos] == TRUE) {
				if (SmallX == -1 && SmallY == -1) // default initialize
				{
					SmallX = column; SmallY = row; LargeX = column; LargeY = row;
				}
				else {
					if (SmallX > column) SmallX = column;
					if (SmallY > row) SmallY = row;
					if (LargeX < column) LargeX = column;
					if (LargeY < row) LargeY = row;
				}
			}
		}
	}

#ifdef DEBUG_SCREEN
	if (pmtrxGridBlocks) {
		temp.Format(_T("SmallX : %d, SmallY: %d, LargeX : %d, LargeY : %d"),
			SmallX, SmallY, LargeX, LargeY);
		AfxGetMainWnd()->Invalidate(FALSE);
		AfxMessageBox(temp);
	}
#endif

	// Move Pattern to (0,0) coordinate
	for (int row = SmallY; row <= LargeY; row++)
		for (int column = SmallX; column <= LargeX; column++) {
			int y_newPos = row - SmallY;
			int x_newPos = column - SmallX;

			if (x_newPos >= LBLOCK_X) x_newPos = LBLOCK_X - 1;
			if (x_newPos < 0) x_newPos = 0;
			if (y_newPos >= LBLOCK_Y) y_newPos = LBLOCK_Y - 1;
			if (y_newPos < 0) y_newPos = 0;

			int IdxPos = row * (int)(BLOCK_X)+column;
			mtrixTempBlocks(y_newPos, x_newPos) = vecOriginalPattern[IdxPos];
		}


#ifdef DEBUG_SCREEN
	if (pmtrxGridBlocks) {
		// Update the drawing
		for (int row = 0; row < BLOCK_Y; row++)
			for (int column = 0; column < BLOCK_X; column++) {
				(*pmtrxGridBlocks)(row, column) = mtrixTempBlocks(row, column);
			}

		AfxGetMainWnd()->Invalidate(FALSE);
		AfxMessageBox(_T("Pattern is moved to 0.0"));
	}
#endif


	//////////////////////// Scaling 8 x 12 -> 16 x 24 ////////////////////////////////////
	double ratio = 0;
	double lvalue = 0; //remainder
	int sourcePos = 0, targetPos = 0;

	// Scaling horizontally
	ratio = LBLOCK_X / (double)(LargeX - SmallX + 1);
	for (int row = 0; row < LBLOCK_Y; row++) {
		targetPos = 0; sourcePos = 0;
		for (int column = 0; column < (LargeX - SmallX + 1); column++) {
			for (int z = 1; z <= ratio; z++) {
				mtrixScaledBlocks(row, targetPos++) = mtrixTempBlocks(row, sourcePos);
				if (targetPos >= LBLOCK_X) targetPos = LBLOCK_X - 1;
				lvalue += (ratio - (int)ratio);
			}
			if (lvalue > 1) {
				mtrixScaledBlocks(row, targetPos++) = mtrixTempBlocks(row, sourcePos);
				if (targetPos >= LBLOCK_X) targetPos = LBLOCK_X - 1;
				lvalue = (lvalue - (int)lvalue);
			}
			sourcePos++;
			if (sourcePos >= LBLOCK_X) sourcePos = LBLOCK_X - 1;
		}
	}


#ifdef DEBUG_SCREEN
	if (pmtrxGridBlocks) {


		for (int row = 0; row < BLOCK_Y; row++)
			for (int column = 0; column < BLOCK_X; column++)
				(*pmtrxGridBlocks)(row, column) = mtrixScaledBlocks(row, column);

		AfxGetMainWnd()->Invalidate(FALSE);
		AfxMessageBox(_T("X Scaled Pattern"));
	}
	//return;
#endif

	for (int row = 0; row < LBLOCK_Y; row++)		// Copy Pattern to TempBlock
		for (int column = 0; column < LBLOCK_X; column++)
			mtrixTempBlocks(row, column) = mtrixScaledBlocks(row, column);

	// Scaling vertically
	targetPos = 0; sourcePos = 0; lvalue = 0;
	ratio = LBLOCK_Y / (float)(LargeY - SmallY + 1);
	for (int column = 0; column < LBLOCK_X; column++) {		// Scaling via Y
		targetPos = 0; sourcePos = 0;
		for (int row = 0; row < (LargeY - SmallY + 1); row++) {
			for (int z = 1; z <= ratio; z++) {
				mtrixScaledBlocks(targetPos++, column) = mtrixTempBlocks(sourcePos, column);
				if (sourcePos >= LBLOCK_Y) sourcePos = LBLOCK_Y - 1;
				if (targetPos >= LBLOCK_Y) targetPos = LBLOCK_Y - 1;
			}
			lvalue += (ratio - (int)ratio);
			if (lvalue > 1) {
				mtrixScaledBlocks(targetPos++, column) = mtrixTempBlocks(sourcePos, column);
				lvalue = (lvalue - (int)lvalue);
			}
			sourcePos++;
			if (sourcePos >= LBLOCK_Y) sourcePos = LBLOCK_Y - 1;
			if (targetPos >= LBLOCK_Y) targetPos = LBLOCK_Y - 1;
		}
	}

#ifdef DEBUG_SCREEN
	if (pmtrxGridBlocks) {

		//FILE* fp = NULL;
		//fopen_s(&fp, "process03.txt", "w+");
		//for (int row = 0; row < LBLOCK_Y; row++) {
		//	for (int column = 0; column < LBLOCK_X; column++) {
		//		if (mtrixScaledBlocks(row, column) == TRUE)
		//			fprintf(fp, "1 ");
		//		else
		//			fprintf(fp, "0 ");
		//	}
		//	fprintf(fp, "\n");
		//}
		//fclose(fp);


		for (int row = 0; row < BLOCK_Y; row++)
			for (int column = 0; column < BLOCK_X; column++)
				(*pmtrxGridBlocks)(row, column) = mtrixScaledBlocks(row, column);

		AfxGetMainWnd()->Invalidate(FALSE);
		AfxMessageBox(_T("Y Scaled Pattern"));
	}
#endif

	//SmallX = 0;	SmallY = 0;
	//LargeX = 0; LargeY = 0;
	//BOOL ValueBlocks[LBLOCK_Y][LBLOCK_X];
	//for (int row = 0; row < LBLOCK_Y; row++)		// Copy Pattern to TempBlock
	//	for (int column = 0; column < LBLOCK_X; column++) {
	//		mtrixTempBlocks(row, column) = mtrixScaledBlocks(row, column);
	//		ValueBlocks(row, column) = FALSE;
	//	}


	// duplicate patterns
	for (int i = 0; i < vecOriginalPattern.size(); i++) {
		vecPreprocessedPattern.push_back(vecOriginalPattern[i]);
	}

	/////////////////////////// Collect Feature ///////////////////////////////
	int AddValue = 0;
	int srow = 0, scolumn = 0;	// small grid 
	for (int row = 0; row < LBLOCK_Y; row += 2) {			// Formalize 4 column 4 pattern
		for (int column = 0; column < LBLOCK_X; column += 2) {

			int newIdxPos = srow * (int)(BLOCK_X)+scolumn;
			AddValue = (int)mtrixScaledBlocks(row, column) + (int)mtrixScaledBlocks(row, column + 1)
				+ (int)mtrixScaledBlocks(row + 1, column) + (int)mtrixScaledBlocks(row + 1, column + 1);
			if (AddValue > 1)
				vecPreprocessedPattern[newIdxPos] = TRUE;
			else
				vecPreprocessedPattern[newIdxPos] = FALSE;

			AddValue = 0;
			scolumn++;
		}
		scolumn = 0;
		srow++;
	}

#ifdef DEBUG_SCREEN
	if (pmtrxGridBlocks) {
		for (int row = 0; row < BLOCK_Y; row++) {
			for (int column = 0; column < BLOCK_X; column++) {
				(*pmtrxGridBlocks)(row, column) = vecPreprocessedPattern[row * (int)(BLOCK_X)+column];
			}
		}

		AfxGetMainWnd()->Invalidate(FALSE);
		AfxMessageBox(_T("Mofied Pattern"));
	}
#endif
}
//*/
