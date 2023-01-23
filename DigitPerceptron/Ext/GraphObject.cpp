// GraphObject.cpp : implementation file
//

#include "pch.h"
#include "GraphObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TITLE_HEIGHT 40/*height*/
#define TITLE_WIDTH 30/*width*/

/////////////////////////////////////////////////////////////////////////////
// CGraphObject

CGraphObject::CGraphObject()
{
	// Set default graph title and subtitle
	m_GraphTitle = "Add graph title here";

	m_pGraphTitleFont = NULL;

	// Set default graph type
	m_GraphType = GT_2DBAR;

	// Set default graph DC and bitmap
	m_pGraphDC = NULL;
	m_pGraphBitmap = NULL;
	// Set default gradient DC and bitmap
	m_pGradientDC = NULL;
	m_pGradientBitmap = NULL;

	// Set default graph background fill and type
	m_GraphFillType = GB_SOLID;
	m_GraphGradientFillType = GRADIENT_FILL_RECT_V;

	// Set default graph gradient colors
	m_GraphGradientColor1 = RGB(0, 0, 64);
	m_GraphGradientColor2 = RGB(0, 0, 216);

	// Set default graph title and subtitle shadow
	m_GraphTitleShadow = FALSE;
	m_GraphSubtitleShadow = FALSE;

	// Set default graph title and subtitle color
	m_GraphTitleColor = RGB(0, 0, 0);
	m_GraphSubtitleColor = RGB(0, 0, 0);
}

CGraphObject::~CGraphObject()
{
	if (m_2DBarGraph) delete m_2DBarGraph;
	m_2DBarGraph = NULL;

	// Delete graph title and subtitle font
	if (m_pGraphTitleFont) {
		m_pGraphTitleFont->DeleteObject();
		delete m_pGraphTitleFont;
		m_pGraphTitleFont = NULL;
	}

	// Delete graph DC and bitmap
	if (m_pGraphDC) {
		m_pGraphDC->DeleteDC();
		delete m_pGraphDC;
		m_pGraphDC = NULL;
	}
	if (m_pGraphBitmap) {
		m_pGraphBitmap->DeleteObject();
		delete m_pGraphBitmap;
		m_pGraphBitmap = NULL;
	}

	// Delete gradient DC and bitmap
	if (m_pGradientDC) {
		m_pGradientDC->DeleteDC();
		delete m_pGradientDC;
		m_pGradientDC = NULL;
	}
	if (m_pGradientBitmap) {
		m_pGradientBitmap->DeleteObject();
		delete m_pGradientBitmap;
		m_pGradientBitmap = NULL;
	}
}


BEGIN_MESSAGE_MAP(CGraphObject, CWnd)
	//{{AFX_MSG_MAP(CGraphObject)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGraphObject message handlers

BOOL CGraphObject::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// Minimal graph size is (200x200)
	CRect rectGraph(rect);
	//if ((rectGraph.right - rectGraph.left) < 200)
	//	rectGraph.right = rectGraph.left + 200;
	//if ((rectGraph.bottom - rectGraph.top) < 200)
	//	rectGraph.bottom = rectGraph.top + 200;

	// Set graph size and position
	m_Position = CPoint(rectGraph.left, rectGraph.top);
	m_Size = CSize(rectGraph.right - rectGraph.left, rectGraph.bottom - rectGraph.top);

	// Set graph background color
	m_GraphBackgroundColor = RGB(255, 255, 255);

	m_Update = TRUE;

	// Set graph title font
	m_pGraphTitleFont = new CFont();
	m_pGraphTitleFont->CreateFont(m_Size.cy * 0.15, rectGraph.Width()/40, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Arial"));

	return CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS, rectGraph, pParentWnd, nID, NULL);
}

void CGraphObject::CreateGraph(int g_type)
{
	// Set graph type
	m_GraphType = g_type;

	// Create 2D bar graph
	// Set graph size and position
	CPoint g_point;
	g_point = CPoint(int(m_Size.cx * 0.05), int(m_Size.cy * 0.2));

	CSize g_size;
	g_size.cx = int(m_Size.cx * 0.99);
	g_size.cy = int(m_Size.cy * 0.65);

	m_2DBarGraph = new C2DBarGraph(g_point, g_size);
	m_2DBarGraph->SetFullSize(m_Size);

	// Get DC
	CDC* pDC = GetDC();

	// Create graph DC and bitmap
	m_pGraphDC = new CDC();
	m_pGraphDC->CreateCompatibleDC(pDC);
	m_pGraphBitmap = new CBitmap();
	m_pGraphBitmap->CreateCompatibleBitmap(pDC, m_Size.cx, m_Size.cy);
	m_pGraphDC->SelectObject(m_pGraphBitmap);
	// Create gradient DC and bitmap
	m_pGradientDC = new CDC();
	m_pGradientDC->CreateCompatibleDC(pDC);
	m_pGradientBitmap = new CBitmap();
	m_pGradientBitmap->CreateCompatibleBitmap(pDC, m_Size.cx, m_Size.cy);
	m_pGradientDC->SelectObject(m_pGradientBitmap);

	// Draw graph gradient background
	GRADIENT_RECT gRct;
	TRIVERTEX gVert[2];

	gVert[0].x = 0;
	gVert[0].y = 0;
	gVert[0].Red = (GetRValue(m_GraphGradientColor1) << 8);
	gVert[0].Green = (GetGValue(m_GraphGradientColor1) << 8);
	gVert[0].Blue = (GetBValue(m_GraphGradientColor1) << 8);
	gVert[0].Alpha = 0x0000;

	gVert[1].x = m_Size.cx;
	gVert[1].y = m_Size.cy;
	gVert[1].Red = (GetRValue(m_GraphGradientColor2) << 8);
	gVert[1].Green = (GetGValue(m_GraphGradientColor2) << 8);
	gVert[1].Blue = (GetBValue(m_GraphGradientColor2) << 8);
	gVert[1].Alpha = 0x0000;

	gRct.UpperLeft = 0;
	gRct.LowerRight = 1;
	::GradientFill(m_pGradientDC->m_hDC, gVert, 2, &gRct, 1, m_GraphGradientFillType);

	// Release DC
	CWnd::ReleaseDC(pDC);
}

void CGraphObject::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Get graph window DC
	CDC* pDC = GetDC();

	CRect gRect;
	// Get graph type
	// Set 2D bar graph clear rect
	//gRect = CRect(int(m_Size.cx * 0.05), int(m_Size.cy * 0.35), int(m_Size.cx * 0.05) + int(m_Size.cx * 0.6), int(m_Size.cy * 0.35) + int(m_Size.cy * 0.6));
	gRect = CRect(int(m_Size.cx * 0.05), 0.0, int(m_Size.cx * 0.05) + int(m_Size.cx * 0.95), int(m_Size.cy * 0.35) + int(m_Size.cy * 0.9));

	// Clear graph memory DC
	if (m_GraphFillType == GB_SOLID)
	{
		// Draw graph solid background
		CBrush bgBrush(m_GraphBackgroundColor);
		::FillRect(m_pGraphDC->m_hDC, gRect, (HBRUSH)bgBrush.GetSafeHandle());
		bgBrush.DeleteObject();
	}
	else
	{
		// Draw graph gradient background
		::BitBlt(m_pGraphDC->m_hDC, 0, 0, m_Size.cx, m_Size.cy, m_pGradientDC->m_hDC, 0, 0, SRCCOPY);
	}

	// Get graph type
	// Build 2D bar graph
	m_2DBarGraph->PaintGraph(m_pGraphDC->m_hDC);

	// Check graph background fill type
	if (m_GraphFillType == GB_GRADIENT)
	{
		// Draw graph title
		CFont* pOldFont = m_pGraphDC->SelectObject(m_pGraphTitleFont);
		COLORREF oldTextColor;
		m_pGraphDC->SetBkMode(TRANSPARENT);
		if (m_GraphTitleShadow == TRUE)
		{
			oldTextColor = m_pGraphDC->SetTextColor(RGB(128, 128, 128));
			m_pGraphDC->TextOut(int(m_Size.cx * 0.065), int(m_Size.cy * 0.037), m_GraphTitle, m_GraphTitle.GetLength());
			m_pGraphDC->SetTextColor(oldTextColor);
		}
		oldTextColor = m_pGraphDC->SetTextColor(m_GraphTitleColor);
		m_pGraphDC->TextOut(int(m_Size.cx * 0.05), int(m_Size.cy * 0.025), m_GraphTitle, m_GraphTitle.GetLength());
		m_pGraphDC->SetBkMode(OPAQUE);
		m_pGraphDC->SelectObject(pOldFont);
		m_pGraphDC->SetTextColor(oldTextColor);
	}
	else
	{
		// Check update flag
		if (m_Update == TRUE)
		{
			// Draw graph title
			COLORREF oldBkColor = m_pGraphDC->SetBkColor(m_GraphBackgroundColor);
			CFont* pOldFont = m_pGraphDC->SelectObject(m_pGraphTitleFont);
			COLORREF oldTextColor;
			m_pGraphDC->SetBkMode(TRANSPARENT);
			if (m_GraphTitleShadow == TRUE)
			{
				oldTextColor = m_pGraphDC->SetTextColor(RGB(128, 128, 128));
				m_pGraphDC->TextOut(int(m_Size.cx * 0.065), int(m_Size.cy * 0.037), m_GraphTitle, m_GraphTitle.GetLength());
				m_pGraphDC->SetTextColor(oldTextColor);
			}
			oldTextColor = m_pGraphDC->SetTextColor(m_GraphTitleColor);
			m_pGraphDC->TextOut(int(m_Size.cx * 0.05), int(m_Size.cy * 0.025), m_GraphTitle, m_GraphTitle.GetLength());
			m_pGraphDC->SetBkMode(OPAQUE);
			m_pGraphDC->SelectObject(pOldFont);
			m_pGraphDC->SetBkColor(oldBkColor);
			m_pGraphDC->SetTextColor(oldTextColor);

			// Clear update flag
			m_Update = FALSE;
		}
	}

	// Draw graph DC
	CRect rect;
	GetClientRect(&rect);
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), m_pGraphDC, 0, 0, SRCCOPY);

	// Relese graph window DC
	CWnd::ReleaseDC(pDC);

	// Do not call CWnd::OnPaint() for painting messages
}

void CGraphObject::SetGraphBackgroundColor(COLORREF g_bgcolor)
{
	// Set graph background color
	m_GraphBackgroundColor = g_bgcolor;

	// Get graph type
	// Set 2D bar graph background color
	m_2DBarGraph->SetGraphBkColor(g_bgcolor);

	Invalidate(FALSE);
}

COLORREF CGraphObject::GetGraphBackgroundColor()
{
	return m_GraphBackgroundColor;
}

BOOL CGraphObject::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	m_Update = TRUE;

	CBrush bgBrush(m_GraphBackgroundColor);
	CBrush* pOldBrush = pDC->SelectObject(&bgBrush);
	m_pGraphDC->SelectObject(&bgBrush);

	CRect rect;
	pDC->GetClipBox(&rect);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	m_pGraphDC->GetClipBox(&rect);
	m_pGraphDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);
	m_pGraphDC->SelectObject(pOldBrush);
	bgBrush.DeleteObject();
	return TRUE;
}

void CGraphObject::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	int result;

	// Get graph type
	// Refresh 2D bar graph
	//m_2DBarGraph->UpdateAnimation();
	//result = m_2DBarGraph->GetGraphAnimationPercent();
	//if (result >= 100)
	//{
	//	CBrush bgBrush(m_GraphBackgroundColor);
	//	CRect rect = CRect(int(m_Size.cx * 0.05), int(m_Size.cy * 0.35), int(m_Size.cx * 0.05) + int(m_Size.cx * 0.5), int(m_Size.cy * 0.35) + int(m_Size.cy * 0.5));
	//	::FillRect(m_pGraphDC->m_hDC, rect, (HBRUSH)bgBrush.GetSafeHandle());
	//	bgBrush.DeleteObject();
	//}

	// Clear update flag
	m_Update = FALSE;

	Invalidate(FALSE);

	CWnd::OnTimer(nIDEvent);
}

void CGraphObject::SetGraphTitle(CString g_title)
{
	// Set graph title
	m_GraphTitle = g_title;

	// Set update flag
	m_Update = TRUE;

	Invalidate(FALSE);
}

void CGraphObject::SetGraphSize(CSize g_size)
{
	// Set graph size
	m_Size = g_size;

	// Check graph size
	//if (m_Size.cx < 200)
	//	m_Size.cx = 200;
	//if (m_Size.cy < 200)
	//	m_Size.cy = 200;

	// Set graph position
	CPoint g_position = CPoint(int(m_Size.cx * 0.05), int(m_Size.cy * 0.35));
	g_position = CPoint(int(m_Size.cx * 0.05), int(m_Size.cy * 0.2));

	// Get graph type
	// Set new 2D bar graph size and position
	g_size.cx = int(m_Size.cx * 0.99);
	g_size.cy = int(m_Size.cy * 0.65);
	m_2DBarGraph->SetGraphSize(g_size);

	//	m_2DBarGraph->SetGraphSize(CSize(int(m_Size.cx * 0.7), int(m_Size.cy * 0.5)));
	m_2DBarGraph->SetGraphPosition(g_position);
	m_2DBarGraph->SetFullSize(m_Size);

	// Set graph title font
	if (m_pGraphTitleFont) {
		m_pGraphTitleFont->DeleteObject();
		delete m_pGraphTitleFont;
		m_pGraphTitleFont = new CFont();
		//m_pGraphTitleFont->CreateFont(int(m_Size.cy * 0.2), int(m_Size.cx * 0.04), 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Arial"));
		m_pGraphTitleFont->CreateFont(m_Size.cy * 0.15, m_Size.cx / 40, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Arial"));
	}

	// Set new graph window size
	::SetWindowPos(this->m_hWnd, HWND_NOTOPMOST, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, SWP_NOZORDER);

	// Set update flag
	m_Update = TRUE;

	// Delete graph DC and bitmap
	m_pGraphDC->DeleteDC();
	delete m_pGraphDC;
	m_pGraphDC = NULL;
	m_pGraphBitmap->DeleteObject();
	delete m_pGraphBitmap;
	m_pGraphBitmap = NULL;
	// Delete gradient DC and bitmap
	m_pGradientDC->DeleteDC();
	delete m_pGradientDC;
	m_pGradientDC = NULL;
	m_pGradientBitmap->DeleteObject();
	delete m_pGradientBitmap;
	m_pGradientBitmap = NULL;

	// Get DC
	CDC* pDC = GetDC();

	// Create graph DC and bitmap
	m_pGraphDC = new CDC();
	m_pGraphDC->CreateCompatibleDC(pDC);
	m_pGraphBitmap = new CBitmap();
	m_pGraphBitmap->CreateCompatibleBitmap(pDC, m_Size.cx, m_Size.cy);
	m_pGraphDC->SelectObject(m_pGraphBitmap);
	// Create gradient DC and bitmap
	m_pGradientDC = new CDC();
	m_pGradientDC->CreateCompatibleDC(pDC);
	m_pGradientBitmap = new CBitmap();
	m_pGradientBitmap->CreateCompatibleBitmap(pDC, m_Size.cx, m_Size.cy);
	m_pGradientDC->SelectObject(m_pGradientBitmap);

	// Draw graph gradient background
	GRADIENT_RECT gRct;
	TRIVERTEX gVert[2];

	gVert[0].x = 0;
	gVert[0].y = 0;
	gVert[0].Red = (GetRValue(m_GraphGradientColor1) << 8);
	gVert[0].Green = (GetGValue(m_GraphGradientColor1) << 8);
	gVert[0].Blue = (GetBValue(m_GraphGradientColor1) << 8);
	gVert[0].Alpha = 0x0000;

	gVert[1].x = m_Size.cx;
	gVert[1].y = m_Size.cy;
	gVert[1].Red = (GetRValue(m_GraphGradientColor2) << 8);
	gVert[1].Green = (GetGValue(m_GraphGradientColor2) << 8);
	gVert[1].Blue = (GetBValue(m_GraphGradientColor2) << 8);
	gVert[1].Alpha = 0x0000;

	gRct.UpperLeft = 0;
	gRct.LowerRight = 1;
	::GradientFill(m_pGradientDC->m_hDC, gVert, 2, &gRct, 1, m_GraphGradientFillType);

	// Release DC
	CWnd::ReleaseDC(pDC);

	Invalidate(FALSE);
}

void CGraphObject::SetGraphPosition(CPoint g_position)
{
	// Set new graph position
	m_Position = g_position;

	// Set new graph window position
	::SetWindowPos(this->m_hWnd, HWND_NOTOPMOST, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, SWP_NOZORDER);

	// Set update flag
	m_Update = TRUE;

	Invalidate(FALSE);
}

void CGraphObject::Add2DBarGraphSegment(CString s_text)
{
	// Add new 2D bar graph segment
	m_2DBarGraph->AddSegment(s_text);
}

void CGraphObject::Delete2DBarGraphSegment(int s_index)
{
	// Delete 2D bar graph segment
	m_2DBarGraph->DeleteSegment(s_index);
}

void CGraphObject::Set2DBarGraphValue(int s_segment, int s_series, float s_value)
{
	// Set 2D bar graph value
	m_2DBarGraph->SetValue(s_segment, s_series, s_value);
}

void CGraphObject::Add2DBarGraphSeries(CString s_text, COLORREF s_color)
{
	// Add new 2D bar graph serie
	m_2DBarGraph->AddSeries(s_text, s_color);
}

void CGraphObject::Detele2DBarGraphSeries(int s_index)
{
	// Delete 2D bar graph serie
	m_2DBarGraph->DeleteSeries(s_index);
}

void CGraphObject::SetGraphFillType(int g_fill)
{
	// Set graph background fill type
	switch (g_fill)
	{
		// Set solid graph background fill
	case GB_SOLID:
		m_GraphFillType = GB_SOLID;
		break;

		// Set gradient graph background fill
	case GB_GRADIENT:
		m_GraphFillType = GB_GRADIENT;
		break;

		// Set default graph background fill (GB_SOLID)
	default:
		m_GraphFillType = GB_SOLID;
		break;
	}
}

int CGraphObject::GetGraphFillType()
{
	return m_GraphFillType;
}

void CGraphObject::SetGraphGradientColors(COLORREF g_col1, COLORREF g_col2)
{
	// Set graph background gradient colors
	m_GraphGradientColor1 = g_col1;
	m_GraphGradientColor2 = g_col2;

	// Draw graph gradient background
	GRADIENT_RECT gRct;
	TRIVERTEX gVert[2];

	gVert[0].x = 0;
	gVert[0].y = 0;
	gVert[0].Red = (GetRValue(m_GraphGradientColor1) << 8);
	gVert[0].Green = (GetGValue(m_GraphGradientColor1) << 8);
	gVert[0].Blue = (GetBValue(m_GraphGradientColor1) << 8);
	gVert[0].Alpha = 0x0000;

	gVert[1].x = m_Size.cx;
	gVert[1].y = m_Size.cy;
	gVert[1].Red = (GetRValue(m_GraphGradientColor2) << 8);
	gVert[1].Green = (GetGValue(m_GraphGradientColor2) << 8);
	gVert[1].Blue = (GetBValue(m_GraphGradientColor2) << 8);
	gVert[1].Alpha = 0x0000;

	gRct.UpperLeft = 0;
	gRct.LowerRight = 1;
	::GradientFill(m_pGradientDC->m_hDC, gVert, 2, &gRct, 1, m_GraphGradientFillType);
}

void CGraphObject::SetGradientFillType(int f_type)
{
	// Set gradient fill type
	switch (f_type)
	{
		// Set horizontal gradient fill type
	case FT_HORIZONTAL:
		m_GraphGradientFillType = GRADIENT_FILL_RECT_H;
		break;

		// Set vertical gradient fill type
	case FT_VERTICAL:
		m_GraphGradientFillType = GRADIENT_FILL_RECT_V;
		break;

		// Set default gradient fill type (FT_VERTICAL)
	default:
		m_GraphGradientFillType = GRADIENT_FILL_RECT_H;
		break;
	}

	// Draw graph gradient background
	GRADIENT_RECT gRct;
	TRIVERTEX gVert[2];

	gVert[0].x = 0;
	gVert[0].y = 0;
	gVert[0].Red = (GetRValue(m_GraphGradientColor1) << 8);
	gVert[0].Green = (GetGValue(m_GraphGradientColor1) << 8);
	gVert[0].Blue = (GetBValue(m_GraphGradientColor1) << 8);
	gVert[0].Alpha = 0x0000;

	gVert[1].x = m_Size.cx;
	gVert[1].y = m_Size.cy;
	gVert[1].Red = (GetRValue(m_GraphGradientColor2) << 8);
	gVert[1].Green = (GetGValue(m_GraphGradientColor2) << 8);
	gVert[1].Blue = (GetBValue(m_GraphGradientColor2) << 8);
	gVert[1].Alpha = 0x0000;

	gRct.UpperLeft = 0;
	gRct.LowerRight = 1;
	::GradientFill(m_pGradientDC->m_hDC, gVert, 2, &gRct, 1, m_GraphGradientFillType);
}

int CGraphObject::GetGradientFillType()
{
	return m_GraphGradientFillType;
}

void CGraphObject::SetGraphTitleShadow(BOOL t_shadow)
{
	// Set graph title shadow
	m_GraphTitleShadow = t_shadow;
}

void CGraphObject::SetGraphSubtitleShadow(BOOL s_shadow)
{
	// Set graph subtitle shadow
	m_GraphSubtitleShadow = s_shadow;
}

BOOL CGraphObject::GetGraphTitleShadow()
{
	return m_GraphTitleShadow;
}

BOOL CGraphObject::GetGraphSubtitleShadow()
{
	return m_GraphSubtitleShadow;
}

void CGraphObject::SetGraphTitleColor(COLORREF t_color)
{
	// Set graph title color
	m_GraphTitleColor = t_color;
}

void CGraphObject::SetGraphSubtitleColor(COLORREF s_color)
{
	// Set graph subtitle color
	m_GraphSubtitleColor = s_color;
}

COLORREF CGraphObject::GetGraphTitleColor()
{
	return m_GraphTitleColor;
}

COLORREF CGraphObject::GetGraphSubtitleColor()
{
	return m_GraphSubtitleColor;
}

void CGraphObject::SetGraphLabelsColor(COLORREF l_color)
{
	// Get graph type
	// Set 2D bar graph labels color
	m_2DBarGraph->SetLabelsColor(l_color);

	Invalidate(FALSE);
}

COLORREF CGraphObject::GetGraphLabelsColor()
{
	COLORREF result;

	// Get graph type
	// Get 2D bar graph labels color
	result = m_2DBarGraph->GetLabelsColor();
	return result;
}

