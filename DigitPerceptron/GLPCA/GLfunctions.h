#pragma once

#include "DRect.h"
#include "freeglut.h"

void drawObject(CDRect rt, GLenum mode);
void drawLine(CDPoint pt1, CDPoint pt2);
void drawObject(vector<CDPoint> &pts, GLenum mode);
void drawCircle(CDPoint pt, double radius);
void drawCircleOutline(CDPoint pt, double radius);
CDPoint DetermineTextPosition(CDRect rtObj, CDRect rtText, CDRect rtRenderingRegion);
