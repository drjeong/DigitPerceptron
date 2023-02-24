#pragma once

#include <PCA/PCA.h>
#include "ColorSpace.h"
#include "InteractionManager.h"
#include "DPoint.h"
#include "ConvexHull2.h"
#include "DigitPattern.h"
#include "ColorSpace.h"


#define INPUTNODESIZE 96
#define HIDDENNODESIZE 48
#define OUTPUTNODESIZE 10

#define LEARNING_RATE 0.1	// learning rate
#define MOMENTUM_RATE 0.7
#define ERROR_RATE 0.05

#define sigmoid(x)  (1./  (1 + exp(-x)) )
#define sigmoid_derivative(x)  (sigmoid(x) * (1 - sigmoid(x)))

#define ReLU(x) (x * (x > 0))
#define ReLU_derivative(x) (x > 0)

#define tanh(x) (2. * sigmoid(2 * x) - 1)

#define ActivationFunction_SIGMOID


typedef vector<double> NNNODES;
typedef vector<vector<double>> NNLINKS;

template<typename KeyType, typename ValueType>
std::pair<KeyType, ValueType> get_max(const std::map<KeyType, ValueType>& x) {
	using pairtype = std::pair<KeyType, ValueType>;
	return *std::max_element(x.begin(), x.end(), [](const pairtype & p1, const pairtype & p2) {
		return p1.second < p2.second;
	});
}

class CDataManager
{
public:
	CDataManager();
	virtual ~CDataManager(void);

	void SetParent(void *parent){m_pParent = parent;};
	void Reset();
	void ResetNeuralNetowrk(int nNumInputNode, int nNumHiddenNode, int nNumOutputNode);
	void RestoreInitialData();

	void SearchByName(vector<CString> names);
	
	//void InitColorForCategoricalInstances();
	vector<CGLColor> GetColorMap(int nNumClasses);

	void UpdatePCAOutputs();
	void CalcIndividualItemUncertainty();
	

	void SetInputDataHiddenNodesRunPCA();
	void SetInputDataHiddenOutputNodesRunPCA();
	void SetInputDataRunPCA();
	void ComputePCANUpdateOutput();
	
	void csvline_populate_values(vector<CString> &record, CString line, char delimiter);

	void RestoreOriginalData();
	void DeleteSelecteditems();
	void ResetSelectedItems();

	CPCA *GetPCA(){return &m_pca;};
	CInteractionManager *GetInteractionManager(){return m_InteractionManagerList.back();};
	
	BOOL IsDataLoaded(){return m_bDataLoaded;}

	CString GetInputFileName() {
		CString szTmp(m_szFileNameNPath);
		szTmp = szTmp.Mid(szTmp.ReverseFind('\\') + 1, szTmp.GetLength());
		return szTmp;
	}
	CString GetInputFileNameNPath() { return m_szFileNameNPath; }
	void SetInputFileNameNPath(CString fname) { m_szFileNameNPath = fname; }
	BOOL LoadPatternCSVFormatFile(CString& szfilePathName);
	BOOL SavePatternCSVFormatFile(CString& szfilePathName);

	void GetNNHiddenNodes(vector<int>& inputPattern, MatrixXd& mtrxHiddenLayerNodes);
	void GetNNHiddenOutputNodes(vector<int>& inputPattern,
		MatrixXd& mtrxHiddenLayerNodes, MatrixXd& mtrxOutputLayerNodes);
	void NNForwardComputation(vector<int>& digitPattern, double* pdOutputValues = NULL);
	void NNBackwardComputation(CDigitPattern& digitPattern, double iteration = 0.0);

	int GetGlyphSize() { return m_nGlyphSizeRatio; }
	void GlyphSizeIncrease() { m_nGlyphSizeRatio += 1; }
	void GlyphSizeDecrease() {
		m_nGlyphSizeRatio -= 1; 
		m_nGlyphSizeRatio = (m_nGlyphSizeRatio < 1) ? 1 : m_nGlyphSizeRatio;
	}

	void InitGLCallList();
	CGLColor GetRGBColor(int nDigit);

public:
	unsigned int m_glList_Grid; // glCallList
	unsigned int m_glList_Rectangle; // glCallList
	multimap < std::size_t/*hash key*/, CDigitPattern> m_mapPatterns;

	int m_nNumInputNode, m_nNumHiddenNode, m_nNumOutputNode;
	NNLINKS m_dWeight_btwnInputHidden; // weight in between input and hidden layer
	NNLINKS m_dWeight_btwnHiddenOutput; // weight in between hidden and output layer
	NNLINKS m_dWeight_btwnInputHiddenPrev; // weight in between input and hidden layer
	NNLINKS m_dWeight_btwnHiddenOutputPrev; // weight in between hidden and output layer
	NNNODES m_dHiddenLayerNodes;	// hidden layer nodes
	NNNODES m_dOutputLayerNodes;	// output layer nodes

private:
	vector<CInteractionManager*> m_InteractionManagerList;
	vector<CGLColor> m_rgbColorMap;

	// Opened file name
	CString m_szFileNameNPath; // file path + name

	int m_nGlyphSizeRatio;

	// PCA
	CPCA m_pca;
	MatrixXd m_Data;	// raw data

	BOOL m_bDataLoaded;	// indicator if data is loaded

	void *m_pParent; // Dialog Window
};

