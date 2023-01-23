// DataManager.cpp : implementation file
//


#include "pch.h"
#include "DigitPerceptronDlg.h"
#include "DataManager.h"
#include <PCA/mathfunctions.h>
#include "StringWrapper.h"
#include "WndApi_Sys.h"
#include "DataFunctions.h"
#include <CSVParser.h>

#include <fstream>
#include <iostream>
#include <thread>
#include <omp.h>
//#include "ap.h"
//using namespace alglib;
#define IDC_CHECK_BUTTON_PCACOORD 20000

double randomdata() {
	return (rand() / 32000. - 0.5);
}

CDataManager::CDataManager()
{
	m_bDataLoaded = FALSE;
	m_glList_Rectangle = -1;
	m_glList_Grid = -1;
	m_nGlyphSizeRatio = 10;

	m_InteractionManagerList.push_back( new CInteractionManager(this) );
	m_InteractionManagerList.back()->Init();

	m_rgbColorMap = GetColorMap(10);
}

CDataManager::~CDataManager(void)
{
	while (m_InteractionManagerList.size()>0)
	{
		vector<CInteractionManager*>::iterator it=m_InteractionManagerList.begin();
		CInteractionManager *pIM= *it;
		delete pIM;
		m_InteractionManagerList.erase(it);
	}
	m_InteractionManagerList.clear();

	if (m_glList_Rectangle != -1)
		glDeleteLists(m_glList_Rectangle, 1);

	if (m_glList_Grid != -1)
		glDeleteLists(m_glList_Grid, 1);
}

void CDataManager::InitGLCallList()
{
	if (m_glList_Rectangle != -1) return;
	GLfloat len = 0.5f;

	m_glList_Grid = glGenLists(1);
	glNewList(m_glList_Grid, GL_COMPILE);

	//glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2d(-len, len);		// left top
	glVertex2d(-len, -len);		// left bottom
	glVertex2d(len, len);		// right top
	glVertex2d(len, -len);		// right bottom
	glEnd();

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2d(-len, len);		// left top
	glVertex2d(len, len);		// right top
	glVertex2d(len, -len);		// right bottom
	glVertex2d(-len, -len);		// left bottom
	glEnd();

	GLfloat gridWidth = (2 * len) / BLOCK_X;
	GLfloat gridHeight = (2 * len) / BLOCK_Y;

	GLfloat pos = -len;
	glBegin(GL_LINES);
	for (int r = 0; r < BLOCK_Y; r++) {
		glVertex2d(-len, pos);				// left
		glVertex2d(len, pos);	// right
		pos += gridHeight;
	}
	glEnd();

	pos = -len;
	glBegin(GL_LINES);
	for (int c = 0; c < BLOCK_X; c++) {
		glVertex2d(pos, len);				// top
		glVertex2d(pos, -len);	// bottom
		pos += gridWidth;
	}
	glEnd();
	glEndList();


	m_glList_Rectangle = glGenLists(1);
	glNewList(m_glList_Rectangle, GL_COMPILE);

	glBegin(GL_TRIANGLE_STRIP);
	glVertex2d(-len, len);		// left top
	glVertex2d(-len, -len);		// left bottom
	glVertex2d(len, len);		// right top
	glVertex2d(len, -len);		// right bottom
	glEnd();
	glEndList();

}

CGLColor CDataManager::GetRGBColor(int nDigit) {
	return m_rgbColorMap[nDigit];
}

vector<CGLColor> CDataManager::GetColorMap(int nNumClasses)
{
	vector<CGLColor> tmprgbcolor;
	if (nNumClasses >= 2 && nNumClasses <= 10)
	{
		switch (nNumClasses)
		{
		case 2:
			tmprgbcolor.push_back(CGLColor(228, 26, 28));
			tmprgbcolor.push_back(CGLColor(55, 126, 184));
			break;
		case 3:
			tmprgbcolor.push_back(CGLColor(228, 26, 28));
			tmprgbcolor.push_back(CGLColor(55, 126, 184));
			tmprgbcolor.push_back(CGLColor(77, 175, 74));
			break;
		case 4:
			tmprgbcolor.push_back(CGLColor(228, 26, 28));
			tmprgbcolor.push_back(CGLColor(55, 126, 184));
			tmprgbcolor.push_back(CGLColor(77, 175, 74));
			tmprgbcolor.push_back(CGLColor(152, 78, 163));
			break;
		case 5:
			tmprgbcolor.push_back(CGLColor(228, 26, 28));
			tmprgbcolor.push_back(CGLColor(55, 126, 184));
			tmprgbcolor.push_back(CGLColor(77, 175, 74));
			tmprgbcolor.push_back(CGLColor(152, 78, 163));
			tmprgbcolor.push_back(CGLColor(255, 127, 0));
			break;
		case 6:
			tmprgbcolor.push_back(CGLColor(228, 26, 28));
			tmprgbcolor.push_back(CGLColor(55, 126, 184));
			tmprgbcolor.push_back(CGLColor(77, 175, 74));
			tmprgbcolor.push_back(CGLColor(152, 78, 163));
			tmprgbcolor.push_back(CGLColor(255, 127, 0));
			tmprgbcolor.push_back(CGLColor(255, 255, 51));
			break;
		case 7:
			tmprgbcolor.push_back(CGLColor(228, 26, 28));
			tmprgbcolor.push_back(CGLColor(55, 126, 184));
			tmprgbcolor.push_back(CGLColor(77, 175, 74));
			tmprgbcolor.push_back(CGLColor(152, 78, 163));
			tmprgbcolor.push_back(CGLColor(255, 127, 0));
			tmprgbcolor.push_back(CGLColor(255, 255, 51));
			tmprgbcolor.push_back(CGLColor(166, 86, 40));
			break;
		case 8:
			tmprgbcolor.push_back(CGLColor(228, 26, 28));
			tmprgbcolor.push_back(CGLColor(55, 126, 184));
			tmprgbcolor.push_back(CGLColor(77, 175, 74));
			tmprgbcolor.push_back(CGLColor(152, 78, 163));
			tmprgbcolor.push_back(CGLColor(255, 127, 0));
			tmprgbcolor.push_back(CGLColor(255, 255, 51));
			tmprgbcolor.push_back(CGLColor(166, 86, 40));
			tmprgbcolor.push_back(CGLColor(247, 129, 191));
			break;
		case 9:
			tmprgbcolor.push_back(CGLColor(228, 26, 28));
			tmprgbcolor.push_back(CGLColor(55, 126, 184));
			tmprgbcolor.push_back(CGLColor(77, 175, 74));
			tmprgbcolor.push_back(CGLColor(152, 78, 163));
			tmprgbcolor.push_back(CGLColor(255, 127, 0));
			tmprgbcolor.push_back(CGLColor(255, 255, 51));
			tmprgbcolor.push_back(CGLColor(166, 86, 40));
			tmprgbcolor.push_back(CGLColor(247, 129, 191));
			tmprgbcolor.push_back(CGLColor(153, 153, 153));
			break;
		case 10:
			tmprgbcolor.push_back(CGLColor(166, 206, 227));
			tmprgbcolor.push_back(CGLColor(31, 120, 180));
			tmprgbcolor.push_back(CGLColor(178, 223, 138));
			tmprgbcolor.push_back(CGLColor(51, 160, 44));
			tmprgbcolor.push_back(CGLColor(251, 154, 153));
			tmprgbcolor.push_back(CGLColor(227, 26, 28));
			tmprgbcolor.push_back(CGLColor(253, 191, 111));
			tmprgbcolor.push_back(CGLColor(255, 127, 0));
			tmprgbcolor.push_back(CGLColor(202, 178, 214));
			tmprgbcolor.push_back(CGLColor(106, 61, 154));
			break;
		}
	}
	else
	{
		CColorSpace cspace;
		double d = 270. / nNumClasses; // degree has been limited to 270 degrees
		for (int i = 0; i < nNumClasses; i++)
		{
			int degree = (int)((double)i * d);
			cspace.SetHSV(degree, 200, 200);
			tmprgbcolor.push_back(cspace.GetGLColor());
		}
	}
	return tmprgbcolor;
}

void CDataManager::UpdatePCAOutputs() // principal components (x & y-axis) are changed
{
	m_pca.UpdatePCAOutput(GetInteractionManager()->GetPCAXCoord(),
		GetInteractionManager()->GetPCAYCoord(),
		GetInteractionManager()->IsTrailEnabled());

	//// Vertical Parallel Coordinate View needs to be recomputed
	//if (((CDigitPerceptronDlg*)m_pParent)->GetGLPCA())
	//	if (((CDigitPerceptronDlg*)m_pParent)->GetGLPCA()->GetGLVPCoord())
	//		if (((CDigitPerceptronDlg*)m_pParent)->GetGLPCA()->GetGLVPCoord()->IsWindowVisible())
	//		((CDigitPerceptronDlg*)m_pParent)->GetGLPCA()->GetGLVPCoord()->ResetParallelCoordinateBorder();
}

void CDataManager::ResetNeuralNetowrk(int nNumInputNode, int nNumHiddenNode, int nNumOutputNode)
{
	m_nNumInputNode = nNumInputNode;
	m_nNumHiddenNode = nNumHiddenNode;
	m_nNumOutputNode = nNumOutputNode;

	// Initialize neural network 
	m_dWeight_btwnInputHidden.clear();
	m_dWeight_btwnInputHiddenPrev.clear();
	for (int i = 0; i < m_nNumInputNode; i++) {
		vector<double> temp;
		for (int p = 0; p < m_nNumHiddenNode; p++) {
			temp.push_back(randomdata());	// Initialize Hidden weight
		}
		m_dWeight_btwnInputHidden.push_back(temp);
		m_dWeight_btwnInputHiddenPrev.push_back(temp);
	}

	m_dWeight_btwnHiddenOutput.clear();
	m_dWeight_btwnHiddenOutputPrev.clear();
	for (int p = 0; p < m_nNumHiddenNode; p++) {
		vector<double> temp;
		for (int q = 0; q < m_nNumOutputNode; q++) {
			temp.push_back(randomdata());	// Initialize Output weight
		}
		m_dWeight_btwnHiddenOutput.push_back(temp);
		m_dWeight_btwnHiddenOutputPrev.push_back(temp);
	}

	m_dHiddenLayerNodes.clear();
	for (int q = 0; q < m_nNumHiddenNode; q++) {
		m_dHiddenLayerNodes.push_back(0.0);	// Initialize node output
	}

	m_dOutputLayerNodes.clear();
	for (int k = 0; k < m_nNumOutputNode; k++) {
		m_dOutputLayerNodes.push_back(0.0);	// Initialize node output
	}

	m_mapPatterns.clear(); // remove all existing patterns
}

void CDataManager::Reset()
{
	//m_rgbColorMap.clear();
	m_pca.Reset();
	m_bDataLoaded = FALSE;

	if (((CDigitPerceptronDlg*)AfxGetMainWnd())->GetVisualRepresentation() != NULL)
	{
		if (((CDigitPerceptronDlg*)AfxGetMainWnd())->GetVisualRepresentation()->GetGLPCA() != NULL)
		{
			if (((CDigitPerceptronDlg*)AfxGetMainWnd())->GetVisualRepresentation()->GetGLPCA()->GetGLPCASpace() != NULL)
				((CDigitPerceptronDlg*)AfxGetMainWnd())->GetVisualRepresentation()->GetGLPCA()->GetGLPCASpace()->Reset();
		}
	}
	
	while (m_InteractionManagerList.size()>0)
	{
		vector<CInteractionManager*>::iterator it=m_InteractionManagerList.begin();
		CInteractionManager *pIM = (*it);
		delete pIM;
		m_InteractionManagerList.erase(it);
	}
	m_InteractionManagerList.clear();
	m_InteractionManagerList.push_back( new CInteractionManager(this) );

}

void CDataManager::CalcIndividualItemUncertainty()
{
// 	if (m_bShowUncertainty==FALSE)
// 	{
// 		if (m_pUncertntyDlg)
// 		{	// destroy window
// 			m_pUncertntyDlg->DestroyWindow();
// 			delete m_pUncertntyDlg;
// 			m_pUncertntyDlg = NULL;
// 		}
// 		return;
// 	}
// 
// 	if (m_nSelectedItems==0 || m_nSelectedItems>1)
// 	{
// 		AfxMessageBox(_T("Not support showing the uncertainties of more than a dataset"));
// 		m_bShowUncertainty = FALSE;
// 		((CButton*)GetDlgItem(IDC_CHECK_SHOWUNCERTAINTY))->SetCheck(m_bShowUncertainty);
// 	}
// 	else
// 	{
// 		if (m_pUncertntyDlg==NULL)
// 		{
// 			m_pUncertntyDlg = new CUncertnty;
// 			m_pUncertntyDlg->Create(this);
// 			this->SetFocus();
// 		}
// 		m_pUncertntyDlg->Reset();
// 
// 		for (int i=0; i<m_bSelectionInfo.size(); i++)
// 		{
// 			if (m_bSelectionInfo[i]==true)
// 			{
// 				m_pUncertntyDlg->CalculateConvexHull(i, m_nUncertaintyPercent/*percent*/);
// 			}
// 		}
// 	}
}

void CDataManager::RestoreInitialData()
{
	//m_dcNumerical_Display.Reset();
	//	
	//while (m_InteractionManagerList.size()>0)
	//{
	//	vector<CInteractionManager*>::iterator it=m_InteractionManagerList.begin();
	//	CInteractionManager *pIM = (*it);
	//	delete pIM;
	//	m_InteractionManagerList.erase(it);
	//}
	//m_InteractionManagerList.clear();
	//m_InteractionManagerList.push_back( new CInteractionManager(this) );
	//m_InteractionManagerList.back()->Init();

	//m_dcNumerical_Display = m_dcNumerical_Original;

	//m_pca.RestoreInitialData(m_dcNumerical_Display.Rows(), m_dcNumerical_Display.Cols());
	//ComputePCANUpdateOutput();
}

void CDataManager::SetInputDataRunPCA()
{
	if (m_mapPatterns.size() < 3) return;

	m_Data.resize(m_mapPatterns.size(), BLOCK_X * BLOCK_Y);

	int row = 0, col = 0;
	for (auto& item : m_mapPatterns) {
		CDigitPattern &pattern = item.second;

		col = 0;
		for (const auto& item2 : pattern.vecPreprocessedPattern) {
			m_Data(row, col) = item2;
			col++;
		}
		row++;
	}

	m_pca.SetData(&m_Data);
	m_pca.ComputeMeanCenteredMatrix();
	ComputePCANUpdateOutput();

	m_bDataLoaded = TRUE;
}

void CDataManager::SetInputDataHiddenNodesRunPCA()
{
	if (m_mapPatterns.size() < 3) return;

	m_Data.resize(m_mapPatterns.size(), BLOCK_X * BLOCK_Y * HIDDENNODESIZE);

	int row = 0, col = 0;
	for (auto& item : m_mapPatterns) {
		CDigitPattern& pattern = item.second;

		col = 0;
		for (const auto& item2 : pattern.vecPreprocessedPattern) {
			m_Data(row, col) = item2;
			col++;
		}

		MatrixXd mtrxHiddenLayerNodes; // 1-d matrix
		GetNNHiddenNodes(pattern.vecPreprocessedPattern, mtrxHiddenLayerNodes);
		for (int idx = 0; idx < mtrxHiddenLayerNodes.rows(); idx++) {
			m_Data(row, col) = mtrxHiddenLayerNodes(idx, 0);
			col++;
		}

		row++;
	}

	m_pca.SetData(&m_Data);
	m_pca.ComputeMeanCenteredMatrix();
	ComputePCANUpdateOutput();

	m_bDataLoaded = TRUE;
}

void CDataManager::SetInputDataHiddenOutputNodesRunPCA()
{
	if (m_mapPatterns.size() < 3) return;

	m_Data.resize(m_mapPatterns.size(), BLOCK_X * BLOCK_Y * HIDDENNODESIZE * OUTPUTNODESIZE);

	int row = 0, col = 0;
	for (auto& item : m_mapPatterns) {
		CDigitPattern& pattern = item.second;

		col = 0;
		for (const auto& item2 : pattern.vecPreprocessedPattern) {
			m_Data(row, col) = item2;
			col++;
		}

		MatrixXd mtrxHiddenLayerNodes, mtrxOutputLayerNodes; // 1-d matrix
		GetNNHiddenOutputNodes(pattern.vecPreprocessedPattern, mtrxHiddenLayerNodes, mtrxOutputLayerNodes);
		for (int idx = 0; idx < mtrxHiddenLayerNodes.rows(); idx++) {
			m_Data(row, col) = mtrxHiddenLayerNodes(idx, 0);
			col++;
		}

		for (int idx = 0; idx < mtrxOutputLayerNodes.rows(); idx++) {
			m_Data(row, col) = mtrxOutputLayerNodes(idx, 0);
			col++;
		}


		row++;
	}

	m_pca.SetData(&m_Data);
	m_pca.ComputeMeanCenteredMatrix();
	ComputePCANUpdateOutput();

	m_bDataLoaded = TRUE;
}

void CDataManager::ComputePCANUpdateOutput()
{
	m_pca.RunPCA();
	UpdatePCAOutputs();
}

void CDataManager::csvline_populate_values(vector<CString> &record, CString line, char delimiter)
{
	int linepos=0;
	int inquotes=false;
	CString c, nc;
	int linemax=line.GetLength();
	CString curstring;
	record.clear();

	while(line.GetLength()!=0 && linepos < linemax)
	{
		c = line.Left(1);
		line.Delete(0, 1);
		nc = line.Left(1);

		if (!inquotes && curstring.GetLength()==0 && c=="\"")
		{
			//beginquotechar
			inquotes=true;
		}
		else if (inquotes && c=="\"")
		{
			//quotechar
			if ( (linepos+1 <linemax) && (nc=="\"") ) 
			{
				//encountered 2 double quotes in a row (resolves to 1 double quote)
				curstring += c;
				linepos++;
			}
			else
			{
				//endquotechar
				inquotes=false; 
			}
		}
		else if (!inquotes && c==delimiter)
		{
			//end of field
			record.push_back( curstring );
			curstring="";
		}
		else if (!inquotes && (c=="\r" || c=="\n") )
		{
			record.push_back( curstring );
			return;
		}
		else
		{
			curstring += c;
		}
		linepos++;
	}
	curstring.TrimLeft();
	curstring.TrimRight();
	record.push_back( curstring );
}


CString RemoveDoubleQuotation(CString szInput)
{
	CString szOutput = szInput;
	szOutput.TrimLeft(); szOutput.TrimRight();
	// remove double quotation
	while (szOutput.Left(1)==_T("\"") || szOutput.Right(1)==_T("\""))
	{
		int pos = szOutput.Find(_T("\""));
		if (pos == 0)
			szOutput = szOutput.Right(szOutput.GetLength()-1);
		else if (pos == szOutput.GetLength()-1)
			szOutput = szOutput.Left(szOutput.GetLength()-1);
	}
	return szOutput;
}

//void CDataManager::SetSelectedVariable(int cKey)
//{
//	m_nSelectedVariableColumnKey = cKey; // update with new selection
//
//	// recalculate all histogram bins
//	//m_dcNumerical_Display.RecomputeAllHistograms();
//}

void CDataManager::ResetSelectedItems()
{
	GetInteractionManager()->ResetSelectedItems();
	GetInteractionManager()->ResetHighlightedItem();
}

void CDataManager::RestoreOriginalData()
{
	GetInteractionManager()->ResetDeletedItems();
	GetInteractionManager()->ResetSelectedItems();
	GetInteractionManager()->ResetHighlightedItem();

	// All Data has been restored
	RestoreInitialData();
}

void CDataManager::DeleteSelecteditems()
{
	CInteractionManager *pIM = GetInteractionManager();

	if (pIM->GetNumSelectedItems() == 0) return;

	//m_dcNumerical_Display.RemoveInstancesByRowIndexes(pIM->GetSelectedItems());

	// reset interaction manager
	pIM->ResetSelectedItems();	
	
	// update PCA flipping control
	//m_pca.GetFlippingCtrl()->Update(m_dcNumerical_Display.Rows(), m_dcNumerical_Display.Cols());
	
	GetPCA()->ComputeMeanCenteredMatrix();
	ComputePCANUpdateOutput();
}


void CDataManager::SearchByName(vector<CString> names)
{
	//CInteractionManager *pIM = GetInteractionManager();
	//
	//for (auto it = m_Original_RawData[m_nSelectedVariableColumnKey].begin();
	//	it != m_Original_RawData[m_nSelectedVariableColumnKey].end();
	//	++it)
	//{
	//	int row_idx = (*it).first;
	//	CString data = (*it).second;
	//	for (int i = 0; i < names.size(); i++)
	//	{
	//		if (data == names[i]) // find all items that have the same name
	//			pIM->SetSelectItem(row_idx); // mark as selected item
	//	}
	//}
}

BOOL CDataManager::LoadPatternCSVFormatFile(CString& szfilePathName)
{
	// checking if the file can be opened.
	FILE* fp = NULL;
	fopen_s(&fp, CStringWrapper(szfilePathName).getchardata(), "r+");
	if (fp == NULL) {
		return FALSE; // error opening the file
	}
	fclose(fp);

	std::vector<std::string> dataset;
	std::string s = stdext::cvt::wstring_convert<cvt_t>().to_bytes(szfilePathName);
	int rows = CSVParser::LoadingCSVFile(s.c_str(), dataset);
	BOOL bDuplicatedPatternFound = FALSE;
	int rowidx = 0, colidx = 0;
	for (const auto& row : dataset) {
		std::vector<std::string> instance;
		CSVParser::csvline_populate_values(instance, row, ",");

		CDigitPattern digitPattern;
		digitPattern.nDigit = atoi(instance[0].c_str());
		for (size_t i=1; i<instance.size(); i++)
		{
			std::string s = instance[i];
			int nitem = atoi(s.c_str());
			digitPattern.szPattern += CString(s.c_str());
			digitPattern.vecOriginalPattern.push_back(nitem);
			digitPattern.vecPreprocessedPattern.push_back(nitem);
		}
		digitPattern.GenerateDigitTargetPattern(m_nNumOutputNode);

		// generate hash for string pattern for checking duplicates
		size_t hashKey = GenerateHash(digitPattern.szPattern);
		//if (m_mapPatterns.find(hashKey) == m_mapPatterns.end()) {
			// add the pattern if the same pattern does not exist.
			m_mapPatterns.insert(pair<size_t, CDigitPattern>(hashKey, digitPattern));
		//}
		//else {
		//	bDuplicatedPatternFound = TRUE;
		//}
	}

	//if (bDuplicatedPatternFound) {
	//	AfxMessageBox(_T("Please note that duplicated pattern has been removed from the list."));
	//}

	return TRUE;
}

BOOL CDataManager::SavePatternCSVFormatFile(CString& szfilePathName)
{
	FILE* fp = NULL;
	fopen_s(&fp, CStringWrapper(szfilePathName).getchardata(), "w+");
	if (fp == NULL) {
		return FALSE; // error opening the file
	}

	for (const auto& item : m_mapPatterns) {
		CDigitPattern pattern = item.second;

		fprintf_s(fp, "%d,", pattern.nDigit);	// DIGIT

		for (int i = 0; i < pattern.vecOriginalPattern.size(); i++) {
			fprintf_s(fp, "%d", pattern.vecOriginalPattern[i]);
			if (i + 1 < pattern.vecOriginalPattern.size())
				fprintf_s(fp, ",");
		}
		fprintf_s(fp, "\n");
	}

	fclose(fp);

	return TRUE;
}


void CDataManager::GetNNHiddenNodes(vector<int>& inputPattern, MatrixXd& mtrxHiddenLayerNodes)
{
	// using previously computed weight information
	NNLINKS& dWeight_btwnInputHidden = m_dWeight_btwnInputHidden;

	// using hidden node output as features
	//  https://datascience.stackexchange.com/questions/24000/how-to-use-neural-networks-hidden-layer-output-for-feature-engineering

	// init the 1-D matrix
	mtrxHiddenLayerNodes.resize(HIDDENNODESIZE, 1);

	double NET = 0.0;
	// Calculate Hidden Layer ////////////////////////
	for (int p = 0; p < HIDDENNODESIZE; p++)
	{
		NET = 0.0;
		for (int i = 0; i < INPUTNODESIZE; i++)
			NET += inputPattern[i] * dWeight_btwnInputHidden[i][p];
#ifdef ActivationFunction_SIGMOID
		mtrxHiddenLayerNodes(p, 0) = sigmoid(NET);
#elif ActivationFunction_RELU
		mtrxHiddenLayerNodes(p, 0) = ReLU(NET);
#endif
	}
}


void CDataManager::GetNNHiddenOutputNodes(vector<int>& inputPattern, 
	MatrixXd& mtrxHiddenLayerNodes, MatrixXd& mtrxOutputLayerNodes)
{
	// using previously computed weight information
	NNLINKS& dWeight_btwnInputHidden = m_dWeight_btwnInputHidden;
	NNLINKS& dWeight_btwnHiddenOutput = m_dWeight_btwnHiddenOutput;

	// using hidden node output as features
	//  https://datascience.stackexchange.com/questions/24000/how-to-use-neural-networks-hidden-layer-output-for-feature-engineering

	// init the 1-D matrix
	mtrxHiddenLayerNodes.resize(HIDDENNODESIZE, 1);
	mtrxOutputLayerNodes.resize(HIDDENNODESIZE, 1);

	double NET = 0.0;

	// Calculate Hidden Layer ////////////////////////
	for (int p = 0; p < HIDDENNODESIZE; p++)
	{
		NET = 0.0;
		for (int i = 0; i < INPUTNODESIZE; i++)
			NET += inputPattern[i] * dWeight_btwnInputHidden[i][p];
#ifdef ActivationFunction_SIGMOID
		mtrxHiddenLayerNodes(p, 0) = sigmoid(NET);
#elif ActivationFunction_RELU
		mtrxHiddenLayerNodes(p, 0) = ReLU(NET);
#endif
	}

	// Calculate Output Layer ////////////////////////
	for (int q = 0; q < OUTPUTNODESIZE; q++)
	{
		NET = 0.0;
		for (int p = 0; p < HIDDENNODESIZE; p++)
			NET += mtrxHiddenLayerNodes(p, 0) * dWeight_btwnHiddenOutput[p][q];
#ifdef ActivationFunction_SIGMOID
		mtrxOutputLayerNodes(q, 0) = sigmoid(NET);
#elif ActivationFunction_RELU
		mtrxOutputLayerNodes(q, 0) = ReLU(NET);
#endif
	}
}

void CDataManager::NNForwardComputation(vector<int>& inputPattern, double *pdOutputValues)
{
	NNNODES& dHiddenLayerNodes = m_dHiddenLayerNodes;
	NNNODES& dOutputLayerNodes = m_dOutputLayerNodes;
	NNLINKS& dWeight_btwnInputHidden = m_dWeight_btwnInputHidden;
	NNLINKS& dWeight_btwnHiddenOutput = m_dWeight_btwnHiddenOutput;

	// Calculate Hidden Layer ////////////////////////

#pragma omp for
	for (int p = 0; p < HIDDENNODESIZE; p++)
	{
		double NET = 0.0;
		for (int i = 0; i < INPUTNODESIZE; i++)
			NET += inputPattern[i] * dWeight_btwnInputHidden[i][p];
#ifdef ActivationFunction_SIGMOID
		dHiddenLayerNodes[p] = sigmoid(NET);
#elif ActivationFunction_RELU
		dHiddenLayerNodes[p] = ReLU(NET);
#endif
	}

	// Calculate Output Layer ////////////////////////
#pragma omp for
	for (int q = 0; q < OUTPUTNODESIZE; q++)
	{
		double NET = 0.0;
		for (int p = 0; p < HIDDENNODESIZE; p++)
			NET += dHiddenLayerNodes[p] * dWeight_btwnHiddenOutput[p][q];

		if (pdOutputValues) pdOutputValues[q] = NET;
#ifdef ActivationFunction_SIGMOID
		dOutputLayerNodes[q] = sigmoid(NET);
#elif ActivationFunction_RELU
		dOutputLayerNodes[q] = ReLU(NET);
#endif
	}
}

void CDataManager::NNBackwardComputation(CDigitPattern& digitPattern, double iteration)
{
	NNNODES& dHiddenLayerNodes = m_dHiddenLayerNodes;
	NNNODES& dOutputLayerNodes = m_dOutputLayerNodes;
	NNLINKS& dWeight_btwnInputHidden = m_dWeight_btwnInputHidden;
	NNLINKS& dWeight_btwnInputHiddenPrev = m_dWeight_btwnInputHiddenPrev;
	NNLINKS& dWeight_btwnHiddenOutput = m_dWeight_btwnHiddenOutput;
	NNLINKS& dWeight_btwnHiddenOutputPrev = m_dWeight_btwnHiddenOutputPrev;

/*/
	double lrate = LEARNING_RATE;
	NNNODES Delta_W_Out;
	for (int q = 0; q < OUTPUTNODESIZE; q++) {
		Delta_W_Out.push_back(0.0);
	}

	// Adjusting Out Layer ///////////////////////////
	for (int q = 0; q < OUTPUTNODESIZE; q++)
	{
		Delta_W_Out[q] = (dOutputLayerNodes[q] * (1.0 - dOutputLayerNodes[q]))
			* ((double)digitPattern.vecTargetPattern[q] - dOutputLayerNodes[q]);
		for (int p = 0; p < HIDDENNODESIZE; p++) {
			double MOMENTUM = MOMENTUM_RATE *
				(dWeight_btwnHiddenOutput[p][q] - dWeight_btwnHiddenOutputPrev[p][q]);
			dWeight_btwnHiddenOutputPrev[p][q] = dWeight_btwnHiddenOutput[p][q];	// backup
			dWeight_btwnHiddenOutput[p][q] +=
				lrate * Delta_W_Out[q] * dHiddenLayerNodes[p] + MOMENTUM;
		}
	}

	// Adjusting Hidden Layer ///////////////////////////
	for (int p = 0; p < HIDDENNODESIZE; p++)
	{
		double delta = 0;
		for (int q = 0; q < OUTPUTNODESIZE; q++) {
			delta += Delta_W_Out[q] * dWeight_btwnHiddenOutput[p][q];
		}
		double Delta_W_Hidden = (dHiddenLayerNodes[p]
			* (1.0 - dHiddenLayerNodes[p])) * delta;
		for (int i = 0; i < INPUTNODESIZE; i++) {
			double MOMENTUM = MOMENTUM_RATE *
				(dWeight_btwnInputHidden[i][p] - dWeight_btwnInputHiddenPrev[i][p]);
			dWeight_btwnInputHiddenPrev[i][p] = dWeight_btwnInputHidden[i][p];
			dWeight_btwnInputHidden[i][p] +=
				lrate * Delta_W_Hidden * digitPattern.vecPreprocessedPattern[i] + MOMENTUM;
		}
	}

	/*/
	// Adjusting Output Layer ///////////////////////////
	// https://blog.quantinsti.com/backpropagation/
	NNNODES Delta_W_Out(OUTPUTNODESIZE);

#ifdef ActivationFunction_SIGMOID
#pragma omp for
	for (int q = 0; q < OUTPUTNODESIZE; q++) {
		Delta_W_Out[q] = (dOutputLayerNodes[q] * (1.0 - dOutputLayerNodes[q])) // activation function differentiation
			* ((double)digitPattern.vecTargetPattern[q] - dOutputLayerNodes[q]);
	}
#elif ActivationFunction_RELU
#pragma omp for
	for (int q = 0; q < OUTPUTNODESIZE; q++) {
		Delta_W_Out[q] = ReLU_derivative(q) // activation function differentiation
			* ((double)digitPattern.vecTargetPattern[q] - dOutputLayerNodes[q]);
	}
#endif

	// Explanation: (MOMENTUM_RATE / ( 2 * iteration) )
	// It is trying to decrease the momentum value when it gets closer to the optimum weight value of the neural network based on the total number of iterations. For instance, if the iteration is increased, the overall momentum rate is decreased.
	// Whenever the computation is gettiing closer to the weight optimum, momentum value must be getting closer to zero. Otherwise, the network training is creating a damping phenomenon (values are continuously up and down to find an optimum). 
	double damping_momentum_optimal = (MOMENTUM_RATE / (2 * iteration));

#pragma omp for collapse(2)
	for (int q = 0; q < OUTPUTNODESIZE; q++)
	{
		for (int p = 0; p < HIDDENNODESIZE; p++) {
			double dMomentum = damping_momentum_optimal *
				(dWeight_btwnHiddenOutput[p][q] - dWeight_btwnHiddenOutputPrev[p][q]);
			dWeight_btwnHiddenOutputPrev[p][q] = dWeight_btwnHiddenOutput[p][q];	// backup
			dWeight_btwnHiddenOutput[p][q] +=
				LEARNING_RATE * Delta_W_Out[q] * dHiddenLayerNodes[p] 
				+ dMomentum;
		}
	}


	// Adjusting Hidden Layer ///////////////////////////
	NNNODES Delta_W_Hidden(HIDDENNODESIZE);

#ifdef ActivationFunction_SIGMOID
#pragma omp for
	for (int p = 0; p < HIDDENNODESIZE; p++)
	{
		double delta = 0;
		for (int q = 0; q < OUTPUTNODESIZE; q++) {
			delta += Delta_W_Out[q] * dWeight_btwnHiddenOutput[p][q];
		}
		Delta_W_Hidden[p] =
			(dHiddenLayerNodes[p] * (1.0 - dHiddenLayerNodes[p])) // activation function differentiation
			* delta;
	}
#elif ActivationFunction_RELU
#pragma omp for
	for (int p = 0; p < HIDDENNODESIZE; p++)
	{
		double delta = 0;
		for (int q = 0; q < OUTPUTNODESIZE; q++) {
			delta += Delta_W_Out[q] * dWeight_btwnHiddenOutput[p][q];
		}
		Delta_W_Hidden[p] =
			ReLU_derivative(dHiddenLayerNodes[p]) // activation function differentiation
			* delta;
	}
#endif // ActivationFunction


#pragma omp for collapse(2)
	for (int p = 0; p < HIDDENNODESIZE; p++)
	{
		for (int i = 0; i < INPUTNODESIZE; i++) {
			double dMomentum = damping_momentum_optimal *
				(dWeight_btwnInputHidden[i][p] - dWeight_btwnInputHiddenPrev[i][p]);
			dWeight_btwnInputHiddenPrev[i][p] = dWeight_btwnInputHidden[i][p];
			dWeight_btwnInputHidden[i][p] +=  
				LEARNING_RATE * Delta_W_Hidden[p] 
				* digitPattern.vecPreprocessedPattern[i] 
				+ dMomentum;
		}
	}
//*/
}
