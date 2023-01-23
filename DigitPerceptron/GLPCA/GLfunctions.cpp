#include "pch.h"
#include "GLfunctions.h"
#include <numeric> // accumulate

void drawObject(CDRect rtObj, GLenum mode)
{
	glPushMatrix();
	glBegin(mode);
	glVertex2d(rtObj.left, rtObj.top);
	glVertex2d(rtObj.left, rtObj.bottom);
	glVertex2d(rtObj.right, rtObj.bottom);
	glVertex2d(rtObj.right, rtObj.top);
	if (mode == GL_LINE_STRIP)
		glVertex2d(rtObj.left, rtObj.top);
	glEnd();
	glPopMatrix();
}

void drawLine(CDPoint pt1, CDPoint pt2)
{
	glPushMatrix();
	glBegin(GL_LINES);
	glVertex2d(pt1.x, pt1.y);
	glVertex2d(pt2.x, pt2.y);
	glEnd();
	glPopMatrix();
}

void drawObject(vector<CDPoint> &pts, GLenum mode)
{
	if (pts.size() == 0) return;

	glPushMatrix();
	glBegin(mode);
	for (int i = 0; i<pts.size(); i++)
		glVertex2d(pts[i].x, pts[i].y);
	glEnd();
	glPopMatrix();
}

void drawCircle(CDPoint pt, double radius)
{
	// circle shape of each item
	const int SEGMENTS = 12;
	const double DEG2RAD = 2 * 3.14159 / SEGMENTS;
	glPushMatrix();
	glBegin(GL_TRIANGLE_FAN);
	glVertex2d(pt.x, pt.y);
	for (int i = 0; i <= SEGMENTS; i++)
	{
		double degInRad = i*DEG2RAD;
		double x = pt.x + cos(degInRad)*radius;
		double y = pt.y + sin(degInRad)*radius;
		glVertex2d(x, y);
	}
	glEnd();
	glPopMatrix();
}

void drawCircleOutline(CDPoint pt, double radius)
{
	// circle shape of each item
	const int SEGMENTS = 12;
	const double DEG2RAD = 2 * 3.14159 / SEGMENTS;
	glPushMatrix();
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= SEGMENTS; i++)
	{
		double degInRad = i*DEG2RAD;
		double x = pt.x + cos(degInRad)*radius;
		double y = pt.y + sin(degInRad)*radius;
		glVertex2d(x, y);
	}
	glEnd();
	glPopMatrix();
}


CDPoint DetermineTextPosition(CDRect rtObj, CDRect rtText, CDRect rtRenderingRegion)
{
	CDRect rtRegion(CDPoint(rtObj.right, rtObj.top), rtText.Height()/2, rtText.Width());
	double boundaryMargin = rtRegion.Height() / 2;
	rtRegion.InflateRect(boundaryMargin, boundaryMargin);

	if (rtRegion.left > rtRenderingRegion.right || rtRegion.right > rtRenderingRegion.right)
	{
		rtRegion.MoveLeft(abs(rtRegion.left - rtRenderingRegion.right)); // text is hidden by object
		rtRegion.MoveDown(rtObj.Height()); // text is outside of the boundary
	}
	if (rtRegion.top > rtRenderingRegion.top || rtRegion.bottom > rtRenderingRegion.top)
	{
		rtRegion.MoveLeft(abs(rtRegion.right - rtObj.left)); // text is hidden by cursor
		rtRegion.MoveDown(abs(rtRegion.top - rtRenderingRegion.top) + rtObj.Height()); // text is outside of the boundary
	}
	if (rtRegion.right < rtRenderingRegion.left || rtRegion.left < rtRenderingRegion.left)
	{
		rtRegion.MoveRight(abs(rtRenderingRegion.left - rtRegion.left)); // text is outside of the boundary
	}
	
	return CDPoint(rtRegion.left, rtRegion.top);
}
