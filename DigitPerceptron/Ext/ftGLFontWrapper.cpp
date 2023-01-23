#include "pch.h"
#include "ftGLFontWrapper.h"
#include "StringWrapper.h"
#include <gl\GL.h>
#include "gl2ps.h"

#define FONT_FILE "arial.ttf"

extern BOOL g_bVectorDrawing2File; // defined in GLPCA.CPP used for generating SVG or PDF capture file

CftGLFontWrapper::CftGLFontWrapper(void)
{
	m_pFTFont=NULL;
}

CftGLFontWrapper::~CftGLFontWrapper(void)
{
	if (m_pFTFont) delete m_pFTFont;
}

void CftGLFontWrapper::Initialize(int facesize)
{
	// remove if exists
	if (m_pFTFont) delete m_pFTFont;

	// create a font
	m_pFTFont = new FTTextureFont(FONT_FILE);
	if (m_pFTFont->Error())
	{
		int msgboxID = MessageBox(
			NULL,
			(LPCWSTR)L"Could not create glfont face!!",
			(LPCWSTR)L"Error Details",
			MB_OK
		);
		//AfxMessageBox(NULL, "Could create font face!!", "Error!!");
		return;
	}
	m_pFTFont->FaceSize(facesize);
	m_pFTFont->Depth(10);
	m_pFTFont->Outset(0, 3);
	m_pFTFont->CharMap(ft_encoding_unicode);
}

void CftGLFontWrapper::FontDrawText(CString &szText, double xpos, double ypos, double zpos)
{
	if (m_pFTFont==NULL) return;

	if (g_bVectorDrawing2File == FALSE)
	{
		glDisable(GL_DEPTH_TEST);
		glPushMatrix();
		//glTranslated(0.0, 0.0, zpos);
		CStringWrapper sText(szText);
		m_pFTFont->Render(sText.getchardata(), -1, FTPoint(xpos, ypos, zpos));
		glPopMatrix();
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		CStringWrapper sText(szText);
		const char *fonts[] =
		{ "Times-New-Roman", "Times-Bold", "Times-Italic", "Times-BoldItalic",
			"Helvetica", "Helvetica-Bold", "Helvetica-Oblique", "Helvetica-BoldOblique",
			"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique",
			"Symbol", "ZapfDingbats" };

		/* call gl2psText before the glut function since glutBitmapCharacter
		changes the raster position... */
		char *string = sText.getchardata();
		glRasterPos3d(xpos, ypos, zpos);
		gl2psTextOpt(string, fonts[0], 9, GL2PS_TEXT_BL, 0.0);
	}
}

void CftGLFontWrapper::FontDrawText(const char *szText, double xpos, double ypos, double zpos)
{
	if (m_pFTFont==NULL) return;

	if (g_bVectorDrawing2File == FALSE)
	{
		// 	glDisable(GL_DEPTH_TEST);
		glPushMatrix();
		glTranslated(0.0, 0.0, zpos);
		m_pFTFont->Render(szText, -1, FTPoint(xpos, ypos, zpos));
		glPopMatrix();
		// 	glEnable(GL_DEPTH_TEST);
	}
	else
	{
		const char *fonts[] =
		{ "Times-New-Roman", "Times-Bold", "Times-Italic", "Times-BoldItalic",
			"Helvetica", "Helvetica-Bold", "Helvetica-Oblique", "Helvetica-BoldOblique",
			"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique",
			"Symbol", "ZapfDingbats" };

		/* call gl2psText before the glut function since glutBitmapCharacter
		changes the raster position... */
		glRasterPos3d(xpos, ypos, zpos);
		gl2psTextOpt(szText, fonts[0], 9, GL2PS_TEXT_BL, 0.0);
	}
}

void CftGLFontWrapper::FontDrawVerticalText(const char *szText, double xpos, double ypos, double zpos)
{
	if (m_pFTFont==NULL) return;

	if (g_bVectorDrawing2File == FALSE)
	{
		glPushMatrix();
		glTranslated(xpos, ypos, zpos);
		glRotated(90, 0, 0, 1);
		m_pFTFont->Render(szText);
		glPopMatrix();
	}
	else
	{
		const char *fonts[] =
		{ "Times-New-Roman", "Times-Bold", "Times-Italic", "Times-BoldItalic",
			"Helvetica", "Helvetica-Bold", "Helvetica-Oblique", "Helvetica-BoldOblique",
			"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique",
			"Symbol", "ZapfDingbats" };

		/* call gl2psText before the glut function since glutBitmapCharacter
		changes the raster position... */
		glRasterPos3d(xpos, ypos, zpos);
		gl2psTextOpt(szText, fonts[0], 9, GL2PS_TEXT_BL, 90.0);
	}
}

void CftGLFontWrapper::FontDrawVerticalText(CString &szText, double xpos, double ypos, double zpos)
{
	if (m_pFTFont==NULL) return;

	if (g_bVectorDrawing2File == FALSE)
	{
		glPushMatrix();
		glTranslated(xpos, ypos, zpos);
		glRotated(90, 0, 0, 1);
		CStringWrapper sText(szText);
		m_pFTFont->Render(sText.getchardata());
		glPopMatrix();
	}
	else
	{
		CStringWrapper sText(szText);
		const char *fonts[] =
		{ "Times-New-Roman", "Times-Bold", "Times-Italic", "Times-BoldItalic",
			"Helvetica", "Helvetica-Bold", "Helvetica-Oblique", "Helvetica-BoldOblique",
			"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique",
			"Symbol", "ZapfDingbats" };
		glPushMatrix();
//		glTranslated(xpos, ypos, zpos);
//		glRotated(90, 0, 0, 1);
		/* call gl2psText before the glut function since glutBitmapCharacter
		changes the raster position... */
		glRasterPos3d(xpos, ypos, zpos);
		gl2psTextOpt(sText.getchardata(), fonts[0], 9, GL2PS_TEXT_BL, 90.0);
		glPopMatrix();
	}
}

void CftGLFontWrapper::FontDrawAngledText(CString &szText, double angle, 
	double xpos, double ypos, double zpos)
{
	if (m_pFTFont==NULL) return;

	if (g_bVectorDrawing2File == FALSE)
	{
		glPushMatrix();
		glTranslated(xpos, ypos, zpos);
		glRotated(angle, 0, 0, 1);
		CStringWrapper sText(szText);
		m_pFTFont->Render(sText.getchardata());
		glPopMatrix();
	}
	else
	{
		//CStringWrapper sText(szText);
		//const char *fonts[] =
		//{ "Times-New-Roman", "Times-Bold", "Times-Italic", "Times-BoldItalic",
		//	"Helvetica", "Helvetica-Bold", "Helvetica-Oblique", "Helvetica-BoldOblique",
		//	"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique",
		//	"Symbol", "ZapfDingbats" };

		///* call gl2psText before the glut function since glutBitmapCharacter
		//changes the raster position... */
		//glRasterPos3d(xpos, ypos, zpos);
		//gl2psTextOpt(sText.getchardata(), fonts[0], 9, GL2PS_TEXT_BL, angle);
	}


}

void CftGLFontWrapper::FontDrawAngledText(const char *szText, double angle, 
	double xpos, double ypos, double zpos)
{
	if (m_pFTFont==NULL) return;

	if (g_bVectorDrawing2File == FALSE)
	{
		glPushMatrix();
		glTranslated(xpos, ypos, zpos);
		glRotated(angle, 0, 0, 1);
		m_pFTFont->Render(szText);
		glPopMatrix();
	}
	else
	{
		//const char *fonts[] =
		//{ "Times-New-Roman", "Times-Bold", "Times-Italic", "Times-BoldItalic",
		//	"Helvetica", "Helvetica-Bold", "Helvetica-Oblique", "Helvetica-BoldOblique",
		//	"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique",
		//	"Symbol", "ZapfDingbats" };

		///* call gl2psText before the glut function since glutBitmapCharacter
		//changes the raster position... */
		//glRasterPos3d(xpos, ypos, zpos);
		//gl2psTextOpt(szText, fonts[0], 9, GL2PS_TEXT_BL, angle);
	}
}


CDPoint CftGLFontWrapper::ftglFontGetCharSize(const char *text)
{
	CDPoint charSize;
	FTBBox bbox;
	float x1, y1, z1, x2, y2, z2;

	bbox = m_pFTFont->BBox(text);

	x1 = bbox.Lower().Xf(); y1 = bbox.Lower().Yf(); z1 = bbox.Lower().Zf();
	x2 = bbox.Upper().Xf(); y2 = bbox.Upper().Yf(); z2 = bbox.Upper().Zf();

	charSize.x = x2-x1;
	charSize.y = y2-y1;

	return charSize;
}

CDPoint CftGLFontWrapper::ftglFontGetCharSize(CString &text)
{
	CDPoint charSize;
	FTBBox bbox;
	float x1, y1, z1, x2, y2, z2;

	CStringWrapper sText(text);
	bbox = m_pFTFont->BBox(sText.getchardata());

	x1 = bbox.Lower().Xf(); y1 = bbox.Lower().Yf(); z1 = bbox.Lower().Zf();
	x2 = bbox.Upper().Xf(); y2 = bbox.Upper().Yf(); z2 = bbox.Upper().Zf();

	charSize.x = x2-x1; // width
	charSize.y = y2-y1; // height

	return charSize;
}


CDRect CftGLFontWrapper::ftglFontGetCharRect(CString &text)
{
	FTBBox bbox;
	float x1, y1, z1, x2, y2, z2;

	CStringWrapper sText(text);
	bbox = m_pFTFont->BBox(sText.getchardata());

	x1 = bbox.Lower().Xf(); y1 = bbox.Lower().Yf(); z1 = bbox.Lower().Zf();
	x2 = bbox.Upper().Xf(); y2 = bbox.Upper().Yf(); z2 = bbox.Upper().Zf();

	return CDRect(x1, y1, x2, y2);
}