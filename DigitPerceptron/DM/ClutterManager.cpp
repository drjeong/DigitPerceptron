// ClutterManager.cpp: implementation of the CTextClutterManager class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ClutterManager.h"
#include "ftGLFontWrapper.h"
#include "StringWrapper.h"
#include "DataFunctions.h"

#include <GL/Gl.h>
#include <GL/Glu.h>
#include "gl2ps.h"

const double gamma = 2.2;
extern BOOL g_bVectorDrawing2File; // defined in GLPCA.CPP used for generating SVG or PDF capture file

//////////////////////////////////////////////////////////////////////

BOOL RectsOverlap(double x0, double y0, double w0, double h0, 
	double x2, double y2, double w2, double h2)
{
	if ((x0 > (x2 + w2)) || ((x0 + w0) < x2)) return FALSE;
	if ((y0 > (y2 + h2)) || ((y0 + h0) < y2)) return FALSE;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CDataItemClutterManager
CDataItemClutterManager::CDataItemClutterManager()
{
	m_dVisualComplexity = 0.25; // default: medium visual complexity
	m_pFont=new CftGLFontWrapper();
	m_pFont->Initialize(10);
}

CDataItemClutterManager::~CDataItemClutterManager()
{
	removeAll(); delete m_pFont;
}

void CDataItemClutterManager::Init(CDRect region)
{
	m_glScreenSpace = region;

	m_RenderingBuffer.resize((int)region.Width(), (int)region.Height());
	m_RenderingBuffer.setZero(); // init with zero

	m_vecManagedSpace.clear();
}

BOOL CDataItemClutterManager::IsPossible2AddObject(CDRect rt)
{
	CDRect rtTmp = rt;

	// normalize for the rendering region 0 ~ width / 0 ~ height 
	rtTmp.MoveLeft(m_glScreenSpace.left);
	rtTmp.MoveDown(m_glScreenSpace.bottom);

	// scale based on visual complexity
	rtTmp.DeflateRect(rtTmp.Width()*m_dVisualComplexity, rtTmp.Height()*m_dVisualComplexity);

	// convert to int type
	int xPos  = (int)rtTmp.left;
	int yPos = (int)rtTmp.top;
	int width = rtTmp.Width();
	int height = rtTmp.Height();

	// check the values maintains within the matrix range
	if (xPos >= m_RenderingBuffer.rows()) xPos = m_RenderingBuffer.rows() - 1;
	if (xPos < 0) xPos = 0;
	if (yPos >= m_RenderingBuffer.cols()) yPos = m_RenderingBuffer.cols() - 1;
	if (yPos < 0) yPos = 0;
	if (xPos + width > m_RenderingBuffer.rows()) width = m_RenderingBuffer.rows() - xPos - 1;
	if (yPos + height > m_RenderingBuffer.cols()) height = m_RenderingBuffer.cols() - yPos - 1;

	Eigen::MatrixXi block = m_RenderingBuffer.block(xPos, yPos, width, height);
	int sum = block.sum();
	if (sum > 0) 
		return FALSE;

	// mark the rendered region with ones
	block.setOnes();
	m_RenderingBuffer.block(xPos, yPos, width, height) = block;

	return TRUE;
}

BOOL CDataItemClutterManager::IsPossible2DrawObject(const CDRect rt)
{
	//CDRect region(rt);
	//region.DeflateRect(region.Width()*m_dVisualComplexity, region.Height()*m_dVisualComplexity);

	//int pixels = (int)region.Width() * (int)region.Height();
	//GLfloat *data = new GLfloat[3 * pixels];
	//glReadPixels((int)region.left, (int)region.bottom, (int)region.Width(), (int)region.Height(),
	//	GL_RGB, GL_FLOAT, data);

	//for (int i = 0; i < pixels; i += 3)
	//{ // background color - 1.0f, 1.0f, 0.9f
	//	if (close_enough(1.0f, data[i], 0.1)
	//		&& close_enough(1.0f, data[i + 1], 0.1)
	//		&& close_enough(0.9f, data[i + 2], 0.1) )
	//	{
	//		continue;
	//	}
	//	return (FALSE);
	//}
	//delete[] data;

	return TRUE;
}

void CDataItemClutterManager::AddNode_noOverlapCheck(size_t rkey, size_t cidx, unsigned int callist,
	const CDRect rt, double zpos, CGLColor color, BOOL bhilight)
{
	CDataItemInfo Info;
	Info.m_callist = callist;
	Info.SetRect(rt);
	Info.m_color = color;
	Info.m_zpos = zpos;
	Info.m_rkey = rkey;		// item number used for GL selection
	Info.m_cindex = cidx;
	Info.m_highlight = bhilight;

	m_vecManagedSpace.push_back(Info);
}

void CDataItemClutterManager::AddNode_EvaluateOverlap(size_t rkey, size_t cidx, 
	unsigned int callist, CDRect rt, double zpos, CGLColor color, BOOL bhilight)
{
	CDataItemInfo Info;
	Info.m_callist = callist;
	Info.SetRect(rt);
	Info.m_color = color;
	Info.m_zpos = zpos;
	Info.m_rkey = rkey;		// item number used for GL selection
	Info.m_cindex = cidx;	
	Info.m_highlight = bhilight;

	if (bhilight)
	{// if highlighted or selected item
		vector<CDataItemInfo>::iterator it = m_vecManagedSpace.begin();
		m_vecManagedSpace.insert(it, Info); // add to the front
	}
	else
	{
		// using buffer to check overlapped region
		if (IsPossible2AddObject(rt))
		{
			m_vecManagedSpace.push_back(Info);
		}

		// Old method to add nodes into the display list.
		// For comparing all items, it takes O(n^3). 
		//for (size_t i=0; i<m_vecManagedSpace.size(); i++)
		//{
		//	CDRect tmp1 = m_vecManagedSpace[i];
		//	CDRect tmp2 = rt;
		//	// adjust the size of the object based on the defined visual complexity
		//	tmp1.DeflateRect(tmp1.Width()*m_dVisualComplexity, 
		//		tmp1.Height()*m_dVisualComplexity);
		//	tmp2.DeflateRect(tmp2.Width()*m_dVisualComplexity, 
		//		tmp2.Height()*m_dVisualComplexity);

		//	if (tmp1.IsRectsOverlap(tmp2)==true)
		//		return; // not adding to clutter manager to draw			
		//}
		//m_vecManagedSpace.push_back(Info);
	}
}

int CDataItemClutterManager::FindObject(double mx, double my)
{
	if (m_vecManagedSpace.size() == 0) return -1;

	for (int i=0; i<m_vecManagedSpace.size(); i++)
	{
		CDataItemInfo *pinfo = &m_vecManagedSpace[i];
		if (pinfo->IsInside(mx, my))
			return pinfo->m_rkey;
	}
	return -1;
}

void CDataItemClutterManager::Draw(GLenum mode)
{
	if (m_vecManagedSpace.size()==0) return;

	size_t i=m_vecManagedSpace.size()-1;
	while (m_vecManagedSpace.size()>0)
	{
		CDataItemInfo *pinfo = &m_vecManagedSpace[i];

		if (mode==GL_SELECT) glLoadName(pinfo->m_rkey);
		glColor4f(pinfo->m_color.R, pinfo->m_color.G, pinfo->m_color.B, pinfo->m_color.A);
		DrawObject(pinfo->m_callist, *pinfo, pinfo->m_zpos);

		if (pinfo->m_highlight)
		{
			glPushMatrix();
			glTranslatef(0.0f, 0.0f, 0.5f);
			glLineWidth(1.5f);
			if (pinfo->m_color.R==0.0 && pinfo->m_color.G == 0.0 && pinfo->m_color.B == 0.0)
				glColor4f(1.0f, 0.0f, 0.0f, 1.0); // red outline for black colored instances
			else
				glColor4f(0.0f, 0.0f, 0.0f, 1.0); // black outline for other colored instances
			DrawObject(*pinfo, GL_LINE_STRIP);
			glLineWidth(1.0f);
			glPopMatrix();
		}

		// Draw cluster information
/*/
		if (mode==GL_RENDER && pinfo->m_cindex!=-1)
		{
			float L = (float)0.2126 * pow((float) pinfo->m_color.R, (float)gamma )
				+ (float)0.7152 * pow((float) pinfo->m_color.G, (float)gamma )
				+ (float)0.0722 * pow((float) pinfo->m_color.B, (float)gamma );
			boolean use_black = ( L > pow((float) 0.5, gamma ) );

			if (use_black) glColor3f(0.0, 0.0, 0.0);
			else glColor3f(1.0, 1.0, 1.0);

			char tmp[2];
			tmp[0] = (pinfo->m_cindex) + 49;
			tmp[1] = '\0';
			CDPoint ptSize = m_pFont->ftglFontGetCharSize(tmp);
			double xpos= (pinfo->Width() - ptSize.x) / 2.;
			double ypos = (pinfo->Height() - ptSize.y) / 2.;
			if (xpos < 0) xpos = pinfo->left;
			else xpos += pinfo->left;
			if (ypos < 0) ypos = pinfo->bottom;
			else ypos += pinfo->bottom;

			m_pFont->FontDrawText(tmp, xpos, ypos, 1.0);
		}
//*/

		if (i==0) break;
		i--;
	}
}

void CDataItemClutterManager::DrawObject(CDRect rt, GLenum mode)
{
	glPushMatrix();
	glBegin(mode);
	glVertex2d(rt.left, rt.top);
	glVertex2d(rt.left, rt.bottom);
	glVertex2d(rt.right, rt.bottom);
	glVertex2d(rt.right, rt.top);
	if (mode == GL_LINE_STRIP)
		glVertex2d(rt.left, rt.top);
	glEnd();
	glPopMatrix();
}

void CDataItemClutterManager::DrawObject(unsigned int gllist, CDRect rt, double zpos)
{
	if (gllist==-1)
	{
		glPushMatrix();
		glBegin(GL_POLYGON);
		glVertex3d(rt.left, rt.top, zpos);
		glVertex3d(rt.left, rt.bottom, zpos);
		glVertex3d(rt.right, rt.bottom, zpos);
		glVertex3d(rt.right, rt.top, zpos);
		glEnd();
		glPopMatrix();
	}
	else
	{// if gl Call List is defined
		glPushMatrix();
		glTranslated(rt.GetHorizontalCenter(), rt.GetVerticalCenter(), zpos);
		glScaled(rt.Width(), rt.Height(), 0.0);
		glCallList(gllist);
		glPopMatrix();
	}
}

//////////////////////////////////////////////////////////////////////
// CTextClutterManager

void CTextClutterManager::addNode(double x, double y, double z, 
	CGLColor color, CString str, CftGLFontWrapper *font)
{
	double w=0, h=0;

	CTextInfo Info;
	Info.m_szInfo = str;
	Info.m_pFont = font;
	Info.m_xpos = x;
	Info.m_ypos = y;
	Info.m_zpos = z;
	Info.m_color = color;

	CDPoint ptSize = font->ftglFontGetCharSize(str);
	w = ptSize.x;
	h = ptSize.y;

	Info.left = x - 0.002/*marginal space*/;
	Info.right = x + w + 0.001/*marginal space*/;
	Info.top = y + h*2/3.;
	Info.bottom = y - h/3.;

	for (size_t i=0; i<m_vecManagedSpace.size(); i++)
	{
		CTextInfo *pinfo1 = &m_vecManagedSpace[i];
		if (pinfo1->IsRectsOverlap(Info)==true)
			return; // not adding to text manager to draw			
	}
	m_vecManagedSpace.push_back(Info);
}

void CTextClutterManager::draw()
{
	for (size_t i = 0; i<m_vecManagedSpace.size(); i++)
	{
		CTextInfo *pinfo = &m_vecManagedSpace[i];

		glColor4f(pinfo->m_color.R, pinfo->m_color.G, pinfo->m_color.B, pinfo->m_color.A);
		if (g_bVectorDrawing2File == FALSE)
		{
			pinfo->m_pFont->FontDrawText(pinfo->m_szInfo, pinfo->m_xpos, pinfo->m_ypos, pinfo->m_zpos);
		}
		else
		{
			CStringWrapper sText(pinfo->m_szInfo);
			const char *fonts[] =
			{ "Times-New-Roman", "Times-Bold", "Times-Italic", "Times-BoldItalic",
				"Helvetica", "Helvetica-Bold", "Helvetica-Oblique", "Helvetica-BoldOblique",
				"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique",
				"Symbol", "ZapfDingbats" };

			/* call gl2psText before the glut function since glutBitmapCharacter
			changes the raster position... */
			char *string = sText.getchardata();
			glRasterPos3d(pinfo->m_xpos, pinfo->m_ypos, pinfo->m_zpos);
			gl2psTextOpt(string, fonts[0], 9, GL2PS_TEXT_BL, 0.0);

			/*for (i = 0; i < pinfo->m_szInfo.GetLength(); i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);*/
		}

	}
}