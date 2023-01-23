
// DigitPerceptronDlg.cpp : implementation file
//

// Neural Network cost function (sum of square errors)
// references 
// https://www.bogotobogo.com/python/scikit-learn/Artificial-Neural-Network-ANN-4-Backpropagation.php
// https://medium.com/@zeeshanmulla/cost-activation-loss-function-neural-network-deep-learning-what-are-these-91167825a4de

#include "pch.h"
#include "framework.h"
#include "DigitPerceptron.h"
#include "DigitPerceptronDlg.h"
#include "afxdialogex.h"
#include "StringWrapper.h"
#include "DigitPatternInfoDlg.h"
#include "resource.h"
#include <algorithm>
#include <fstream>
#include "hpc_helpers.hpp"
#include <CSVParser.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDigitPerceptronDlg dialog

CDigitPerceptronDlg::CDigitPerceptronDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLGDIGIPATTERNGENERATOR_DIALOG, pParent)
	, m_nNumInputNode(INPUTNODESIZE)
	, m_nNumHiddenNode(HIDDENNODESIZE)
	, m_nNumOutputNode(OUTPUTNODESIZE)
	, m_szDigitPatternFileName(_T(""))
	, m_szDigitForPattern(_T(""))
	, m_nTotalPatterns(0)
	, m_pVisualRepresentation(NULL)
	, m_szMSE(_T("0.0%"))
	, m_nPCAInputDataType(ID_PCAINPUTDATA_INPUTNODES) // default
	, m_dNumErrorRate(ERROR_RATE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDigitPerceptronDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NUM_INPUTNODE, m_nNumInputNode);
	DDX_Text(pDX, IDC_NUM_HIDDENNODE, m_nNumHiddenNode);
	DDX_Text(pDX, IDC_NUM_OUTPUTNODE, m_nNumOutputNode);
	DDX_Text(pDX, IDC_DIGIT_PATTERN_FILE, m_szDigitPatternFileName);
	DDX_Text(pDX, IDC_NUM_DIGIT, m_szDigitForPattern);
	DDX_Text(pDX, IDC_NUM_PATTERNS, m_nTotalPatterns);
	DDV_MinMaxInt(pDX, m_nTotalPatterns, 0, INT_MAX);
	DDX_Control(pDX, IDC_LIST_PATTERNS, m_ctrlPatternsGrid);
	DDX_Control(pDX, IDC_LIST_PATTERNS_CNT, m_ctrlPatternsCntGrid);
	DDX_Control(pDX, IDC_PROGRESS_NNPROCESSING, m_ctrlNNProgress);
	DDX_Text(pDX, IDC_STATIC_MSE, m_szMSE);
	DDX_Text(pDX, IDC_NUM_ERROR_RATE, m_dNumErrorRate);
}

BEGIN_MESSAGE_MAP(CDigitPerceptronDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RUN_NN_PROCESSING, &CDigitPerceptronDlg::OnClickedRunNNProcessing)
	ON_COMMAND(ID_HELP_ABOUT, &CDigitPerceptronDlg::OnHelpAbout)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_FILE_OPEN, &CDigitPerceptronDlg::OnFileOpenPatternFile)
	ON_COMMAND(ID_FILE_SAVEPATTERNFILE, &CDigitPerceptronDlg::OnFileSavePatternFile)
	ON_BN_CLICKED(IDC_ADD_CURRENTPATTERN, &CDigitPerceptronDlg::OnBnClickedAddCurrentpattern)
	ON_BN_CLICKED(IDC_BUTTON_CLEARSCREEN, &CDigitPerceptronDlg::OnBnClickedButtonClearscreen)
	ON_COMMAND(ID_FILE_EXIT, &CDigitPerceptronDlg::OnFileExit)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PATTERNS, &CDigitPerceptronDlg::OnDblclkListPatterns)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_DIGITRECOGNITION, &CDigitPerceptronDlg::OnBnClickedButtonDigitrecognition)
	ON_COMMAND(ID_VISUAL_REPRESENTATION, &CDigitPerceptronDlg::OnVisualRepresentation)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PATTERNS, &CDigitPerceptronDlg::OnItemchangedListPatterns)
	ON_BN_CLICKED(IDC_REMOVE_SELECTEDPATTERN, &CDigitPerceptronDlg::OnBnClickedRemoveSelectedpattern)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PATTERNS, &CDigitPerceptronDlg::OnClickListPatterns)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST_PATTERNS, &CDigitPerceptronDlg::OnKillfocusListPatterns)
	ON_COMMAND(ID_PCAINPUTDATA_INPUTNODES, &CDigitPerceptronDlg::OnPcainputdataInputnodes)
	ON_COMMAND(ID_PCAINPUTDATA_INPUTANDHIDDENNODES, &CDigitPerceptronDlg::OnPcainputdataInputandhiddennodes)
	ON_COMMAND(ID_PCAINPUTDATA_INPUTHIDDENOUTPUTNODES, &CDigitPerceptronDlg::OnPcainputdataInputhiddenoutputnodes)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDigitPerceptronDlg message handlers

BOOL CDigitPerceptronDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_DataManager.SetParent(this);

	m_bPatternDataModified = FALSE;
	m_bDrawingActivated = FALSE;

	m_mtrxGridBlocks.resize(BLOCK_Y, BLOCK_X);
	m_mtrxGridBlocks.setZero();

	for (int i = 0; i < BLOCK_X; i++) {
		for (int j = 0; j < BLOCK_Y; j++) {
			m_mtrxGridBlocks(j, i) = FALSE;
		}
	}

	m_ctrlPatternsGrid.SetExtendedStyle(m_ctrlPatternsGrid.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT);
	m_ctrlPatternsGrid.InsertColumn(0, _T("Digit"), LVCFMT_LEFT, 60);
	m_ctrlPatternsGrid.InsertColumn(1, _T("Pattern"), LVCFMT_LEFT, 600);

	m_ctrlPatternsCntGrid.SetExtendedStyle(m_ctrlPatternsCntGrid.GetExtendedStyle() |
			LVS_EX_FULLROWSELECT);
	m_ctrlPatternsCntGrid.InsertColumn(0, _T("Digit"), LVCFMT_LEFT, 60);
	m_ctrlPatternsCntGrid.InsertColumn(1, _T("Count"), LVCFMT_LEFT, 80);

	srand((unsigned)time(NULL));

	m_DataManager.ResetNeuralNetowrk(m_nNumInputNode, m_nNumHiddenNode, m_nNumOutputNode);

	m_NeuralNetworkProcessed = FALSE;
	GetDlgItem(IDC_BUTTON_DIGITRECOGNITION)->EnableWindow(FALSE);
	GetDlgItem(IDC_RUN_NN_PROCESSING)->EnableWindow(FALSE);
	GetDlgItem(IDC_ADD_CURRENTPATTERN)->EnableWindow(FALSE);
	GetDlgItem(IDC_REMOVE_SELECTEDPATTERN)->EnableWindow(FALSE);

	//GetMenu()->EnableMenuItem(ID_VISUAL_REPRESENTATION, MF_DISABLED);
	//GetMenu()->EnableMenuItem(ID_PCAINPUTDATA_INPUTNODES, MF_DISABLED);
	//GetMenu()->EnableMenuItem(ID_PCAINPUTDATA_INPUTANDHIDDENNODES, MF_DISABLED);
	//GetMenu()->EnableMenuItem(ID_PCAINPUTDATA_INPUTHIDDENOUTPUTNODES, MF_DISABLED);


	m_pGraphObjects = NULL;
	CreatePatternResultsGraphs();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDigitPerceptronDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDigitPerceptronDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		DrawBlock();

		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDigitPerceptronDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDigitPerceptronDlg::OnHelpAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


void CDigitPerceptronDlg::DrawBlock()
{
	CClientDC	dc(this);
	CString TEMP;
	CRect rect;
	GetDlgItem(IDC_DRAWING_REGION)->GetWindowRect(rect);
	ScreenToClient(rect);

	CPen newPen2(PS_SOLID, 2, RGB(0, 0, 0));
	dc.SelectObject(&newPen2);
	dc.Rectangle(rect.left, rect.top, rect.right, rect.bottom);
	
	m_dGridWidth = rect.Width() / (double)BLOCK_X;		// Determine the area width
	m_dGridHeight = rect.Height() / (double)BLOCK_Y;

	CPen newPen(PS_SOLID, 1, RGB(120, 0, 0));

	dc.SelectObject(&newPen);

	// draw horizontal line
	for (int i = 1; i < BLOCK_Y; i++)
	{
		dc.MoveTo(rect.left, rect.top + (int)(i * m_dGridHeight));
		dc.LineTo(rect.right, rect.top + (int)(i * m_dGridHeight));
	}

	// draw vertical line
	for (int i = 1; i < BLOCK_X; i++)
	{
		dc.MoveTo(rect.left + (int)(i * m_dGridWidth), rect.top);
		dc.LineTo(rect.left + (int)(i * m_dGridWidth), rect.bottom);
	}

	for (int i = 0; i < BLOCK_X; i++) // column
	{
		for (int j = 0; j < BLOCK_Y; j++) // row
		{
			if (m_mtrxGridBlocks(j, i) > 0) {
				// if selected, highlight the block
				int x = rect.left + (int)(i * m_dGridWidth);
				int y = rect.top + (int)(j * m_dGridHeight);
				int cx = (int)((i + 1) * m_dGridWidth) - (int)(i * m_dGridWidth);
				int cy = (int)((j + 1) * m_dGridHeight) - (int)(j * m_dGridHeight);
				dc.FillSolidRect(x, y, cx, cy, RGB(0, 0, 0));
			}
		}
	}
}


void CDigitPerceptronDlg::ResetGridBlocks()
{
	// Initialize Grid Blocks
	for (int j = 0; j < BLOCK_Y; j++) {
		for (int i = 0; i < BLOCK_X; i++) {
			m_mtrxGridBlocks(j, i) = FALSE;
		}
	}
}

void CDigitPerceptronDlg::RedrawGridBlocks()
{
	// redraw Grid Blocks
	CRect rect;
	GetDlgItem(IDC_DRAWING_REGION)->GetWindowRect(rect);
	InvalidateRect(rect, FALSE);
}

void CDigitPerceptronDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect;
	GetDlgItem(IDC_DRAWING_REGION)->GetWindowRect(rect);
	ScreenToClient(rect);

	if (rect.left < point.x && point.x < rect.right
		&& rect.top < point.y && point.y < rect.bottom) {
		// check if the user interaction is happening within the grid block.

		int column = (int)((point.x - rect.left) / m_dGridWidth);
		int row = (int)((point.y - rect.top) / m_dGridHeight);
		if (column >= m_mtrxGridBlocks.cols())
			column = (int)m_mtrxGridBlocks.cols() - 1;
		if (row >= m_mtrxGridBlocks.rows())
			row = (int)m_mtrxGridBlocks.rows() - 1;

		m_mtrxGridBlocks(row, column) = !m_mtrxGridBlocks(row, column);	// Enable or Disable Grid block
		m_bDrawingActivated ^= TRUE;	// Activate drawing

		RedrawGridBlocks();

		GetDlgItem(IDC_ADD_CURRENTPATTERN)->EnableWindow(TRUE);
	}

	// disable pattern remove button from the list
	GetDlgItem(IDC_REMOVE_SELECTEDPATTERN)->EnableWindow(FALSE);
	
	// de-select item in the list
	m_ctrlPatternsGrid.SetItemState(m_ctrlPatternsGrid.GetSelectionMark(), 0, LVIS_SELECTED | LVIS_FOCUSED);

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CDigitPerceptronDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDrawingActivated = FALSE;

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CDigitPerceptronDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDrawingActivated) {
		CRect rect;
		GetDlgItem(IDC_DRAWING_REGION)->GetWindowRect(rect);
		ScreenToClient(rect);

		if (rect.left < point.x && point.x < rect.right
			&& rect.top < point.y && point.y < rect.bottom) {
			// check if the user interaction is happening within the grid block.

			int column = (int)((point.x - rect.left) / m_dGridWidth);
			int row = (int)((point.y - rect.top) / m_dGridHeight);

			if (m_mtrxGridBlocks(row, column) == FALSE) {
				// the selected block region needs to be activated. 
				m_mtrxGridBlocks(row, column) = TRUE;

				RedrawGridBlocks();
			}
		}
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CDigitPerceptronDlg::OnFileOpenPatternFile()
{
	if (m_bPatternDataModified == TRUE) {
		if (::MessageBox(NULL, _T("Your pattern data has been updated. Do you want to save it first?"), 
			_T("Confirmation"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			OnFileSavePatternFile();
		}
	}

	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, 
		_T("Digit Pattern Files (*.ptn)|*.ptn|All Files (*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		m_DataManager.Reset(); // remove data
		m_DataManager.ResetNeuralNetowrk(m_nNumInputNode, m_nNumHiddenNode, m_nNumOutputNode); // reset NN & corresponding data

		m_mtrxGridBlocks.setZero();
		for (int i = 0; i < BLOCK_X; i++) {
			for (int j = 0; j < BLOCK_Y; j++) {
				m_mtrxGridBlocks(j, i) = FALSE;
			}
		}

		m_bPatternDataModified = FALSE;
		m_szDigitForPattern.Empty();
		m_szDigitPatternFileName.Empty();

		m_szDigitPatternFilePathName = fileDlg.GetPathName();
		m_szDigitPatternFileName = fileDlg.GetFileName();
		UpdateData(FALSE);

		CWaitCursor wait;
		LoadPatternCSVFormatFile(m_szDigitPatternFilePathName);

		// apply pre-processing
		for (auto& item : m_DataManager.m_mapPatterns) {
			CDigitPattern& digitPattern = item.second;
			digitPattern.ApplyPreprocessing();
			digitPattern.GenerateDigitTargetPattern(m_nNumOutputNode);
		}

		InsertPatternsIntoList();
		UpdatePatternsCntIntoList();

		GetDlgItem(IDC_RUN_NN_PROCESSING)->EnableWindow(TRUE);

		UpdateVisualRepresentation();
	}
}

void CDigitPerceptronDlg::OnFileSavePatternFile()
{
	CFileDialog fileDlg(FALSE, _T(".ptn"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Digit Pattern Files (*.ptn)|*.ptn|All Files (*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		m_szDigitPatternFilePathName = fileDlg.GetPathName();
		m_szDigitPatternFileName = fileDlg.GetFileName();
		UpdateData(FALSE);

		SavePatternCSVFormatFile(m_szDigitPatternFilePathName);

		m_bPatternDataModified = FALSE;
	}
}


void CDigitPerceptronDlg::UpdatePatternsCntIntoList()
{
	m_ctrlPatternsCntGrid.DeleteAllItems();
	int nIndex(1);
	for (int digit = 0; digit < 10; digit++) {
		CString szTmp;
		szTmp.Format(_T("%d"), digit);
		int nPos = m_ctrlPatternsCntGrid.InsertItem(nIndex++, szTmp);

		int count =
			std::count_if(m_DataManager.m_mapPatterns.begin(), m_DataManager.m_mapPatterns.end(),
				[digit](const PATTERNST& e) { return e.second.nDigit == digit; });
		szTmp.Format(_T("%d"), count);
		m_ctrlPatternsCntGrid.SetItemText(nPos, 1, szTmp);
	}
}

void CDigitPerceptronDlg::InsertPatternsIntoList()
{
	m_ctrlPatternsGrid.DeleteAllItems();	// remove all items

	int nIndex(1);
	for (const auto& item : m_DataManager.m_mapPatterns) {
		CDigitPattern pattern = item.second;

		CString szDigit;
		szDigit.Format(_T("%d"), pattern.nDigit);

		int nPos = m_ctrlPatternsGrid.InsertItem(nIndex++, szDigit);
		m_ctrlPatternsGrid.SetItemText(nPos, 1, pattern.szPattern);
	}
}

void CDigitPerceptronDlg::LoadPatternCSVFormatFile(CString& szfilePathName)
{
	if (m_DataManager.LoadPatternCSVFormatFile(szfilePathName) == FALSE) {
		AfxMessageBox(_T("Not able to open the pattern file."));
		return; // error opening the file
	}

	m_nTotalPatterns = m_DataManager.m_mapPatterns.size(); // TOTAL DIGITPATTERN
	m_nNumInputNode = BLOCK_X * BLOCK_Y; // TOTAL INPUT NODE

	UpdateData(FALSE);
}

void CDigitPerceptronDlg::LoadPatternFile(CString & szfilePathName)
{
	FILE* fp = NULL;
	fopen_s(&fp, CStringWrapper(szfilePathName).getchardata(), "r+");
	if (fp == NULL) {
		AfxMessageBox(_T("Not able to open the pattern file."));
		return; // error opening the file
	}


	BOOL bDuplicatedPatternFound = FALSE;

	int temp;
	fscanf_s(fp, "%d ", &m_nTotalPatterns); // TOTAL DIGITPATTERN
	fscanf_s(fp, "%d ", &m_nNumInputNode); // TOTAL INPUT NODE
	for (int r = 0; r < m_nTotalPatterns; r++) {
		CDigitPattern digitPattern;

		fscanf_s(fp, "%d ", &(digitPattern.nDigit));	// DIGIT

		CString szTmp;
		for (int i = 0; i < m_nNumInputNode; i++) {
			fscanf_s(fp, "%d ", &temp);
			digitPattern.vecOriginalPattern.push_back(temp);
			szTmp.Format(_T("%d"), temp);
			digitPattern.szPattern += szTmp;
		}
		size_t hashKey = GenerateHash(digitPattern.szPattern);

		if (m_DataManager.m_mapPatterns.find(hashKey) == m_DataManager.m_mapPatterns.end()) {
			// add the pattern if the same pattern does not exist.
			m_DataManager.m_mapPatterns.insert(pair<size_t, CDigitPattern>(hashKey, digitPattern));
		}
		else {
			bDuplicatedPatternFound = TRUE;
		}
	}
	fclose(fp);

	if (bDuplicatedPatternFound) {
		AfxMessageBox(_T("Please note that duplicated pattern has been removed from the list."));
	}

	UpdateData(FALSE);
}

void CDigitPerceptronDlg::SavePatternCSVFormatFile(CString& szfilePathName)
{
	if (m_DataManager.SavePatternCSVFormatFile(szfilePathName) == FALSE) {
		AfxMessageBox(_T("Not able to create a pattern file."));
	}
}

void CDigitPerceptronDlg::SavePatternFile(CString& szfilePathName)
{
	FILE* fp = NULL;
	fopen_s(&fp, CStringWrapper(szfilePathName).getchardata(), "w+");
	if (fp == NULL) {
		AfxMessageBox(_T("Not able to create a pattern file."));
		return; // error opening the file
	}

	fprintf_s(fp, "%d\n", m_nTotalPatterns); // TOTAL DIGITPATTERN
	fprintf_s(fp, "%d\n", m_nNumInputNode); // TOTAL INPUT NODE

	for (const auto& item : m_DataManager.m_mapPatterns) {
		CDigitPattern pattern = item.second;

		fprintf_s(fp, "%d\t", pattern.nDigit);	// DIGIT

		for (int i = 0; i < pattern.vecOriginalPattern.size(); i++) {
			fprintf_s(fp, "%d\t", pattern.vecOriginalPattern[i]);
		}
		fprintf_s(fp, "\n");
	}

	fclose(fp);
}


void CDigitPerceptronDlg::OnBnClickedAddCurrentpattern()
{
	UpdateData(TRUE);

	// Formatting the user entered pattern
	BOOL bUserEnteredPatternExist = FALSE;
	CDigitPattern ptnUserEnteredPattern;
	for (int row = 0; row < BLOCK_Y; row++) {
		for (int column = 0; column < BLOCK_X; column++) {
			if (m_mtrxGridBlocks(row, column) == TRUE) {
				ptnUserEnteredPattern.szPattern += _T("1");
				ptnUserEnteredPattern.vecOriginalPattern.push_back(1);	// selected
				bUserEnteredPatternExist = TRUE;
			}
			else {
				ptnUserEnteredPattern.szPattern += _T("0");
				ptnUserEnteredPattern.vecOriginalPattern.push_back(0);	// not selected
			}
		}
	}

	if (bUserEnteredPatternExist == FALSE) {
		AfxMessageBox(_T("No pattern is entered!"));
		return;
	}
	if (m_szDigitForPattern.IsEmpty()) {
		AfxMessageBox(_T("Digit information needs to be entered!"));
		GetDlgItem(IDC_NUM_DIGIT)->SetFocus();
		return;
	}

	// Check if the same pattern exists in the list
	// find the pattern that has been updated
	for (const auto& item : m_DataManager.m_mapPatterns) {
		CDigitPattern digitPattern = item.second;
		if (ptnUserEnteredPattern.szPattern == digitPattern.szPattern)
		{
			// the same pattern exists
			AfxMessageBox(_T("Not able to add the pattern. The same pattern exists already!"));
			return;
		}
	}

	//------------------
	// Add the current pattern to the list
	ptnUserEnteredPattern.nDigit = _ttoi(m_szDigitForPattern);
	ptnUserEnteredPattern.ApplyPreprocessing();
	ptnUserEnteredPattern.GenerateDigitTargetPattern(m_nNumOutputNode);

	size_t hashKey = GenerateHash(ptnUserEnteredPattern.szPattern);
	m_DataManager.m_mapPatterns.insert(pair<size_t, CDigitPattern>(hashKey, ptnUserEnteredPattern));

	m_nTotalPatterns++;
	m_bPatternDataModified = TRUE;

	UpdateData(FALSE);

	//--------------------------------
	// Update Lists
	UpdatePatternsCntIntoList();
	int nIndex(1);
	if (m_ctrlPatternsGrid.GetItemCount() > 1)
		nIndex = m_ctrlPatternsGrid.GetItemCount() - 1;

	// convert int to CString format
	int nPos = m_ctrlPatternsGrid.InsertItem(nIndex, m_szDigitForPattern);
	m_ctrlPatternsGrid.SetItemText(nPos, 1, ptnUserEnteredPattern.szPattern);

	GetDlgItem(IDC_RUN_NN_PROCESSING)->EnableWindow(TRUE);

	UpdateVisualRepresentation();
}

void CDigitPerceptronDlg::UpdateVisualRepresentation()
{
	// update visual representation if available
	if (m_pVisualRepresentation) {
		if (m_pVisualRepresentation->IsWindowVisible()) {
			CWaitCursor wait;
			switch (m_nPCAInputDataType)
			{
			case ID_PCAINPUTDATA_INPUTNODES:
				m_DataManager.SetInputDataRunPCA();
				break;
			case ID_PCAINPUTDATA_INPUTANDHIDDENNODES: 
				if (m_NeuralNetworkProcessed == FALSE)
					NNProcessingAllPatterns(); // NN processing must be done first
				m_DataManager.SetInputDataHiddenNodesRunPCA();
				break;
			case ID_PCAINPUTDATA_INPUTHIDDENOUTPUTNODES:
				if (m_NeuralNetworkProcessed == FALSE)
					NNProcessingAllPatterns(); // NN processing must be done first
				m_DataManager.SetInputDataHiddenOutputNodesRunPCA();
				break;
			}
			m_pVisualRepresentation->Invalidate(FALSE);
		}
	}
}

void CDigitPerceptronDlg::OnClickedRunNNProcessing()
{
	if (m_DataManager.m_mapPatterns.size() == 0) {
		AfxMessageBox(_T("No pattern data has been added. Please add patterns first!"));
		return;
	}

	if (m_szDigitPatternFileName.IsEmpty()) {
		if (::MessageBox(NULL, _T("Your pattern data need to be save first. Do you want to save it?"),
			_T("Confirmation"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			OnFileSavePatternFile();
		}
		else {
			return;
		}
	}

	if (m_bPatternDataModified == TRUE) {
		if (::MessageBox(NULL, _T("Your pattern data has been changed. Do you want to save it?"),
			_T("Confirmation"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			SavePatternCSVFormatFile(m_szDigitPatternFilePathName);
			m_bPatternDataModified = FALSE;
		}
		else {
			return;
		}
	}

	CWaitCursor wait;
	NNProcessingAllPatterns();
	SaveNNToFile();

	AfxMessageBox(_T("Nerual Network processing has completed!"));

}

void CDigitPerceptronDlg::SaveNNToFile()
{
	CString szNNFilePathName = m_szDigitPatternFilePathName;
	szNNFilePathName.Replace(_T(".ptn"), _T(".net"));

	FILE* fp = NULL;
	fopen_s(&fp, CStringWrapper(szNNFilePathName).getchardata(), "w+");
	if (fp == NULL) {
		AfxMessageBox(_T("Not able to create a network file!"));
		return;
	}

	CString szTrainingAccuracy;
	fprintf(fp, "%d\n%d\n%d\n", m_nNumInputNode, m_nNumHiddenNode, m_nNumOutputNode);
	for (int i = 0; i < m_nNumInputNode; i++) {
		for (int p = 0; p < m_nNumHiddenNode; p++) {
			fprintf(fp, "%f ", m_DataManager.m_dWeight_btwnInputHidden[i][p]);
		}
		fprintf(fp, "\n");
	}

	for (int p = 0; p < m_nNumHiddenNode; p++) {
		for (int q = 0; q < m_nNumOutputNode; q++) {
			fprintf(fp, "%f ", m_DataManager.m_dWeight_btwnHiddenOutput[p][q]);
		}
		fprintf(fp, "\n");
	}

	// testing with the same dataset
	int correctCount = 0;
	for (auto& item : m_DataManager.m_mapPatterns) {
		CDigitPattern &digitPattern = item.second;

		m_DataManager.NNForwardComputation(digitPattern.vecPreprocessedPattern);

		fprintf(fp, " %d: ", digitPattern.nDigit);
		double highest = -1;
		int highestIdx = -1;
		int idx = 0;
		for (const auto& out : m_DataManager.m_dOutputLayerNodes) {
			fprintf(fp, " %f ", out);
			if (highest < out) {
				highest = out;
				highestIdx = idx;
			}
			idx++;
		}
		fprintf(fp, "\n");

		if (highestIdx == digitPattern.nDigit)
			correctCount++;
	}
	szTrainingAccuracy.Format(_T("Training Accuracy: %.2f"), 
		(correctCount / (double)m_DataManager.m_mapPatterns.size()) * 100.);
	fprintf(fp, "%s\n", CStringWrapper(szTrainingAccuracy).getchardata());
	fclose(fp);

	GetDlgItem(IDC_STATIC_NNACCURACY)->SetWindowTextW(szTrainingAccuracy);
	//AfxMessageBox(szTrainingAccuracy);
}

void CDigitPerceptronDlg::OnBnClickedButtonClearscreen()
{
	m_szDigitForPattern.Empty();
	ResetGridBlocks(); // reset grid
	RedrawGridBlocks();

	UpdateData(FALSE);

	GetDlgItem(IDC_ADD_CURRENTPATTERN)->EnableWindow(FALSE);
}


void CDigitPerceptronDlg::OnFileExit()
{
	PostMessage(WM_CLOSE);
}

void CDigitPerceptronDlg::NNProcessingAllPatterns()
{
	UpdateData(TRUE);

	// initialize progress bar
	m_ctrlNNProgress.SetRange(0, 100);
	m_ctrlNNProgress.SetPos(0);
	m_szMSE = _T("0.0%");
	UpdateData(FALSE);

	//// apply pre-processing
	//for (auto& item : m_DataManager.m_mapPatterns) {
	//	CDigitPattern &digitPattern = item.second;
	//	digitPattern.ApplyPreprocessing();
	//	digitPattern.GenerateDigitTargetPattern(m_nNumOutputNode);
	//}

	TIMERSTART(NEURAL_NETWORK);

	// apply neural network
	int i = 1;
	double error, error_max = 0.0;
	int progressbar_step = 0;
	do
	{
		error = 0;
		for (auto& item : m_DataManager.m_mapPatterns) {
			CDigitPattern& digitPattern = item.second;
			std::vector<int>& target = digitPattern.vecTargetPattern;

			m_DataManager.NNForwardComputation(digitPattern.vecPreprocessedPattern);
		
			// cost function (sum of square errors)
			for (int k = 0; k < m_nNumOutputNode; k++) {
				error += ((digitPattern.vecTargetPattern[k] - m_DataManager.m_dOutputLayerNodes[k])
					* (digitPattern.vecTargetPattern[k] - m_DataManager.m_dOutputLayerNodes[k]));
			}
			m_DataManager.NNBackwardComputation(digitPattern, i);
		}
		error = error / m_DataManager.m_mapPatterns.size();

		if (error_max == 0.0) {
			error_max = error;
			int step = (int)((error - m_dNumErrorRate) * 1000);
			step = (step < 0) ? 0 : step; // step cannot be negative value
			if (step > 0)
				m_ctrlNNProgress.SetRange(0, (short)step);
		}
		else {
			double step = ((error - m_dNumErrorRate) * 1000.0);
			int nLower, nUpper;
			m_ctrlNNProgress.GetRange(nLower, nUpper);
			step = nUpper - step;
			step = (step < 0) ? 0 : step; // step cannot be negative value
			m_ctrlNNProgress.SetPos((int)step);
			m_szMSE.Format(_T("MSE: %.3f%%"), error);
			UpdateData(FALSE);
		}

		i++;
		//cout << i << ": " << error << endl;
	} while (error >= m_dNumErrorRate);

	TIMERSTOP(NEURAL_NETWORK);

	int nLower, nUpper;
	m_ctrlNNProgress.GetRange(nLower, nUpper);
	m_ctrlNNProgress.SetPos(nUpper);
	UpdateData(FALSE);

	/*/
	// testing 
	for (const auto& item : m_mtmPreprocessedPatterns) {
		const int& digit = item.first;
		DIGITPATTERN targetPattern;
		for (int k = 0; k < m_nNumOutputNode; k++) {
			if (digit == k)
				targetPattern.push_back(1);
			else
				targetPattern.push_back(0);
		}

		const DIGITPATTERN& PreProcessedPattern = item.second;
		NNForwardComputation(PreProcessedPattern, m_DataManager.m_dHiddenLayerNodes, m_DataManager.m_dOutputLayerNodes,
			m_DataManager.m_dWeight_btwnInputHidden, m_DataManager.m_dWeight_btwnHiddenOutput);

		cout << digit << ": ";
		for (const auto &out: m_DataManager.m_dOutputLayerNodes)
			cout << out << " ";
		cout << endl;
	}
	//*/

	GetDlgItem(IDC_BUTTON_DIGITRECOGNITION)->EnableWindow(TRUE);
	m_NeuralNetworkProcessed = TRUE;
}

void CDigitPerceptronDlg::OnBnClickedButtonDigitrecognition()
{
	// Check if patten is added in the grid block
	BOOL bPatternAdded = FALSE;
	for (int y = 0; y < BLOCK_Y; y++) {	// row
		for (int x = 0; x < BLOCK_X; x++) {	// column
			if (m_mtrxGridBlocks(y, x) == TRUE) {
				bPatternAdded = TRUE;
				break;
			}
		}
	}

	if (bPatternAdded == FALSE) {
		AfxMessageBox(_T("Please enter pattern first!"));
		return;
	}

	if (m_NeuralNetworkProcessed == FALSE) {
		AfxMessageBox(_T("Neural Network needs to be processed first!"));
		return;
	}

	NNRunRecognition();
}


void CDigitPerceptronDlg::NNRunRecognition()
{
	CDigitPattern ptnUserEnteredPattern;
	for (int row = 0; row < BLOCK_Y; row++) {
		for (int column = 0; column < BLOCK_X; column++) {
			if (m_mtrxGridBlocks(row, column) == TRUE) {
				ptnUserEnteredPattern.vecOriginalPattern.push_back(1);	// selected
			}
			else {
				ptnUserEnteredPattern.vecOriginalPattern.push_back(0);	// not selected
			}
		}
	}

	// Pre-processing the user entered pattern on screen
	//ptnUserEnteredPattern.ApplyPreprocessing(&m_mtrxGridBlocks);
	ptnUserEnteredPattern.ApplyPreprocessing();
	ptnUserEnteredPattern.GenerateDigitTargetPattern(m_nNumOutputNode);

	// Forward computation
	double pdOutputValues[OUTPUTNODESIZE];
	m_DataManager.NNForwardComputation(ptnUserEnteredPattern.vecPreprocessedPattern, pdOutputValues);

	// initialize (digit 0 ~ 9) with the results in the out layer
	DIGITWEIGHT NUM[10];
	for (int i = 0; i < 10; i++) {
		NUM[i].Digit = i;
		NUM[i].NNOutput = m_DataManager.m_dOutputLayerNodes[i];
	}

	auto compareWeight = [](DIGITWEIGHT i, DIGITWEIGHT j) {
		return (i.NNOutput > j.NNOutput);
	};

	// Sorting
	int n = sizeof(NUM) / sizeof(NUM[0]);
	std::sort(NUM, NUM + n, compareWeight);
	CString temp;
	temp.Format(_T("Recognized as: %d"), NUM[0].Digit);
	//AfxMessageBox(temp);
	m_pGraphObjects->SetGraphTitle(temp);

	// change color in graph
	m_pGraphObjects->GetBarGraph()->emphasize_index = NUM[0].Digit;
	if (NUM[0].Digit == ptnUserEnteredPattern.nDigit)
		m_pGraphObjects->GetBarGraph()->color_indicator = 1;
	else
		m_pGraphObjects->GetBarGraph()->color_indicator = -1;


	// compute Softmax - output probabilities
	double sum_digits_exp = 0;
	for (int digit = 0; digit < 10; digit++) {
		sum_digits_exp += exp(pdOutputValues[digit]);
	}
	// update graph
	for (int digit = 0; digit < 10; digit++) {
		double prob = exp(pdOutputValues[digit]) / sum_digits_exp;
		m_pGraphObjects->Set2DBarGraphValue(digit + 1, 1, (float)prob);
		std::cout << digit << " " << (float)prob << " " << NUM[digit].NNOutput << endl;
	}
	m_pGraphObjects->Invalidate(FALSE);


	//m_szTestingOutput.Empty();
	//for (int i = 0; i < 10; i++) {
	//	/*CString temp;
	//	temp.Format(_T("DIGIT %d : %.4f"), NUM[i].Digit, NUM[i].NNOutput);
	//	temp += "\r\n";*/
	//	cout << NUM[i].Digit << ": " << NUM[i].NNOutput << endl;
	//	//m_szTestingOutput += temp;
	//	UpdateData(FALSE);
	//}
	//m_szNNResult.Format(_T("FINAL: %d"), NUM[0].Digit);

	UpdateData(FALSE);
}

void CDigitPerceptronDlg::OnVisualRepresentation()
{
	if (m_DataManager.m_mapPatterns.size() <= 3) {
		AfxMessageBox(_T("At least three patterns need to have for generating a visualization!"));
		return;
	}

	if (m_pVisualRepresentation == NULL) {
		m_pVisualRepresentation = new CDigitPerceptronViz;
		m_pVisualRepresentation->Create(IDD_DIALOG_VIZ, CWnd::GetDesktopWindow());

		m_DataManager.Reset();
	}
	m_pVisualRepresentation->ShowWindow(SW_SHOW);

	UpdateVisualRepresentation();
}

void CDigitPerceptronDlg::OnClose()
{
	if (m_bPatternDataModified == TRUE) {
		if (::MessageBox(NULL, _T("Your pattern data has not been saved yet. Do you want to save it?"),
			_T("Confirmation"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			OnFileSavePatternFile();
		}
	}

	if (m_pVisualRepresentation) {
		m_pVisualRepresentation->PostMessageW(WM_CLOSE);
		delete m_pVisualRepresentation;
	}

	CDialogEx::OnClose();
}

void CDigitPerceptronDlg::OnBnClickedRemoveSelectedpattern()
{
	OnBnClickedButtonClearscreen(); // remove current pattern from the screen

	// remove the selected pattern from the list. 
	int row = m_ctrlPatternsGrid.GetSelectionMark();
	if (row >= 0) {
		CString selectedDigitPattern = m_ctrlPatternsGrid.GetItemText(row, 1);

		// remove the selected pattern from the multimap
		for (auto it = m_DataManager.m_mapPatterns.begin(); it != m_DataManager.m_mapPatterns.end(); ++it) {
			CDigitPattern digitPattern = it->second;
			
			if (digitPattern.szPattern == selectedDigitPattern) {
				m_DataManager.m_mapPatterns.erase(it);
				break;
			}
		}

		m_ctrlPatternsGrid.DeleteItem(row);	// remove from the list
		m_nTotalPatterns--;	// decrease # of patterns
		UpdatePatternsCntIntoList(); // update list control

		if (m_nTotalPatterns == 0) {
			GetDlgItem(IDC_RUN_NN_PROCESSING)->EnableWindow(FALSE);
		}
	}
	UpdateData(FALSE);
	GetDlgItem(IDC_REMOVE_SELECTEDPATTERN)->EnableWindow(FALSE);

	UpdateVisualRepresentation();
}


void CDigitPerceptronDlg::OnItemchangedListPatterns(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (m_ctrlPatternsGrid.GetItemCount() > 0) {
		if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVNI_SELECTED))
		{
			GetDlgItem(IDC_REMOVE_SELECTEDPATTERN)->EnableWindow(TRUE);

			int row = m_ctrlPatternsGrid.GetSelectionMark();
			m_szDigitForPattern = m_ctrlPatternsGrid.GetItemText(row, 0);
			CStringWrapper pattern = m_ctrlPatternsGrid.GetItemText(row, 1);

			// update the selected pattern to grid blocks
			char* szpattern = pattern.getchardata();
			for (int row = 0; row < BLOCK_Y; row++) {
				for (int column = 0; column < BLOCK_X; column++) {
					int idx = row * (int)(BLOCK_X)+column;
					m_mtrxGridBlocks(row, column) = FALSE;
					if (szpattern[idx] == '1')
						m_mtrxGridBlocks(row, column) = TRUE;
				}
			}

			UpdateData(FALSE);
			RedrawGridBlocks();
		}
		else {
			//cout << "OnItemchangedListPatterns" << pNMLV->uChanged << "|" << pNMLV->uNewState << endl;
			GetDlgItem(IDC_REMOVE_SELECTEDPATTERN)->EnableWindow(FALSE);
		}
	}

	*pResult = 0;
}

void CDigitPerceptronDlg::OnClickListPatterns(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if (m_ctrlPatternsGrid.GetItemCount() > 0) {
		GetDlgItem(IDC_REMOVE_SELECTEDPATTERN)->EnableWindow(TRUE);

		int row = m_ctrlPatternsGrid.GetSelectionMark();
		m_szDigitForPattern = m_ctrlPatternsGrid.GetItemText(row, 0);
		CStringWrapper pattern = m_ctrlPatternsGrid.GetItemText(row, 1);

		// update the selected pattern to grid blocks
		char* szpattern = pattern.getchardata();
		for (int row = 0; row < BLOCK_Y; row++) {
			for (int column = 0; column < BLOCK_X; column++) {
				int idx = row * (int)(BLOCK_X) + column;
				m_mtrxGridBlocks(row, column) = FALSE;
				if (szpattern[idx] == '1')
					m_mtrxGridBlocks(row, column) = TRUE;
			}
		}
		UpdateData(FALSE);
		RedrawGridBlocks();
		
		//cout << "OnClickListPatterns" << row << endl;
	}

	*pResult = 0;
}


void CDigitPerceptronDlg::OnDblclkListPatterns(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (m_ctrlPatternsGrid.GetItemCount() > 0) {
		int row = m_ctrlPatternsGrid.GetSelectionMark();
		CString szDigitForPattern = m_ctrlPatternsGrid.GetItemText(row, 0);
		CString szDigitPattern = m_ctrlPatternsGrid.GetItemText(row, 1);

		CDigitPatternInfoDlg dlg;
		dlg.m_szDigit = szDigitForPattern;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			CString szUpdatedDigitForPattern = dlg.m_szDigit;
			if (szDigitForPattern != szUpdatedDigitForPattern) {
				// if digit information has been changed
				m_ctrlPatternsGrid.SetItemText(row, 0, szUpdatedDigitForPattern);
				UpdateData(FALSE);
				RedrawGridBlocks();

				// find the pattern that has been updated
				for (auto& item : m_DataManager.m_mapPatterns) {
					CDigitPattern& pattern = item.second;
					if (pattern.szPattern == szDigitPattern) {
						pattern.nDigit = _ttoi(szUpdatedDigitForPattern);
						pattern.GenerateDigitTargetPattern(m_nNumOutputNode);
						break;
					}
				}

				UpdatePatternsCntIntoList(); // update list control

				UpdateVisualRepresentation(); // update visualization if available
			}
		}
	}
	*pResult = 0;
}


void CDigitPerceptronDlg::OnKillfocusListPatterns(NMHDR* pNMHDR, LRESULT* pResult)
{
	//GetDlgItem(IDC_REMOVE_SELECTEDPATTERN)->EnableWindow(FALSE);
	*pResult = 0;
}

void CDigitPerceptronDlg::OnPcainputdataInputnodes()
{
	// ID_PCAINPUTDATA_INPUTNODES
	GetMenu()->CheckMenuItem(ID_PCAINPUTDATA_INPUTNODES, MF_CHECKED | MF_BYCOMMAND);
	GetMenu()->CheckMenuItem(ID_PCAINPUTDATA_INPUTANDHIDDENNODES, MF_UNCHECKED | MF_BYCOMMAND);
	GetMenu()->CheckMenuItem(ID_PCAINPUTDATA_INPUTHIDDENOUTPUTNODES, MF_UNCHECKED | MF_BYCOMMAND);

	m_nPCAInputDataType = ID_PCAINPUTDATA_INPUTNODES;
	if (m_pVisualRepresentation) {
		if (m_pVisualRepresentation->IsWindowVisible()) {
			UpdateVisualRepresentation();
		}
	}
}


void CDigitPerceptronDlg::OnPcainputdataInputandhiddennodes()
{
	// ID_PCAINPUTDATA_INPUTANDHIDDENNODES
	GetMenu()->CheckMenuItem(ID_PCAINPUTDATA_INPUTNODES, MF_UNCHECKED | MF_BYCOMMAND);
	GetMenu()->CheckMenuItem(ID_PCAINPUTDATA_INPUTANDHIDDENNODES, MF_CHECKED | MF_BYCOMMAND);
	GetMenu()->CheckMenuItem(ID_PCAINPUTDATA_INPUTHIDDENOUTPUTNODES, MF_UNCHECKED | MF_BYCOMMAND);
	m_nPCAInputDataType = ID_PCAINPUTDATA_INPUTANDHIDDENNODES;

	if (m_pVisualRepresentation) {
		if (m_pVisualRepresentation->IsWindowVisible()) {
			UpdateVisualRepresentation();
		}
	}
}


void CDigitPerceptronDlg::OnPcainputdataInputhiddenoutputnodes()
{
	// ID_PCAINPUTDATA_INPUTHIDDENOUTPUTNODES
	GetMenu()->CheckMenuItem(ID_PCAINPUTDATA_INPUTNODES, MF_UNCHECKED | MF_BYCOMMAND);
	GetMenu()->CheckMenuItem(ID_PCAINPUTDATA_INPUTANDHIDDENNODES, MF_UNCHECKED | MF_BYCOMMAND);
	GetMenu()->CheckMenuItem(ID_PCAINPUTDATA_INPUTHIDDENOUTPUTNODES, MF_CHECKED | MF_BYCOMMAND);
	m_nPCAInputDataType = ID_PCAINPUTDATA_INPUTHIDDENOUTPUTNODES;

	if (m_pVisualRepresentation) {
		if (m_pVisualRepresentation->IsWindowVisible()) {
			UpdateVisualRepresentation();
		}
	}
}


BOOL CDigitPerceptronDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE; // Do not process further (not close the dialog window)
		}

		int ctrlValue = ::GetKeyState(VK_CONTROL);
		if (ctrlValue)
		{
			switch (pMsg->wParam)
			{
			case 'v':
			case 'V':
				GetMenu()->EnableMenuItem(ID_VISUAL_REPRESENTATION, MF_ENABLED);
				GetMenu()->EnableMenuItem(ID_PCAINPUTDATA_INPUTNODES, MF_ENABLED);
				GetMenu()->EnableMenuItem(ID_PCAINPUTDATA_INPUTANDHIDDENNODES, MF_ENABLED);
				GetMenu()->EnableMenuItem(ID_PCAINPUTDATA_INPUTHIDDENOUTPUTNODES, MF_ENABLED);

				break;

			case 't':
			case 'T':
				TimeEvaluation_TTMNIST();
				break;

			case 'm':
			case 'M':
				MNISTDataConversion();
				break;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDigitPerceptronDlg::TimeEvaluation_DS2400()
{
	ofstream outputFile;
	outputFile.open("time.log");

	CWaitCursor wait;
	for (int samples = 10; samples <= 2400; samples += 10) {
		CString filename;
		filename.Format(_T("C:\\Workspace2022\\AAAI\\nr%d.ptn"), samples);
		outputFile << CT2A(filename) << "\t" << samples << "\t";

		for (int i = 0; i < 10; i++) {
			m_DataManager.ResetNeuralNetowrk(m_nNumInputNode, m_nNumHiddenNode, m_nNumOutputNode); // remove NN

			LoadPatternCSVFormatFile(filename);

			// apply pre-processing
			for (auto& item : m_DataManager.m_mapPatterns) {
				CDigitPattern& digitPattern = item.second;
				digitPattern.ApplyPreprocessing();
				digitPattern.GenerateDigitTargetPattern(m_nNumOutputNode);
			}


			const std::chrono::time_point<std::chrono::steady_clock> start =
				std::chrono::steady_clock::now();

			// apply neural network
			double error, error_max = 0.0;
			int progressbar_step = 0;
			do
			{
				error = 0;
				for (auto& item : m_DataManager.m_mapPatterns) {
					CDigitPattern& digitPattern = item.second;
					std::vector<int>& target = digitPattern.vecTargetPattern;

					m_DataManager.NNForwardComputation(digitPattern.vecPreprocessedPattern);

					// cost function (sum of square errors)
					for (int k = 0; k < m_nNumOutputNode; k++) {
						error += ((digitPattern.vecTargetPattern[k] - m_DataManager.m_dOutputLayerNodes[k])
							* (digitPattern.vecTargetPattern[k] - m_DataManager.m_dOutputLayerNodes[k]));
					}
					m_DataManager.NNBackwardComputation(digitPattern);
				}
				error = error / m_DataManager.m_mapPatterns.size();

				if (error_max == 0.0) {
					error_max = error;
					int step = (int)((error - m_dNumErrorRate) * 1000);
					step = (step < 0) ? 0 : step; // step cannot be negative value
					if (step > 0)
						m_ctrlNNProgress.SetRange(0, (short)step);
				}
				else {
					double step = ((error - m_dNumErrorRate) * 1000.0);
					int nLower, nUpper;
					m_ctrlNNProgress.GetRange(nLower, nUpper);
					step = nUpper - step;
					step = (step < 0) ? 0 : step; // step cannot be negative value
					m_ctrlNNProgress.SetPos((int)step);
					m_szMSE.Format(_T("MSE: %.3f%%"), error);
					UpdateData(FALSE);
				}

				//cout << i++ << ": " << error << endl;
			} while (error >= m_dNumErrorRate);

			const auto end = std::chrono::steady_clock::now();

			std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000. << endl;
			outputFile << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000. << "\t";
			
			//<< "us = "
			//	<< (end - start) / 1ms << "ms = " // almost equivalent form of the above, but
			//	<< (end - start) / 1s << "s.\n";  // using milliseconds and seconds accordingly
		}


		// testing with the same dataset
		int correctCount = 0;
		for (auto& item : m_DataManager.m_mapPatterns) {
			CDigitPattern& digitPattern = item.second;
			std::vector<int>& target = digitPattern.vecTargetPattern;

			m_DataManager.NNForwardComputation(digitPattern.vecPreprocessedPattern);

			double highest = -1;
			int highestIdx = -1;
			int idx = 0;
			for (const auto& out : m_DataManager.m_dOutputLayerNodes) {
				if (highest < out) {
					highest = out;
					highestIdx = idx;
				}
				idx++;
			}

			if (highestIdx == digitPattern.nDigit)
				correctCount++;
		}
		outputFile << (correctCount / (double)m_DataManager.m_mapPatterns.size()) * 100. << endl;
	}

	outputFile << endl;

	outputFile.close();
}


void CDigitPerceptronDlg::TimeEvaluation_TTMNIST()
{

	ofstream outputFile;
	outputFile.open("C:\\Workspace2022\\AAAI\\feautres2\\time60000.log");

	CWaitCursor wait;
	for (int samples = 60000; samples <= 60100; samples += 1000) {
		CString filename;
		filename.Format(_T("C:\\Workspace2022\\AAAI\\feautres2\\nr%d.ptn"), samples);
		outputFile << CT2A(filename) << "\t" << samples << "\t";

		for (int i = 0; i < 10; i++) {
			m_DataManager.ResetNeuralNetowrk(m_nNumInputNode, m_nNumHiddenNode, m_nNumOutputNode); // remove NN

			LoadPatternCSVFormatFile(filename);

			// apply pre-processing
			for (auto& item : m_DataManager.m_mapPatterns) {
				CDigitPattern& digitPattern = item.second;
				digitPattern.ApplyPreprocessing();
				digitPattern.GenerateDigitTargetPattern(m_nNumOutputNode);
			}


			const std::chrono::time_point<std::chrono::steady_clock> start =
				std::chrono::steady_clock::now();

			// apply neural network
			double error, error_max = 0.0;
			int progressbar_step = 0;
			do
			{
				error = 0;
				for (auto& item : m_DataManager.m_mapPatterns) {
					CDigitPattern& digitPattern = item.second;
					std::vector<int>& target = digitPattern.vecTargetPattern;

					m_DataManager.NNForwardComputation(digitPattern.vecPreprocessedPattern);

					// cost function (sum of square errors)
					for (int k = 0; k < m_nNumOutputNode; k++) {
						error += ((digitPattern.vecTargetPattern[k] - m_DataManager.m_dOutputLayerNodes[k])
							* (digitPattern.vecTargetPattern[k] - m_DataManager.m_dOutputLayerNodes[k]));
					}
					m_DataManager.NNBackwardComputation(digitPattern);
				}
				error = error / m_DataManager.m_mapPatterns.size();

				if (error_max == 0.0) {
					error_max = error;
					int step = (int)((error - m_dNumErrorRate) * 1000);
					step = (step < 0) ? 0 : step; // step cannot be negative value
					if (step > 0)
						m_ctrlNNProgress.SetRange(0, (short)step);
				}
				else {
					double step = ((error - m_dNumErrorRate) * 1000.0);
					int nLower, nUpper;
					m_ctrlNNProgress.GetRange(nLower, nUpper);
					step = nUpper - step;
					step = (step < 0) ? 0 : step; // step cannot be negative value
					m_ctrlNNProgress.SetPos((int)step);
					m_szMSE.Format(_T("MSE: %.3f%%"), error);
					UpdateData(FALSE);
				}

				//cout << i++ << ": " << error << endl;
			} while (error >= m_dNumErrorRate);

			const auto end = std::chrono::steady_clock::now();

			std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000. << "  ";
			outputFile << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000. << "\t";

			//<< "us = "
			//	<< (end - start) / 1ms << "ms = " // almost equivalent form of the above, but
			//	<< (end - start) / 1s << "s.\n";  // using milliseconds and seconds accordingly
		}
		std::cout << endl;


		// testing with the same dataset
		int correctCount = 0;
		for (auto& item : m_DataManager.m_mapPatterns) {
			CDigitPattern& digitPattern = item.second;
			std::vector<int>& target = digitPattern.vecTargetPattern;

			m_DataManager.NNForwardComputation(digitPattern.vecPreprocessedPattern);

			double highest = -1;
			int highestIdx = -1;
			int idx = 0;
			for (const auto& out : m_DataManager.m_dOutputLayerNodes) {
				if (highest < out) {
					highest = out;
					highestIdx = idx;
				}
				idx++;
			}

			if (highestIdx == digitPattern.nDigit)
				correctCount++;
		}
		outputFile << (correctCount / (double)m_DataManager.m_mapPatterns.size()) * 100. << endl;
	}

	outputFile << endl;

	outputFile.close();
}


void CDigitPerceptronDlg::MNISTDataConversion()
{
	//CString szfilePathName(_T("C:\\Workspace2022\\AAAI\\MNIST\\mnist_train_noheader_test.csv"));
	//CString szfilePathName(_T("C:\\Workspace2022\\AAAI\\MNIST\\mnist_train_noheader.csv"));
	CString szfilePathName(_T("C:\\Workspace2022\\AAAI\\MNIST\\mnist_test_noheader.csv"));
	
	std::vector<std::string> dataset;
	std::string s = stdext::cvt::wstring_convert<cvt_t>().to_bytes(szfilePathName);
	int rows = CSVParser::LoadingCSVFile(s.c_str(), dataset);

	FILE* fp = NULL;
	fopen_s(&fp, "C:\\Workspace2022\\AAAI\\MNIST\\mnist_test_noheader.ptn", "w+");

	int rowidx = 0, colidx = 0;
	for (const auto& row : dataset) {
		std::cout << row << endl;
		std::vector<std::string> instance;
		CSVParser::csvline_populate_values(instance, row, ",");

		CDigitPattern digitPattern;
		digitPattern.nDigit = atoi(instance[0].c_str());
		for (size_t i = 1; i < instance.size(); i++)
		{
			std::string s = instance[i];
			int nitem = atoi(s.c_str());
			digitPattern.szPattern += CString(s.c_str());
			digitPattern.vecOriginalPattern.push_back(nitem);
		}
		digitPattern.ApplyPreprocessing2();

		fprintf_s(fp, "%d,", digitPattern.nDigit);	// DIGIT
		for (int i = 0; i < digitPattern.vecPreprocessedPattern.size(); i++) {
			fprintf_s(fp, "%d", digitPattern.vecPreprocessedPattern[i]);
			if (i + 1 < digitPattern.vecPreprocessedPattern.size())
				fprintf_s(fp, ",");
		}
		fprintf_s(fp, "\n");
	}
	fclose(fp);

	std::cout << "processing - done" << endl;

}


void CDigitPerceptronDlg::CreatePatternResultsGraphs()
{
	CRect rtRegion;
	GetDlgItem(IDC_DRAWING_REGION_RESULT)->GetWindowRect(rtRegion);
	ScreenToClient(rtRegion);

	// generate hashcode
	UINT hashcode = 1010101;

	// Create CGraphObject ( 2DBar graph )
	m_pGraphObjects = new CGraphObject();
	m_pGraphObjects->Create(NULL, NULL, NULL, rtRegion, this, hashcode, NULL);

	// Create graph and set graph parameters
	m_pGraphObjects->CreateGraph(GT_2DBAR);
	m_pGraphObjects->SetGraphBackgroundColor(RGB(255, 255, 255));
	m_pGraphObjects->SetGraphTitle(_T(""));

	// Add graph segments
	m_pGraphObjects->Add2DBarGraphSegment(_T("0"));
	m_pGraphObjects->Add2DBarGraphSegment(_T("1"));
	m_pGraphObjects->Add2DBarGraphSegment(_T("2"));
	m_pGraphObjects->Add2DBarGraphSegment(_T("3"));
	m_pGraphObjects->Add2DBarGraphSegment(_T("4"));
	m_pGraphObjects->Add2DBarGraphSegment(_T("5"));
	m_pGraphObjects->Add2DBarGraphSegment(_T("6"));
	m_pGraphObjects->Add2DBarGraphSegment(_T("7"));
	m_pGraphObjects->Add2DBarGraphSegment(_T("8"));
	m_pGraphObjects->Add2DBarGraphSegment(_T("9"));

	// Add graph series
	m_pGraphObjects->Add2DBarGraphSeries(_T("Series 1"), RGB(128, 128, 128));
	ResetGraph();
}

void CDigitPerceptronDlg::ResetGraph()
{
	// Set graph series values
	m_pGraphObjects->Set2DBarGraphValue(1, 1, 0.0f);
	m_pGraphObjects->Set2DBarGraphValue(2, 1, 0.0f);
	m_pGraphObjects->Set2DBarGraphValue(3, 1, 0.0f);
	m_pGraphObjects->Set2DBarGraphValue(4, 1, 0.0f);
	m_pGraphObjects->Set2DBarGraphValue(5, 1, 0.0f);
	m_pGraphObjects->Set2DBarGraphValue(6, 1, 0.0f);
	m_pGraphObjects->Set2DBarGraphValue(7, 1, 0.0f);
	m_pGraphObjects->Set2DBarGraphValue(8, 1, 0.0f);
	m_pGraphObjects->Set2DBarGraphValue(9, 1, 0.0f);
	m_pGraphObjects->Set2DBarGraphValue(10, 1, 0.0f);
}


void CDigitPerceptronDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	m_pGraphObjects->DestroyWindow();
	delete m_pGraphObjects;
	m_pGraphObjects = NULL;
}

