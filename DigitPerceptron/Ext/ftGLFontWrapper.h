#ifndef FTGLFUNC_DEFINE_H
#define FTGLFUNC_DEFINE_H

#pragma once

#include <FTGL/ftgl.h>
#include "DPoint.h"
#include "DRect.h"

class CftGLFontWrapper
{
public:
	CftGLFontWrapper(void);
	virtual ~CftGLFontWrapper(void);

	void Initialize(int facesize=20);
	void FontDrawText(CString &szText, double xpos, double ypos, double zpos=0.0);
	void FontDrawVerticalText(CString &szText, double xpos, double ypos, double zpos=0.0);
	void FontDrawAngledText(CString &szText, double angle, double xpos, double ypos, double zpos=0.0);
	void FontDrawText(const char *szText, double xpos, double ypos, double zpos=0.0);
	void FontDrawVerticalText(const char *szText, double xpos, double ypos, double zpos=0.0);
	void FontDrawAngledText(const char *szText, double angle, double xpos, double ypos, double zpos=0.0);
	CDPoint ftglFontGetCharSize(const char *text);
	CDPoint ftglFontGetCharSize(CString &text);
	CDRect ftglFontGetCharRect(CString &text);

private:
	FTFont *m_pFTFont;
};

#endif