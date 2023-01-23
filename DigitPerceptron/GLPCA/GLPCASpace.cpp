// GLPCASpace.cpp : implementation file
//

#include "pch.h"
#include "DigitPerceptronDlg.h"
#include "GLPCASpace.h"
#include "ColorSpace.h"
#include "freeglut.h"
#include "DRect.h"
#include "ConvexHull.h"
#include "StringWrapper.h"
#include <PCA/mathfunctions.h>
#include "gl2ps.h"
#include "GLfunctions.h"
#include "GLScissorTest.h"
#include "DataFunctions.h"
#include "resource.h"

#define ZOOMING_TIME 5
#define GLPCASPACE_MARGINGAP 20

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGLPCASpace

CGLPCASpace::CGLPCASpace()
{
	CGLView::m_nWindowType = GLWIN_PCASPACE;
	m_nDefinedMouseAction = NULL;
	m_timebuffer = 0;

	m_ftglFontText.Initialize(20);
}

CGLPCASpace::~CGLPCASpace()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGLPCASpace message handlers

void CGLPCASpace::Reset()
{
	m_nVisualComplexity = CGLView::MEDIUMCOMPLEXITY;
	m_ClutterManager_DataInstances.SetVisualComplexity(0.25);
	m_nShowBoxplot = MF_UNCHECKED;
	m_nShowBiplot = MF_UNCHECKED;

    CPortal::setPosition(-0.5, -0.5, 1.0, 1.0, 0.300, 0.300, .400, .400);
}

void CGLPCASpace::OnPaint() 
{
	//cout << "CGLPCASpace::OnPaint" << endl;
	glPushMatrix();
	glLoadIdentity();

	// draw boundary of this window
	glColor4f(0.5f, 0.5f, 0.5f, 0.3f);
	drawObject(*this, GL_LINE_STRIP);

	{
		CGLScissorTest glScissorTest(*this);
		DrawGLScene();
	}

	glPopMatrix();
}

CDPoint CGLPCASpace::GetGLScreenCoordinate(CDPoint pt)
{
	// normalization (normalize 0 ~ Width or 0 ~ Height)
	//CDataManager *pDM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager();
	//CInteractionManager *pIM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager()->GetInteractionManager();
	//C2DMinMax XAxisMinMax = pDM->GetPCA()->GetPCAMinMax(pIM->GetPCAXCoord());
	//C2DMinMax YAxisMinMax = pDM->GetPCA()->GetPCAMinMax(pIM->GetPCAYCoord());
	//pt.x = m_glRenderingRegion.left + (pt.x - XAxisMinMax.GetMin()) / (XAxisMinMax.GetDiff()) * m_glRenderingRegion.Width();
	//pt.y = m_glRenderingRegion.bottom + (pt.y - YAxisMinMax.GetMin()) / (YAxisMinMax.GetDiff()) * m_glRenderingRegion.Height();
	
	pt.x = m_glRenderingRegion.left + (pt.x - m_PC1DataRange.GetMin()) 
		/ (m_PC1DataRange.GetDiff()) * m_glRenderingRegion.Width();
	pt.y = m_glRenderingRegion.bottom + (pt.y - m_PC2DataRange.GetMin()) 
		/ (m_PC2DataRange.GetDiff()) * m_glRenderingRegion.Height();

	return pt;
}

void CGLPCASpace::DrawInstances()
{
	CDataManager *pDM = ((CDigitPerceptronDlg*)AfxGetMainWnd())->GetDataManager();
	CInteractionManager *pIM = ((CDigitPerceptronDlg*)AfxGetMainWnd())->GetDataManager()->GetInteractionManager();

	if (m_ClutterManager_DataInstances.IsEmpty() == FALSE && pIM->IsTrailEnabled() == FALSE)
	{
		m_ClutterManager_DataInstances.Draw(GL_RENDER);
		m_ClutterManager_TextInfo.draw();

		return;
	}

	m_ClutterManager_DataInstances.Init(*this);

	//cout << *this << endl;
	vector<vector<CDPoint>> *p2DPCAOutputList = pDM->GetPCA()->Get2DPCAOutputTrails();
	auto& VisibleInstances = pDM->m_mapPatterns;

	// define default blending
	double blend_amount = 0.6/(p2DPCAOutputList->size());
	double alpha = 0.8; // default

	if (pIM->IsTrailEnabled() == TRUE)
		alpha = blend_amount; // blend enabled
	if (pIM->GetHighlightedObject() != -1)
		alpha = blend_amount; // blend enabled
	if (pIM->GetNumSelectedItems() > 0)
		alpha = blend_amount; // blend enabled

	double dGlyphSizeRatio = pDM->GetGlyphSize();
	double ratio = dGlyphSizeRatio * (m_dPadWidth / m_dInitPadWidth);
	//ratio = (ratio > 20) ? 20 : ratio;
	//cout << dGlyphSizeRatio << endl;

	for (auto it=p2DPCAOutputList->begin(); it!=p2DPCAOutputList->end(); ++it)
	{
		int mrow = 0;
		auto pData = &(*it);
		for (auto& instance : VisibleInstances)
		{
			size_t rkey = instance.first;
			CGLColor color(1.0f, 1.0f, 1.0f);

			CDPoint pt = GetGLScreenCoordinate((*pData)[mrow]);
			CDRect rtItem(pt, ratio, ratio);
			CDRect rtGLItem = CPortal::GetPadPos(rtItem);

			//CDPoint pttmp(rtGL.GetHorizontalCenter(), rtGL.GetVerticalCenter());
			//if (this->IsInside(pttmp.x, pttmp.y) == false)
			//	continue; // not draw if the item is outside of the viewing angle

			if (this->IsInside(m_glRenderingRegion.GetHorizontalCenter(), 
				m_glRenderingRegion.GetVerticalCenter()) == false)
				continue; // not draw if the item is outside of the viewing angle
			

			glColor4f(color.R, color.G, color.B, alpha);
			if (pIM->IsTrailEnabled() == TRUE)
			{ // when trail is enabled

				glPushMatrix();
				if (pIM->IsItemSelected(rkey) == TRUE || pIM->GetHighlightedObject() == rkey)
				{
					glColor4f(color.R, color.G, color.B, 1.0);
					glTranslatef(0.0f, 0.0f, -0.8f); // to show the item in front
				}
				//drawObject(rt, GL_POLYGON); // traditional rectangle rendering
				drawObject(pDM->m_glList_Grid, rtGLItem, 0.8f); // using opengl calllist rendering
				glPopMatrix();

				// highlighted or selected items
				if (pIM->IsItemSelected(rkey) == TRUE || pIM->GetHighlightedObject() == rkey)
				{
					glPushMatrix();
					glTranslatef(0.0f, 0.0f, 0.5f);
					glLineWidth(1.5f);
					if (pIM->IsItemSelected(rkey) == TRUE)
						glColor4f(0.5f, 0.0f, 0.0f, 1.0);
					else if (pIM->GetHighlightedObject() == rkey)
						glColor4f(0.5f, 0.5f, 0.5f, 1.0);
					drawObject(rtGLItem, GL_LINE_STRIP);
					glLineWidth(1.0f);
					glPopMatrix();
				}
			}
			else
			{
				BOOL bHighlight = FALSE;
				if (pIM->IsItemSelected(rkey) == TRUE || pIM->GetHighlightedObject() == rkey)
					bHighlight = TRUE;
					
				// get cluster index if exists
				//int clusterIdx = -1;// ((CDigitPerceptronDlg*)AfxGetMainWnd())->GetClusteringDlg()->GetClusterIdx(rkey);
				//m_ClutterManager_DataInstances.AddNode_EvaluateOverlap(rkey, clusterIdx,
				//	pDM->m_glList_Grid, rt, 0.0f, CGLColor(color.R, color.G, color.B, alpha),
				//	bHighlight);

				glPushMatrix();
				glColor4f(color.R, color.G, color.B, 1.0);
				//glTranslatef(0.0f, 0.0f, -0.8f); // to show the item in front
				//drawObject(rt, GL_POLYGON); // traditional rectangle rendering
				drawObject(pDM->m_glList_Grid, rtGLItem, 0.8f); // using OpenGL Call_list rendering

				GLfloat len = 0.5f;
				GLfloat gridWidth = rtGLItem.Width() / BLOCK_X;
				GLfloat gridHeight = rtGLItem.Height() / BLOCK_Y;

				CDigitPattern &digitPattern = instance.second;
				GLfloat pos = -len;
				for (int j = 0; j < BLOCK_Y; j++) // row
				{
					for (int i = 0; i < BLOCK_X; i++) // column
					{
						if (digitPattern.vecPreprocessedPattern[j * BLOCK_X + i] == TRUE) {
							double l = rtGLItem.left + (int)(i * gridWidth);
							double t = rtGLItem.top - (int)(j * gridHeight);
							double r = l + gridWidth;
							double b = t - gridHeight;
							//CDRect rtGLTmp(l, b, r, t);

							//glColor3f(0.0f, 0.0f, 0.0f);
							color = pDM->GetRGBColor(instance.second.nDigit);
							glColor4f(color.R, color.G, color.B, 1.0);
							drawObject(pDM->m_glList_Rectangle, CDRect(l, b, r, t), 0.8f);
						}
					}
				}
				glPopMatrix();
			}

			// only the last point is considered to track positions (even if trail enabled)
			if (&p2DPCAOutputList->back() == pData)
			{
				if ((pIM->GetHighlightedObject() == rkey || pIM->IsItemSelected(rkey) == TRUE) ||
					pIM->IsShowLabel() == TRUE)
				{
					CString szLabel;
					szLabel.Format(_T("%d"), instance.second.nDigit);

					// for debugging only
					//CString tmp;
					//tmp.Format(_T(" %d"), rkey);
					//szLabel += tmp;

					//CDPoint pt = DetermineTextPosition(rtGLItem,
					//	m_ftglFontText.ftglFontGetCharRect(szLabel),
					//	m_glRenderingRegion);

					//m_ClutterManager_TextInfo.addNode(pt.x, pt.y, 1.0,
					//	CGLColor(0.0f, 0.0f, 0.0f, 1.0f), szLabel, &m_ftglFontText);

					glColor3f(0.0f, 0.0f, 0.0f);
					m_ftglFontText.FontDrawText(szLabel,
						rtGLItem.left, rtGLItem.top, 1.0);
				}
			}

			mrow++;
		}
		alpha += blend_amount;
	}
	//m_ClutterManager_DataInstances.Draw(GL_RENDER);
	//m_ClutterManager_TextInfo.draw();
}

void CGLPCASpace::DrawRangeSelectionBoundary()
{
	if (m_nDefinedMouseAction == MOUSE_RANGE_SELECTION)
	{
		CDRect *pregion = m_UserSelectedRegion.GetGLRegion();
		if (pregion->IsRectEmpty() == false)
		{
			glPushMatrix();
			glColor4f(0.0f, 0.0f, 0.0f, 0.5);
			drawObject(*pregion, GL_LINE_LOOP);
			glPopMatrix();
		}

		//glPushMatrix();
		//glDisable(GL_DEPTH_TEST);
		//glColor3f(0.4f, 0.0f, 1.0f);
		//glBegin(GL_LINE_STRIP);
		//POINTLIST::iterator it;
		//for (it = m_ptlist.begin(); it != m_ptlist.end(); ++it)
		//{
		//	CDPoint pt = *it;
		//	CDPoint glpt = CPortal::GetPadPos(pt);
		//	glVertex2d(pt.x, pt.y);
		//}
		//glEnd();
		//glEnable(GL_DEPTH_TEST);
		//glPopMatrix();
	}
}

void CGLPCASpace::DrawBoxPlot()
{
	if (IsReadyToRender() == FALSE) return;
	
	//if (m_nShowBoxplot == MF_CHECKED)
	//{
	//	CDRect horz_region(m_glRenderingRegion), vert_region(m_glRenderingRegion);
	//	switch (m_nBoxplotStyle)
	//	{
	//	case CGLBoxPlot::BASIC:
	//		horz_region.bottom = horz_region.top - 20;
	//		vert_region.right = vert_region.left + 20;
	//		break;
	//	case CGLBoxPlot::SIDEBYSIDE:
	//		horz_region.bottom = horz_region.top - 40;
	//		vert_region.right = vert_region.left + 40;
	//		break;
	//	}
	//	horz_region.left = vert_region.right;
	//	vert_region.top = horz_region.bottom;

	//	m_glRenderingRegion.left = vert_region.right;
	//	m_glRenderingRegion.top = horz_region.bottom;

	//	// draw outline body
	//	glColor4d(1.0, 1.0, 1.0, 0.5);
	//	drawObject(horz_region, GL_POLYGON);
	//	drawObject(vert_region, GL_POLYGON);

	//	switch (m_nBoxplotStyle)
	//	{
	//	case CGLBoxPlot::BASIC:
	//		DrawBasic_HorizontalBoxPlot(horz_region);
	//		DrawBasic_VerticalBoxPlot(vert_region);
	//		break;
	//	case CGLBoxPlot::SIDEBYSIDE:
	//		DrawGrouped_HorizontalBoxPlot(horz_region);
	//		DrawGrouped_VerticalBoxPlot(vert_region);
	//		break;
	//	}
	//}
}

void CGLPCASpace::DrawGrouped_VerticalBoxPlot(CDRect region)
{
	//CDataManager *pDM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager();
	//auto PCAOutput = pDM->GetPCA()->Get2DPCAOutputTrails()->back();

	//map<CString, vector<int/*rkey*/>> instance_rkeys; // store all instances separately per each categorical values
	//map<CString, vector<double>> data; // store data values separately per each categorical values
	//MatrixXd *pData = pDM->GetDataCube()->GetNormData();
	//auto& VisibleInstances = pDM->GetDataCube()->GetRowIndexes();
	//for (auto& instance : VisibleInstances)
	//{
	//	int mrow = instance.first;
	//	int rkey = instance.second;
	//	CString szLabel = pDM->GetDataItem(rkey, pDM->GetSelectedVariable());
	//	CDPoint pt = PCAOutput[mrow];
	//	data[szLabel].push_back(pt.y); // PC-2 data
	//	instance_rkeys[szLabel].push_back(rkey);
	//}

	//// determine quartiles for boxplots
	//map<CString, CGLBoxPlot> boxplots;
	//for (auto it = data.begin(); it != data.end(); ++it)
	//{
	//	CString szLabel = (*it).first;
	//	boxplots[szLabel].Initialize(data[szLabel], &instance_rkeys[szLabel],
	//		CGLBoxPlot::VERTICAL, m_nBoxplotBodyStyle, m_nWhiskerStyle);

	//	// computer quartiles & update if the data range is changed
	//	boxplots[szLabel].ComputePercentilesNWhiskers(m_PC2DataRange);
	//}

	//// drawing boxplots
	//double width = region.Width() / data.size(); // determine width of each boxplot
	//CDRect rtBoxPlotRegion(region.left, region.bottom, region.left + width, region.top); // determine the 1st boxplot region
	//for (auto it = data.begin(); it != data.end(); ++it)
	//{
	//	CString szLabel = (*it).first;
	//	CGLColor color = pDM->GetRGBColor(szLabel);
	//	color.A = 0.5f;

	//	boxplots[szLabel].Draw(GL_RENDER, rtBoxPlotRegion, m_PC2DataRange, color);

	//	// move to the next boxplot region
	//	rtBoxPlotRegion.MoveRight(width);
	//}
}

void CGLPCASpace::DrawGrouped_HorizontalBoxPlot(CDRect region)
{
	//CDataManager *pDM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager();
	//auto PCAOutput = pDM->GetPCA()->Get2DPCAOutputTrails()->back();

	//map<CString, vector<int/*rkey*/>> instance_rkeys;
	//map<CString, vector<double>> data;
	//MatrixXd *pData = pDM->GetDataCube()->GetNormData();
	//auto& VisibleInstances = pDM->GetDataCube()->GetRowIndexes();
	//for (auto& instance : VisibleInstances)
	//{
	//	int mrow = instance.first;
	//	int rkey = instance.second;
	//	CString szLabel = pDM->GetDataItem(rkey, pDM->GetSelectedVariable());
	//	CDPoint pt = PCAOutput[mrow];
	//	data[szLabel].push_back(pt.x); // PC-1 data
	//	instance_rkeys[szLabel].push_back(rkey);
	//}

	//// determine quartiles for boxplots
	//map<CString, CGLBoxPlot> boxplots;
	//for (auto it = data.begin(); it != data.end(); ++it)
	//{
	//	CString szLabel = (*it).first;
	//	boxplots[szLabel].Initialize(data[szLabel], &instance_rkeys[szLabel],
	//		CGLBoxPlot::HORZONTAL, m_nBoxplotBodyStyle, m_nWhiskerStyle);

	//	// computer quartiles & update if the data range is changed
	//	boxplots[szLabel].ComputePercentilesNWhiskers(m_PC1DataRange);
	//}

	//// drawing boxplots
	//double height = region.Height() / data.size(); // determine height of each boxplot
	//CDRect rtBoxPlotRegion(region.left, region.top - height, region.right, region.top);
	//for (auto it = data.begin(); it != data.end(); ++it)
	//{
	//	CString szLabel = (*it).first;
	//	CGLColor color = pDM->GetRGBColor(szLabel);
	//	color.A = 0.5f;
	//	boxplots[szLabel].Draw(GL_RENDER, rtBoxPlotRegion, m_PC1DataRange, color);

	//	double percentile = boxplots[szLabel].Get3rdPercentile();
	//	double dnQ2 = GetRelativeTargetValue(rtBoxPlotRegion.right, rtBoxPlotRegion.left,
	//		m_PC1DataRange.GetMax(), m_PC1DataRange.GetMin(), percentile);

	//	// move to next boxplot
	//	rtBoxPlotRegion.MoveDown(height);
	//}
}

void CGLPCASpace::DrawBasic_VerticalBoxPlot(CDRect region)
{
	//CDataManager *pDM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager();
	//CInteractionManager *pIM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager()->GetInteractionManager();

	//auto PCAOutput = pDM->GetPCA()->Get2DPCAOutputTrails()->back();

	//CGLBoxPlot boxplot;
	//vector<double> data; // store data values in the specified variable
	//vector<int> keys;
	//auto& VisibleInstances = pDM->GetDataCube()->GetRowIndexes();
	//for (auto& instance : VisibleInstances)
	//{
	//	int mrow = instance.first;
	//	int rkey = instance.second;
	//	CDPoint pt = PCAOutput[mrow];
	//	data.push_back(pt.y); // PC-2 data
	//	keys.push_back(rkey);
	//}
	//// init boxplot
	//boxplot.Initialize(data, &keys, CGLBoxPlot::VERTICAL, m_nBoxplotBodyStyle, m_nWhiskerStyle);
	//// computer quartiles & update
	//boxplot.ComputePercentilesNWhiskers(m_PC2DataRange); // determine data range for all columns

	//glPushMatrix();
	//glTranslatef(0.0f, 0.0f, 0.1f);
	//// drawing boxplot
	//CGLColor color(0.4f, 0.4f, 0.4f, 0.5f);
	//boxplot.Draw(GL_RENDER, region, m_PC2DataRange, color);
	//glPopMatrix();

}

void CGLPCASpace::DrawBasic_HorizontalBoxPlot(CDRect region)
{
	//CDataManager *pDM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager();
	//CInteractionManager *pIM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager()->GetInteractionManager();

	//auto PCAOutput = pDM->GetPCA()->Get2DPCAOutputTrails()->back();

	//CGLBoxPlot boxplot;
	//vector<double> data; // store data values in the specified variable
	//vector<int> keys;
	//auto& VisibleInstances = pDM->GetDataCube()->GetRowIndexes();
	//for (auto& instance : VisibleInstances)
	//{
	//	int mrow = instance.first;
	//	int rkey = instance.second;
	//	CDPoint pt = PCAOutput[mrow];
	//	data.push_back(pt.x); // PC-1 data
	//	keys.push_back(rkey);
	//}
	//// init boxplot
	//boxplot.Initialize(data, &keys, CGLBoxPlot::HORZONTAL, m_nBoxplotBodyStyle, m_nWhiskerStyle);
	//// computer quartiles & update
	//boxplot.ComputePercentilesNWhiskers(m_PC1DataRange); // determine data range for all columns

	//glPushMatrix();
	//glTranslatef(0, 0, 0.1);
	//// drawing boxplot
	//CGLColor color(0.4f, 0.4f, 0.4f, 0.5f);
	//boxplot.Draw(GL_RENDER, region, m_PC1DataRange, color);
	//glPopMatrix();

}

void CGLPCASpace::DrawBiplot()
{
	//if (m_nShowBiplot == MF_UNCHECKED) return;

	//CDataManager *pDM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager();
	//CInteractionManager *pIM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager()->GetInteractionManager();

	//map<int/*col index*/, CString> *pVariableNames = pDM->GetOrgVariableNames();
	//MatrixXd &matRightEigenVectorT = pDM->GetPCA()->GetRightEigenVectorT();
	//int PC1 = pIM->GetPCAXCoord();
	//int PC2 = pIM->GetPCAYCoord();
	//auto pColIndexes = pDM->GetDataCube()->GetColIndexes();
	//for (auto it = pColIndexes->begin(); it != pColIndexes->end(); ++it)
	//{
	//	int mcol = (*it).first;
	//	int ckey = (*it).second;
	//	CString szLabel = (*pVariableNames)[ckey];

	//	CDPoint ptCenter(m_glRenderingRegion.GetHorizontalCenter(),
	//		m_glRenderingRegion.GetVerticalCenter());
	//	CDPoint ptEigenVector = CDPoint(matRightEigenVectorT(PC1, mcol), 
	//		matRightEigenVectorT(PC2, mcol));
	//	CDPoint pt1 = ptCenter;
	//	CDPoint pt2;
	//	pt2.x = GetRelativeTargetValue(m_glRenderingRegion.right, m_glRenderingRegion.left,
	//		1.0, -1.0, ptEigenVector.x);
	//	pt2.y = GetRelativeTargetValue(m_glRenderingRegion.top, m_glRenderingRegion.bottom,
	//		1.0, -1.0, ptEigenVector.y);
	//	pt1 = CPortal::GetPadPos(pt1);
	//	pt2 = CPortal::GetPadPos(pt2);

	//	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	//	glBegin(GL_LINE_STRIP);
	//	glVertex2d(pt1.x, pt1.y);
	//	glVertex2d(pt2.x, pt2.y);
	//	glEnd();

	//	double magnitude = sqrt((matRightEigenVectorT(PC1, mcol) *
	//		matRightEigenVectorT(PC1, mcol)) +
	//		(matRightEigenVectorT(PC2, mcol) *
	//		matRightEigenVectorT(PC2, mcol)));
	//	CString title;
	//	title.Format(_T("(%.2f)"), magnitude);
	//	title = szLabel + title;

	//	m_ftglFontText.FontDrawText(CStringWrapper(title).getchardata(), pt2.x, pt2.y);
	//}
}

void CGLPCASpace::UpdateDataRange()
{
	CDataManager *pDM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager();
	CInteractionManager *pIM = pDM->GetInteractionManager();

	m_PC1DataRange = pDM->GetPCA()->GetPCAMinMax(pIM->GetPCAXCoord());
	m_PC2DataRange = pDM->GetPCA()->GetPCAMinMax(pIM->GetPCAYCoord());
}

void CGLPCASpace::DrawGLScene()
{
	if (IsReadyToRender()==FALSE) return;

	// calculate zooming time & apply
	clock_t timebuffer;
	timebuffer = clock();
	if (m_timebuffer == 0)
		m_timebuffer = timebuffer;
	long gap = timebuffer-m_timebuffer; 
 	if (gap != 0 && gap < ZOOMING_TIME)
 		return;
	m_timebuffer = timebuffer; // save temporary

	m_glRenderingRegion = *this;
	m_glRenderingRegion.DeflateRect(5, 5);
	//// draw rendering region (used for debugging purpose)
	glColor4f(0.5f, 0.0f, 0.0f, 0.3f);
	drawObject(m_glRenderingRegion, GL_LINE_STRIP);
	
	UpdateDataRange();

	DrawBoxPlot();

	DrawInstances();
	
	DrawRangeSelectionBoundary();

	DrawBiplot();

	// display mouse over instance's text information
	//CInteractionManager *pIM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager()->GetInteractionManager();
	//if (pIM->IsShowLabel() == FALSE && !m_HighlitedObjectText.IsEmpty())
	//{
	//	glColor3f(0.0f, 0.0f, 0.0f);
	//	m_ftglFontText.FontDrawText(m_HighlitedObjectText, m_dMouseX, m_dMouseY, 1.0);
	//}

	if (CPortal::PortalMotion(gap) == true)
		GetParent()->Invalidate(FALSE);
}

void CGLPCASpace::ResetClutterManager()
{
	m_ClutterManager_DataInstances.removeAll();
	m_ClutterManager_TextInfo.removeAll();
}


BOOL CGLPCASpace::OnMButtonDown(CPoint scpoint, CDPoint glpoint)
{
	if (IsReadyToInteract(glpoint) == FALSE) return FALSE;

	return TRUE;
}

BOOL CGLPCASpace::OnMouseHWheel(short zDelta, CPoint scpoint, CDPoint glpoint)
{
	if (IsReadyToInteract(glpoint) == FALSE) return FALSE;

	// horizontal movement with HWheel applied to x-positional movement only
	if (zDelta > 0)
		glpoint.x += 10; 
	else
		glpoint.x -= 10;

	ResetClutterManager();
	CPortal::PortalMotion_Panning(glpoint);

	return TRUE;
}

BOOL CGLPCASpace::OnMouseWheel(short zDelta, CPoint scpoint, CDPoint glpoint)
{	
	if (IsReadyToInteract(glpoint) == FALSE) return FALSE;

	m_dMouseX = glpoint.x;
	m_dMouseY = glpoint.y;

	ResetClutterManager();

	if (zDelta > 0)
		CPortal::PortalMotion_ZoomIn(0.2);
	else
		CPortal::PortalMotion_ZoomOut(0.2);

	return TRUE;
}

BOOL CGLPCASpace::OnLButtonDown(CPoint scpoint, CDPoint glpoint)
{
	if (IsReadyToInteract(glpoint)==FALSE) return FALSE;

	CDigitPerceptronDlg *piPCADlg = (CDigitPerceptronDlg *)AfxGetMainWnd();
	CInteractionManager *pIM = piPCADlg->GetDataManager()->GetInteractionManager();

	m_dMouseX = glpoint.x;
	m_dMouseY = glpoint.y;

	if (pIM->IsItemSelectionEnabled())
	{
		m_nDefinedMouseAction = MOUSE_RANGE_SELECTION;
		m_UserSelectedRegion.SetGLRegion(CDRect(glpoint.x, glpoint.y, glpoint.x, glpoint.y));
		m_UserSelectedRegion.SetScreenRegion(CRect(scpoint.x, scpoint.y, scpoint.x, scpoint.y));
	}
	else if (glGetModifiers() == NULL)
	{
		m_nDefinedMouseAction = MOUSE_PANNING;
	}

	return FALSE;
}

BOOL CGLPCASpace::OnRButtonDown(CDPoint glpoint) 
{
	if (IsReadyToInteract(glpoint)==FALSE) return FALSE;

	return FALSE;
}

BOOL CGLPCASpace::OnMouseMove(CPoint scpoint, CDPoint glpoint)
{
	if (IsReadyToInteract(glpoint)==FALSE) return FALSE;

	CDigitPerceptronDlg *piPCADlg = (CDigitPerceptronDlg *)AfxGetMainWnd();
	CDataManager *pDM = piPCADlg->GetDataManager();
	CInteractionManager *pIM = pDM->GetInteractionManager();

	int bReturn = FALSE;

	if (m_HighlitedObjectText.IsEmpty() == FALSE && glGetModifiers() != GL_ACTIVE_CTRL)
	{ // if the highlighted object text is not needed anymore, disable it
		m_HighlitedObjectText.Empty();
		bReturn = TRUE;
	}

	if (m_nDefinedMouseAction == MOUSE_PANNING)
	// panning only(mouse is in the outside of the object)
	{
		CPortal::PortalMotion_Panning(glpoint);
		bReturn = TRUE;
	}
	else if (m_nDefinedMouseAction == MOUSE_NODEPOS_MOVE)
	{}
	else if (m_nDefinedMouseAction == MOUSE_RANGE_SELECTION)
	{
		if (m_UserSelectedRegion.IsRectEmpty() == FALSE)
		{
			// update selected region information
			CDRect *pGLRegion = m_UserSelectedRegion.GetGLRegion();
			CRect *pScreenRegion = m_UserSelectedRegion.GetScreenRegion();

			pGLRegion->right = glpoint.x;
			pGLRegion->bottom = glpoint.y;

			pScreenRegion->right = scpoint.x;
			pScreenRegion->bottom = scpoint.y;
		
			bReturn = TRUE;
		}
	}
	else if (pIM->IsShowLabel() == FALSE && glGetModifiers() == GL_ACTIVE_CTRL)
	{
		m_dMouseX = glpoint.x;
		m_dMouseY = glpoint.y;

		if (FindHighlightedObject(glpoint.x, glpoint.y) >= 0)
			bReturn = TRUE;
	}
	return bReturn;
}

BOOL CGLPCASpace::OnLButtonUp(CDPoint glpoint) 
{
	if (IsReadyToInteract(glpoint)==FALSE) return FALSE;
	return (LButtonUp(glpoint));
}

BOOL CGLPCASpace::LButtonUp(CDPoint glpoint)
{
	if (m_nDefinedMouseAction==MOUSE_RANGE_SELECTION) 
	{
		ResetClutterManager();
		FindSelectedInstances();
	}
	m_nDefinedMouseAction = 0;

	return TRUE;
}

void CGLPCASpace::OnRButtonUp(CDPoint glpoint) 
{
	if (IsReadyToInteract(glpoint)==FALSE) return;
	m_nDefinedMouseAction = 0; // disable mouse action
}

BOOL CGLPCASpace::OnMButtonUp(CDPoint glpoint)
{
	if (IsReadyToInteract(glpoint) == FALSE) return FALSE;
	m_nDefinedMouseAction = 0; // disable mouse action
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Detecting Selected Instances
void CGLPCASpace::FindSelectedInstances()
{
	CDataManager *pDM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager();
	CInteractionManager *pIM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager()->GetInteractionManager();
	C2DMinMax XAxisMinMax = pDM->GetPCA()->GetPCAMinMax(pIM->GetPCAXCoord());
	C2DMinMax YAxisMinMax = pDM->GetPCA()->GetPCAMinMax(pIM->GetPCAYCoord());
	CDRect *pGLRegion = m_UserSelectedRegion.GetGLRegion();

	auto& p2DPCAOutput = pDM->GetPCA()->Get2DPCAOutput();
	//auto pVisibleInstances = pDM->GetDataCube()->GetRowIndexes();
	//for (auto& instance : pVisibleInstances)
	//{
	//	int mrow = instance.first;
	//	int rindex = instance.second;

	//	CDPoint pt = p2DPCAOutput[mrow];
	//	pt.x = m_glRenderingRegion.left + 
	//		(pt.x - XAxisMinMax.GetMin()) / (XAxisMinMax.GetDiff()) *  m_glRenderingRegion.Width();
	//	pt.y = m_glRenderingRegion.bottom + 
	//		(pt.y - YAxisMinMax.GetMin()) / (YAxisMinMax.GetDiff()) *  m_glRenderingRegion.Height();
	//	pt = GetPadPos(pt);

	//	if (pGLRegion->IsInside(pt.x, pt.y))
	//	{
	//		pIM->SetSelectUnselectItem(rindex);
	//	}
	//}
}

BOOL CGLPCASpace::IsReadyToInteract(CDPoint glpoint)
{
	if (IsWindowVisible() == FALSE) return FALSE;
	
	if (((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager()->IsDataLoaded()==FALSE || 
		((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager()->GetPCA()->IsPCACalculated()==FALSE ||
		this->IsInside(glpoint.x, glpoint.y)==FALSE) return FALSE;

	return TRUE;
}

BOOL CGLPCASpace::IsReadyToRender()
{
	if (IsWindowVisible() == FALSE) return FALSE;
	
	if (((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager()->IsDataLoaded()==FALSE || 
		((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager()->GetPCA()->IsPCACalculated()==FALSE) return FALSE;

	return TRUE;
}

int CGLPCASpace::FindHighlightedObject(double mx, double my)
{
	//CDataManager *pDM = ((CDigitPerceptronDlg *)AfxGetMainWnd())->GetDataManager();
	//if (pDM->GetCurrentlySelectedVariableDataType() != VARTYPE_CATEGORICAL_GROUP)
	//	return -1;

	//// highlighting object is only supported to VARTYPE_CATEGORICAL_GROUP
	//float pick_col[4];
	//glReadPixels((int)mx, (int)my, 1, 1, GL_RGBA, GL_FLOAT, pick_col);
	//CGLColor selectcolor(pick_col[0], pick_col[1], pick_col[2], pick_col[3]);

	//m_HighlitedObjectText.Empty();

	//CGLColor bacgkrouund(1.0f, 1.0f, 0.9f);
	//if (close_enough(bacgkrouund.R, selectcolor.R, 0.1f)
	//	&& close_enough(bacgkrouund.G, selectcolor.G, 0.1f)
	//		&& close_enough(bacgkrouund.B, selectcolor.B, 0.1f))
	//{ // background color - no item
	//	return -1;
	//}

	//// Because of alpha channel, 
	//// there is no exactly matched object that has the same color information.
	//double diff = 100;
	//map<CString, CGLColor> *colorMap = pDM->GetVariableColorMap();
	//for (auto it = colorMap->begin(); it != colorMap->end(); ++it)
	//{
	//	CString szLabel = (*it).first;
	//	CGLColor color = (*it).second;

	//	double tmp = abs(color.R - selectcolor.R)
	//		+ abs(color.G - selectcolor.G)
	//		+ abs(color.B - selectcolor.B);
	//	if (diff > tmp)
	//	{
	//		diff = tmp;
	//		m_HighlitedObjectText = szLabel;
	//	}
	//}
	if (!m_HighlitedObjectText.IsEmpty())
		return 1;
	return -1;
}

int CGLPCASpace::SelectObject(double mx, double my)
{
	//if (m_Instances.isDbOpened())
	//{
	//	CString query;
	//	query.Format(_T("%.2f > left AND %.2f < right AND %.2f < top AND %.2f > bottom"),
	//		mx, mx, my, my);
	//	int rkey = m_Instances.FindInstance(_T("Instances"), query);
	//	if (rkey >= 0) return rkey;
	//}
	//return -1;

	if (m_ClutterManager_DataInstances.IsEmpty() == FALSE)
		return (m_ClutterManager_DataInstances.FindObject(mx, my));
	return -1;

	//((CGLPCA*)GetParent())->PreparePickScene(pickX, pickY, m_selectBuffer);
	//if (m_ClutterManager_DataInstances.IsEmpty() == FALSE)
	//{
	//	glPushName(0);
	//	m_ClutterManager_DataInstances.Draw(GL_SELECT);
	//	glPopName();
	//}
	//return DetermineSelectedObject();
}

int CGLPCASpace::DetermineSelectedObject() 
{
	int numHits = glRenderMode(GL_RENDER);
	int offset = 0;

	int identity=-1;
	float overallMinZ = 0;
	int minZItem = 0;
	int minZType = 0;

	if (numHits > 0) 
	{
		for (int i=0; i<numHits; i++) {
			int numElements = m_selectBuffer[offset];
			offset++;
			//float minZ = (float)((float)m_selectBuffer[offset]/(float)0x7fffffff);
			float minZ = m_selectBuffer[offset];
			offset++;
			//float maxZ = (float)((float)m_selectBuffer[offset]/(float)0x7fffffff);
			float maxZ = m_selectBuffer[offset];
			offset++;

			identity = m_selectBuffer[offset]; // rendered item
			offset++;
			break;
		}
	}
	return identity;
}

BOOL CGLPCASpace::OnContextMenu(CPoint point, CDPoint glpoint)
{
	if (IsReadyToInteract(glpoint) == FALSE) return FALSE;
	if (glGetModifiers() == GL_ACTIVE_SHIFT
		|| glGetModifiers() == GL_ACTIVE_CTRL
		|| glGetModifiers() == GL_ACTIVE_ALT)
		return FALSE;

	CDataManager* pDM = ((CDigitPerceptronDlg*)AfxGetMainWnd())->GetDataManager();
	CInteractionManager* pIM = ((CDigitPerceptronDlg*)AfxGetMainWnd())->GetDataManager()->GetInteractionManager();

	// show PopUp menu
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_MENU_PROJECTIONVIEW));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	if (pIM->IsShowLabel())
		pPopup->CheckMenuItem(ID_POPUP_SHOWLABEL, MF_CHECKED | MF_BYCOMMAND);
	else
		pPopup->CheckMenuItem(ID_POPUP_SHOWLABEL, MF_UNCHECKED | MF_BYCOMMAND);

	int nCmd = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, point.x, point.y, GetParent());
	pPopup->DestroyMenu();

	switch (nCmd)
	{
	case ID_POPUP_PROJECTIONVIEW_PCPROJECTION_FLIPHORIZONTALLY:
		// data flip horizontally
		pDM->GetPCA()->FlipPCAOutputHorizontally();
		GetParent()->Invalidate(FALSE);
		break;
	case ID_POPUP_PROJECTIONVIEW_PCPROJECTION_FLIPVERTICALLY:
		// data flip vertically
		pDM->GetPCA()->FlipPCAOutputVertically();
		GetParent()->Invalidate(FALSE);
		break;
	case ID_GLYPHSIZE_INCREASE:
		pDM->GlyphSizeIncrease();
		GetParent()->Invalidate(FALSE);
		break;
	case ID_GLYPHSIZE_DECREASE:
		pDM->GlyphSizeDecrease();
		GetParent()->Invalidate(FALSE);
		break;
	case ID_POPUP_SHOWLABEL:
		pIM->SetShowLabel();
		GetParent()->Invalidate(FALSE);
		break;
		//----------------------------------------------------------------------------------
	case ID_POPUP_PCOORDVIEW_CANCEL:
		break;
	}
	return TRUE;
}
