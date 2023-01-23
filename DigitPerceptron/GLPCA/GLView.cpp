// GLView.cpp : implementation file
//

#include "pch.h"
#include "GLView.h"


// CGLView

CGLView::CGLView()
{
	m_nWindowType = -1;
	__glModifierMask = (unsigned int) ~0;
	//m_nCmdShow = SW_SHOW;
	m_selectBuffer = NULL;
	//this->SetRectEmpty();
	//m_pParent = NULL;
	m_ftglFontText.Initialize(9);

	if (m_selectBuffer==NULL) m_selectBuffer = new GLuint [GL_SELECT_BUFFER_SIZE];
}

CGLView::~CGLView()
{
	if (m_selectBuffer) delete [] m_selectBuffer;
}


// CGLView message handlers

unsigned int CGLView::glGetModifiers()
{
	__glModifierMask = 0;

	if (GetKeyState(VK_SHIFT) < 0)  /* < 0 = high order bit is on */
		__glModifierMask |= GL_ACTIVE_SHIFT;
	if (GetKeyState(VK_CONTROL) < 0)  /* < 0 = high order bit is on */
		__glModifierMask |= GL_ACTIVE_CTRL;
	if (GetKeyState(VK_MENU) < 0)  /* < 0 = high order bit is on */
		__glModifierMask |= GL_ACTIVE_ALT;

	return __glModifierMask;
}

