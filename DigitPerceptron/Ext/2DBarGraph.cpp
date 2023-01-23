// 2DBarGraph.cpp: implementation of the C2DBarGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "2DBarGraph.h"
#include "GraphObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define COLOR_RED RGB(255, 0, 0)
#define COLOR_BLUE RGB(0, 0, 255)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2DBarGraph::C2DBarGraph(CPoint g_position, CSize g_size)
{
	// Set graph position and size
	m_Position = g_position;
	m_Size = g_size;

	// Set graph segments and series number
	m_SegmentsNumber = 0;
	m_Segments = NULL;
	m_SeriesNumber = 0;
	m_Series = NULL;

	// Set graph series font
	m_SegmentFont = NULL;

	// Set graph value font
	m_ValueFont = new CFont();
	m_ValueFont->CreateFont(m_Size.cy * 0.22, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Arial") );

	// Set default graph background color
	m_GraphBkColor = RGB(255,255,2555);

	// Set default graph labels color
	m_LabelsColor = RGB(0,0,0);

	// Set default graph legend background color
	m_LegendBackgroundColor = RGB(255,255,255);
	m_LegendTextColor = RGB(0,0,0);

	color_indicator = 0;
	emphasize_index = -1;
}

C2DBarGraph::~C2DBarGraph()
{
	// Delete graph segments and series
	ClearGraph();

	// Delete graph segment font
	m_SegmentFont = NULL;

	// Delete graph value font
	m_ValueFont->DeleteObject();
	delete m_ValueFont;
	m_ValueFont = NULL;
}

void C2DBarGraph::ClearGraph()
{
	_2DBarGraphSeries *curr_ser, *prev_ser;
	_2DBarGraphSegments *curr_seg, *prev_seg;
	_2DBarGraphValues *curr_val, *prev_val;

	// Delete all graph series
	curr_ser = m_Series;
	while ( curr_ser != NULL )
	{
		// Delete all graph values
		curr_val = curr_ser->values;
		while ( curr_val != NULL )
		{
			prev_val = curr_val;
			curr_val = curr_val->next;
			delete prev_val;
		}

		prev_ser = curr_ser;
		curr_ser = curr_ser->next;
		delete prev_ser;
	}

	// Delete all graph segments
	curr_seg = m_Segments;
	while ( curr_seg != NULL )
	{
		prev_seg = curr_seg;
		curr_seg = curr_seg->next;
		delete prev_seg;
	}

	// Set graph segment and series parameters
	m_Segments = NULL;
	m_SegmentsNumber = 0;
	m_Series = NULL;
	m_SeriesNumber = 0;
}

void C2DBarGraph::PaintGraph(HDC hDC)
{
	int seg_width, seg_height;
	CPoint ser_start, seg_font_pos;
	int ser_width;
	float max_value;

	// Set graph step parameters
	//int seg_area = int(m_Size.cx*0.6);
	int seg_area = int(m_Size.cx * 0.9);
	int seg_step = int(double(seg_area)/(4*double(m_SegmentsNumber)));
	int ser_step = int(double(seg_step)/8 + 0.5);

	// Set segment font height
	int seg_number = m_SegmentsNumber;
	if ( seg_number == 1 )
		seg_number++;
	int label_font_height = m_Size.cy * 0.22;// int((double(seg_area) / double(seg_number)) * 0.38 + 0.5);

	_2DBarGraphSegments *curr_seg;
	_2DBarGraphSeries *c_ser, *curr_ser, *cc_ser;
	_2DBarGraphValues *c_val, *curr_val;
	_2DBarGraphSegments *c_seg;

	HBRUSH hOldSerBrush, hSerBrush;
	HFONT hOldSegFont, hOldValFont;

	// Calculate graph x and y coordinate system begining
	int startX = m_Position.x + MARGIN_LEFT;	// int(m_Size.cx * MARGIN_RATIO_LEFT);
	int startY = m_Position.y + int(m_Size.cy * 0.95);
	// Draw x coordinate axis
	::MoveToEx( hDC, startX, startY, NULL );
	::LineTo( hDC, m_Size.cx, startY );
	// Draw x coordinate axis arrow
	::MoveToEx( hDC, m_Size.cx, startY, NULL );
	::LineTo( hDC, int(m_Size.cx*0.98), startY-int(m_Size.cy*0.02) );
	::MoveToEx( hDC, m_Size.cx, startY, NULL );
	::LineTo( hDC, int(m_Size.cx*0.98), startY+int(m_Size.cy*0.02) );
	// Draw y coordinate axis
	::MoveToEx( hDC, startX, startY, NULL );
	::LineTo( hDC, startX, m_Position.y + int(m_Size.cy*0.05) );
	// Draw y coordinate axis arrow
	::MoveToEx( hDC, startX, m_Position.y + int(m_Size.cy*0.05), NULL );
	::LineTo( hDC, startX - int(m_Size.cx*0.015), m_Position.y + int(m_Size.cy*0.085) );
	::MoveToEx( hDC, startX, m_Position.y + int(m_Size.cy*0.05), NULL );
	::LineTo( hDC, startX + int(m_Size.cx*0.015), m_Position.y + int(m_Size.cy*0.085) );

	// Check segments number
	if ( ( m_SegmentsNumber > 0 ) && ( m_SeriesNumber > 0 ) )
	{
		// Calculate segments width
		seg_width = int((double(seg_area) - (m_SegmentsNumber-1)*seg_step + m_SegmentsNumber*(m_SeriesNumber-1)*ser_step)/double(m_SegmentsNumber));

		// Calculate series start point and width 
		if ( m_SegmentsNumber > 1 )
			ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
		else
			ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
		ser_width = int((double(seg_width) - (m_SeriesNumber-1)*ser_step)/double(m_SeriesNumber));

		// Get max series value
		//max_value = 0;
		//c_ser = m_Series;
		//c_val = NULL;
		//curr_seg = m_Segments;
		//while ( curr_seg != NULL )
		//{
		//	while ( c_ser != NULL )
		//	{
		//		// Find segment value
		//		c_val = c_ser->values;
		//		while ( c_val->index != curr_seg->index )
		//			c_val = c_val->next;
		//		// Check max value
		//		if ( c_val->value > max_value )
		//			max_value = c_val->value;

		//		// Get next series
		//		c_ser = c_ser->next;
		//	}

		//	// Get next segment
		//	curr_seg = curr_seg->next;
		//}

		max_value = 1.0;

		// Draw value text in axes
		hOldValFont = (HFONT)::SelectObject( hDC, m_ValueFont->GetSafeHandle() );
		::SetBkMode( hDC, TRANSPARENT );
		::SetTextAlign( hDC, TA_RIGHT );
		COLORREF oldTextColor = ::SetTextColor( hDC, m_LabelsColor );
		CString str_max;
		str_max.Format(_T("%.1f"), max_value );
		::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.85), str_max, str_max.GetLength() );
		str_max.Format(_T("%.1f"), double(max_value)/2.0 );
		::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.45), str_max, str_max.GetLength() );
		str_max = "0";
		::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.05), str_max, str_max.GetLength() );
		::SetTextAlign( hDC, TA_LEFT );
		::SetBkMode( hDC, OPAQUE );
		::SelectObject( hDC, hOldValFont );

		// Draw graph segments
		curr_seg = m_Segments;
		while ( curr_seg != NULL )
		{
			// Set new graph legend font
			m_SegmentFont = new CFont();
			m_SegmentFont->CreateFont(label_font_height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
				DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Arial") );
			
			// Draw segment text
			hOldSegFont = (HFONT)::SelectObject( hDC, m_SegmentFont->GetSafeHandle() );
			::SetBkMode( hDC, TRANSPARENT );
			::SetTextAlign( hDC, TA_CENTER );
			::TextOut( hDC, ser_start.x + int((seg_width + m_SeriesNumber*ser_step)/2), startY+int(m_Size.cy*0.01), curr_seg->text, curr_seg->text.GetLength() );
			::SetTextAlign( hDC, TA_LEFT );
			::SetBkMode( hDC, OPAQUE );
			::SelectObject( hDC, hOldSegFont );
			m_SegmentFont->DeleteObject();
			delete m_SegmentFont;

			// Draw graph series
			curr_ser = m_Series;
			curr_val = NULL;
			while ( curr_ser != NULL)
			{
				if (emphasize_index != -1) {

					// Draw graph series values
					curr_val = curr_ser->values;
					while (curr_val->index != curr_seg->index)
						curr_val = curr_val->next;
					// Calculate value height
					seg_height = int((double(curr_val->value) / double(max_value)) * (m_Size.cy * 0.8));
					// Draw value rectangle
					if (color_indicator != 0 && (emphasize_index + 1) == curr_val->index)
					{
						if (color_indicator == 1)
							hSerBrush = ::CreateSolidBrush(COLOR_RED);
						else
							hSerBrush = ::CreateSolidBrush(COLOR_BLUE);
					}
					else {
						hSerBrush = ::CreateSolidBrush(curr_ser->color);
					}
					hOldSerBrush = (HBRUSH)::SelectObject(hDC, hSerBrush);
					::Rectangle(hDC, ser_start.x, ser_start.y - seg_height, ser_start.x + ser_width, ser_start.y);
					::SelectObject(hDC, hOldSerBrush);
					::DeleteObject(hSerBrush);

					// Draw value text
					hOldValFont = (HFONT)::SelectObject(hDC, m_ValueFont->GetSafeHandle());
					::SetBkMode(hDC, TRANSPARENT);
					::SetTextAlign(hDC, TA_RIGHT);
					COLORREF oldTextColor2 = ::SetTextColor(hDC, COLORREF(RGB(128, 0, 0)));
					CString str_value;
					str_value.Format(_T("%.2f"), curr_val->value);

					::TEXTMETRIC metrics;
					::GetTextMetrics(hDC, &metrics);

					//CDC desktopDC;
					//desktopDC.Attach(::GetDC(0));
					//desktopDC.SelectObject(&hOldValFont);
					//::TEXTMETRIC metrics;
					//desktopDC.GetTextMetrics(&metrics);
					int lineHeight = metrics.tmHeight * 1.1;
					int lineWidth = (metrics.tmAveCharWidth + metrics.tmExternalLeading) * str_value.GetLength() / 2;
					::TextOut(hDC, ser_start.x + (ser_width - lineWidth), ser_start.y - seg_height - lineHeight, str_value, str_value.GetLength());
					

					::SetTextAlign(hDC, TA_LEFT);
					::SetBkMode(hDC, OPAQUE);
					::SelectObject(hDC, hOldValFont);
					::SetTextColor(hDC, oldTextColor2);
				}

				// Get next series
				curr_ser = curr_ser->next;

				// Set next series start point
				if ( curr_ser != NULL )
					ser_start.x += ser_width + ser_step;
				else
					ser_start.x += ser_width;
			}

			// Get next segment
			curr_seg = curr_seg->next;

			// Set next series start point
			ser_start.x += seg_step;
		}
		::SetTextColor( hDC, oldTextColor );
	}
}

void C2DBarGraph::SetGraphBkColor(COLORREF g_bkColor)
{
	// Set graph background color
	m_GraphBkColor = g_bkColor;
}

void C2DBarGraph::SetGraphPosition(CPoint g_position)
{
	// Set new graph position
	m_Position = g_position;
}

void C2DBarGraph::SetGraphSize(CSize g_size)
{
	// Set new graph size
	m_Size = g_size;

	// Set new graph value font size
	m_ValueFont->DeleteObject();
	delete m_ValueFont;
	m_ValueFont = new CFont();
	m_ValueFont->CreateFont( int(m_Size.cy*0.1), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Arial") );
}

void C2DBarGraph::AddSegment(CString s_text)
{
	// Increment segments number
	m_SegmentsNumber++;

	// Create new graph segment
	_2DBarGraphSegments* newSegment = new _2DBarGraphSegments;
	newSegment->index = m_SegmentsNumber;
	newSegment->text = s_text;
	newSegment->next = NULL;

	// Add new segment to the graph
	_2DBarGraphSegments* curr_seg = m_Segments;
	if ( curr_seg == NULL )
		m_Segments = newSegment;
	else
	{
		while ( curr_seg->next != NULL )
			curr_seg = curr_seg->next;
		curr_seg->next = newSegment;
	}

	// Check graph series number
	if ( m_SeriesNumber > 0 )
	{
		// Create new graph value
		_2DBarGraphValues* newValue = new _2DBarGraphValues;
		newValue->index = m_SegmentsNumber;
		newValue->value = 0;
		newValue->next = NULL;

		// Add new value to the graph segments
		_2DBarGraphSeries* curr_ser = m_Series;
		_2DBarGraphValues* curr_val = NULL;
		while ( curr_ser != NULL )
		{
			// Add new graph value to the graph
			curr_val = curr_ser->values;
			if ( curr_val == NULL )
				curr_ser->values = newValue;
			else
			{
				while ( curr_val->next != NULL )
					curr_val = curr_val->next;
				curr_val->next = newValue;
			}

			// Get next series
			curr_ser = curr_ser->next;
		}
	}
}

void C2DBarGraph::DeleteSegment(int s_index)
{
	// Check index bounds
	if ( ( s_index < 1 ) || ( s_index > m_SegmentsNumber ) )
		AfxMessageBox(_T("Index is out of bounds..."), MB_OK, NULL );
	else
	{
		// Decrement segments number
		m_SegmentsNumber--;

		// Delete segment from the graph
		_2DBarGraphSegments* curr_seg = m_Segments;
		_2DBarGraphSegments* prev_seg = NULL;
		if ( curr_seg != NULL )
		{
			// Find graph segment
			while ( curr_seg->index != s_index )
			{
				prev_seg = curr_seg;
				curr_seg = curr_seg->next;
			}

			// Delete first graph segment
			if ( curr_seg == m_Segments )
				m_Segments = m_Segments->next;
			// Delete other graph segment
			else
				prev_seg->next = curr_seg->next;

			// Delete graph segment
			delete curr_seg;

			// Delete value from the graph series
			_2DBarGraphSeries* curr_ser = m_Series;
			_2DBarGraphValues* curr_val = NULL;
			_2DBarGraphValues* prev_val = NULL;
			while ( curr_ser != NULL )
			{
				// Delete graph value from the graph
				curr_val = curr_ser->values;
				prev_val = NULL;
				if ( curr_val == NULL )
				{
					// Find graph series value
					while ( curr_val->index != s_index )
					{
						prev_val = curr_val;
						curr_val = curr_val->next;
					}

					// Delete first graph series value
					if ( curr_val == curr_ser->values )
						curr_ser->values = curr_ser->values->next;
					// Delete other graph series value
					else
						prev_val->next = curr_val->next;

					// Delete graph value
					delete curr_val;
				}

				// Get net series
				curr_ser = curr_ser->next;
			}
		}
	}
}

void C2DBarGraph::SetValue(int s_segment, int s_series, float s_value)
{
	// Check segment bounds
	if ( ( s_segment < 1 ) || ( s_segment > m_SegmentsNumber ) )
		AfxMessageBox(_T("Segments index is out of bounds..."), MB_OK, NULL );
	else if ( ( s_series < 1 ) || ( s_series > m_SeriesNumber ) )
		AfxMessageBox(_T("Series index is out of bounds..."), MB_OK, NULL );
	else
	{
		// Find graph series
		_2DBarGraphSeries* curr_ser = m_Series;
		while ( curr_ser->index != s_series )
			curr_ser = curr_ser->next;

		// Find graph series value
		_2DBarGraphValues* curr_val = curr_ser->values;
		while ( curr_val->index != s_segment )
			curr_val = curr_val->next;

		// Set graph series value
		curr_val->value = s_value;
	}
}

void C2DBarGraph::AddSeries(CString s_text, COLORREF s_color)
{
	// Check segments number
	if ( m_SegmentsNumber > 0 )
	{
		// Increment series number
		m_SeriesNumber++;

		// Create new graph series
		_2DBarGraphSeries* newSeries = new _2DBarGraphSeries;
		newSeries->index = m_SeriesNumber;
		newSeries->text = s_text;
		newSeries->color = s_color;
		newSeries->values = NULL;
		newSeries->next = NULL;

		// Add values to new graph series
		_2DBarGraphValues* curr_val = newSeries->values;
		_2DBarGraphValues* newValue = NULL;
		for ( int index=1; index<=m_SegmentsNumber; index++ )
		{
			// Create new graph series value
			newValue = new _2DBarGraphValues;
			newValue->index = index;
			newValue->value = 0;
			newValue->next = NULL;

			// Add new graph value to the graph
			if ( curr_val == NULL )
				newSeries->values = newValue;
			else
				curr_val->next = newValue;
			curr_val = newValue;
		}

		// Add new graph series to the graph
		_2DBarGraphSeries* curr_ser = m_Series;
		if ( curr_ser == NULL )
			m_Series = newSeries;
		else
		{
			while ( curr_ser->next != NULL )
				curr_ser = curr_ser->next;
			curr_ser->next = newSeries;
		}
	}
	else
		AfxMessageBox(_T("No graph segments..."), MB_OK, NULL );
}

void C2DBarGraph::DeleteSeries(int s_index)
{
	// Check series bounds
	if ( ( s_index < 1 ) || ( s_index > m_SeriesNumber ) )
		AfxMessageBox(_T("Series index is out of bounds..."), MB_OK, NULL );
	else
	{
		// Decrement series number
		m_SeriesNumber--;

		// Find graph series
		_2DBarGraphSeries* curr_ser = m_Series;
		_2DBarGraphSeries* prev_ser = NULL;
		while ( curr_ser->index != s_index )
		{
			prev_ser = curr_ser;
			curr_ser = curr_ser->next;
		}

		// Delete graph series values
		_2DBarGraphValues* curr_val = curr_ser->values;
		_2DBarGraphValues* prev_val = NULL;
		while ( curr_val != NULL )
		{
			prev_val = curr_val;
			curr_val = curr_val->next;
			delete prev_val;
		}

		// Delete graph series
		if ( curr_ser == m_Series )
			m_Series = m_Series->next;
		else
			prev_ser->next = curr_ser->next;
		delete curr_ser;
	}
}

void C2DBarGraph::SetFullSize(CSize full_size)
{
	// Set full size
	m_FullSize = full_size;
}

void C2DBarGraph::SetLabelsColor(COLORREF l_color)
{
	// Set graph labels color
	m_LabelsColor = l_color;
}

COLORREF C2DBarGraph::GetLabelsColor()
{
	return m_LabelsColor;
}

void C2DBarGraph::SetLegendBackgroundColor(COLORREF l_bgcolor)
{
	// Set legend background color
	m_LegendBackgroundColor = l_bgcolor;
}

COLORREF C2DBarGraph::GetLegendBackgroundColor()
{
	return m_LegendBackgroundColor;
}

void C2DBarGraph::SetLegendTextColor(COLORREF t_color)
{
	// Set legend text color
	m_LegendTextColor = t_color;
}

COLORREF C2DBarGraph::GetLegendTextColor()
{
	return m_LegendTextColor;
}
