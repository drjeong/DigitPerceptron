#include "pch.h"
#include "GLWnd.h"
#include "glut.h"


CGLWnd::CGLWnd(void)
{
	m_pDC = NULL;
}


CGLWnd::~CGLWnd(void)
{
	//Delete the DC
	if (m_pDC) delete m_pDC;

	//Set it to NULL
	m_pDC = NULL;
}

BEGIN_MESSAGE_MAP(CGLWnd, CWnd)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


BOOL CGLWnd::MakeActive()
{
	if (m_hRC==0 || m_pDC==0) return FALSE;

	BOOL bRet = FALSE;
	int i = 0;

	// check if m_hRC is already current
	if (wglGetCurrentContext() == m_hRC)
		return TRUE;

	while ((bRet = wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC)) == FALSE)
	{
		if (++i > 100)										// Try it only 1000 times
		{
			TRACE("Abort \"MakeActive\" after %d successless trials\n", i);
			break;
		}
		if (GetLastError() == ERROR_INVALID_PIXEL_FORMAT)	// The pixel format is invalid.
		{
			InitializeOpenGL();
		}
	}
#ifdef _DEBUG
	if (i > 0 && bRet)
		TRACE("\"MakeActive\" was successfull after %d trials.\n", i);
#endif // _DEBUG

	return bRet;
}


BOOL CGLWnd::InitializeOpenGL()
{
	//Get a DC for the Client Area
	m_pDC = new CClientDC(this);

	//Failure to Get DC
	if(m_pDC == NULL)
	{
		MessageBox(_T("Error Obtaining DC"));
		return FALSE;
	}

	//Failure to set the pixel format
	if(!SetupPixelFormat())
	{
		return FALSE;
	}

	//Create Rendering Context
	m_hRC = ::wglCreateContext (m_pDC->GetSafeHdc ());

	//Failure to Create Rendering Context
	if(m_hRC == 0)
	{
		MessageBox(_T("Error Creating RC"));
		return FALSE;
	}

	//Make the RC Current
	if(::wglMakeCurrent (m_pDC->GetSafeHdc (), m_hRC)==FALSE)
	{
		MessageBox(_T("Error making RC Current"));
		return FALSE;
	}

	//Enable Depth Testing
	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
 	glDepthFunc(GL_LEQUAL);	
// 	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Supporting Anti-alising.
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glLineWidth(1.0);

	// Gouraud shading
	glShadeModel(GL_SMOOTH);

	//Specify light yellow as the clear color
	glClearColor(1.0f, 1.0f, 0.9f, 1.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black

	//Specify the back of the buffer as clear depth
 	glClearDepth(1.0f);

	//glPolygonMode(GL_FRONT, GL_FILL);
	//glPolygonMode(GL_BACK, GL_LINE);

	//glFrontFace(GL_CCW);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	return TRUE;
}

//Setup Pixel Format
/////////////////////////////////////////////////////////////////////////////
BOOL CGLWnd::SetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
			1,                              // version number
			PFD_DRAW_TO_WINDOW |            // support window
			PFD_SUPPORT_OPENGL |		    // support OpenGL
			PFD_DOUBLEBUFFER,				// double buffered
			PFD_TYPE_RGBA,                  // RGBA type
			24,                             // 24-bit color depth
			0, 0, 0, 0, 0, 0,               // color bits ignored
			0,                              // no alpha buffer
			0,                              // shift bit ignored
			0,                              // no accumulation buffer
			0, 0, 0, 0,                     // accum bits ignored
			16,                             // 16-bit z-buffer
			0,                              // no stencil buffer
			0,                              // no auxiliary buffer
			PFD_MAIN_PLANE,                 // main layer
			0,                              // reserved
			0, 0, 0                         // layer masks ignored
	};

	int m_nPixelFormat = ::ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);

	if ( m_nPixelFormat == 0 )
	{
		return FALSE;
	}

	if ( ::SetPixelFormat(m_pDC->GetSafeHdc(), m_nPixelFormat, &pfd) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

void CGLWnd::OnDestroy()
{
	__super::OnDestroy();
	
	if (wglGetCurrentContext() != NULL)
		wglMakeCurrent(NULL, NULL);

	if (m_hRC != NULL)
	{
		wglDeleteContext(m_hRC);
		m_hRC = NULL;
	}
}
