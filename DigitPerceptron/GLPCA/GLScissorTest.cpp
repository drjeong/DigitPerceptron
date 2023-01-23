#include "pch.h"
#include "GLScissorTest.h"
#include "freeglut.h"

CGLScissorTest::CGLScissorTest(CDRect rect)
{
	glEnable(GL_SCISSOR_TEST);
	glGetDoublev(GL_MODELVIEW_MATRIX, M);
	glGetDoublev(GL_PROJECTION_MATRIX, P);
	glGetIntegerv(GL_VIEWPORT, V);
	gluProject(rect.left, rect.bottom, 0, M, P, V, &px1, &py1, &pz);
	gluProject(rect.right, rect.top, 0, M, P, V, &px2, &py2, &pz);
	glScissor((int)px1, (int)py1, (int)(px2 - px1), (int)(py2 - py1));
}


CGLScissorTest::~CGLScissorTest()
{
	glDisable(GL_SCISSOR_TEST);
}
