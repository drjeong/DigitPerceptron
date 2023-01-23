// GLDraw.cpp : implementation file
//

#include "pch.h"
#include "GLDraw.h"
#include <PCA\mathfunctions.h>

// CGLDraw

CGLDraw::CGLDraw()
{

}

CGLDraw::~CGLDraw()
{
}

// CGLDraw message handlers

CGLColor CGLDraw::getHilightColor(CGLColor &color)
{
	CGLColor highlited(0.0f, 0.0f, 0.0f, 1.0f); // black outline (default)
	
	// if instance color is black, red outline color is used.
	if (color.R == 0.0f && color.G == 0.0f && color.B == 0.0f)
		highlited = CGLColor(1.0f, 0.0f, 0.0f, 1.0f); // red outline for black colored instances

	return highlited;
}

void CGLDraw::drawObject(GLfloat zpos, CDRect rt)
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

void CGLDraw::drawObject(CDRect rt)
{
	glPushMatrix();
	glBegin(GL_POLYGON);
	glVertex2d(rt.left, rt.top);
	glVertex2d(rt.left, rt.bottom);
	glVertex2d(rt.right, rt.bottom);
	glVertex2d(rt.right, rt.top);
	glEnd();
	glPopMatrix();
}

void CGLDraw::drawTriangleObject(vector<CDPoint> &pts)
{
	if (pts.size() == 0) return;

	glPushMatrix();
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2d(pts[1].x, pts[1].y);
	glVertex2d(pts[0].x, pts[0].y);
	glVertex2d(pts[3].x, pts[3].y);
	glVertex2d(pts[2].x, pts[2].y);
	glEnd();
	glPopMatrix();
}

void CGLDraw::drawObject(list<CDPoint> &pts, GLenum mode)
{
	if (pts.size() == 0) return;

	glPushMatrix();
	glBegin(mode);
	for (auto it = pts.begin(); it != pts.end(); ++it)
	{
		CDPoint pt = (*it);
		glVertex2d(pt.x, pt.y);
	}
	glEnd();
	glPopMatrix();
}

void CGLDraw::drawObject(vector<CDPoint> &pts, GLenum mode)
{
	if (pts.size() == 0) return;

	glPushMatrix();
	glBegin(mode);
	for (int i=0; i<pts.size(); i++)
		glVertex2d(pts[i].x, pts[i].y);
	if (mode == GL_LINE_STRIP)
		glVertex2d(pts[0].x, pts[0].y);
	glEnd();
	glPopMatrix();
}

void CGLDraw::drawObject(CDRect rt, GLenum mode)
{
	glPushMatrix();
	glBegin(mode);
	glVertex2d(rt.left, rt.top);
	glVertex2d(rt.left, rt.bottom);
	glVertex2d(rt.right, rt.bottom);
	glVertex2d(rt.right, rt.top);
	if (mode==GL_LINE_STRIP)
		glVertex2d(rt.left, rt.top);
	glEnd();
	glPopMatrix();
}

void CGLDraw::drawObject(CDPoint pt, double width, double height)
{
	glPushMatrix();
	glBegin(GL_POLYGON);
	glVertex2d(pt.x-width/2., pt.y+height/2.);
	glVertex2d(pt.x-width/2., pt.y-height/2.);
	glVertex2d(pt.x+width/2., pt.y-height/2.);
	glVertex2d(pt.x+width/2., pt.y+height/2.);
	glEnd();
	glPopMatrix();	
}

void CGLDraw::drawObject(CDPoint pt, double width, double height, GLenum mode)
{
	glPushMatrix();
	glBegin(mode);
	glVertex2d(pt.x-width/2., pt.y+height/2.);
	glVertex2d(pt.x+width/2., pt.y+height/2.);
	glVertex2d(pt.x+width/2., pt.y-height/2.);
	glVertex2d(pt.x-width/2., pt.y-height/2.);
	glEnd();
	glPopMatrix();
}

void CGLDraw::drawObject(CDPoint lt, CDPoint lb, CDPoint rt, CDPoint rb, GLenum mode)
{
	glPushMatrix();
	glBegin(mode);
	glVertex2d(lt.x, lt.y);
	glVertex2d(lb.x, lb.y);
	glVertex2d(rb.x, rb.y);
	glVertex2d(rt.x, rt.y);
	glEnd();
	glPopMatrix();
}

void CGLDraw::drawCircle(CDPoint pt, double radius)
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

void CGLDraw::drawCircleOutline(CDPoint pt, double radius)
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

void CGLDraw::drawLine(CDPoint pt1, CDPoint pt2)
{
	glPushMatrix();
	glBegin(GL_LINES);
	glVertex2d(pt1.x, pt1.y);
	glVertex2d(pt2.x, pt2.y);
	glEnd();
	glPopMatrix();
}

void CGLDraw::drawObject(unsigned int gllist, CDRect rt, float zpos)
{
	glPushMatrix();
	glTranslated(rt.GetHorizontalCenter(), rt.GetVerticalCenter(), zpos);
	glScaled(rt.Width(), rt.Height(), 0.0);
	glCallList(gllist);
	glPopMatrix();
}


double CGLDraw::GetRelativeTargetValue(double tmax, double tmin, double smax, double smin, double val)
{
	double res = 0;
	// min-max normalization Artificial Neural Networks By Kevin L. Priddy, Paul E. Keller
	res = (tmax - tmin)*((val - smin) / (smax - smin)) + tmin;
	return res;
}

//void CGLDraw::drawHorizontalBoxplot(CDRect region, vector<double>data, CGLColor color, double tbmargin)
//{
//	// determine quartiles	
//	C2DMinMax minmax;
//	auto max = std::max_element(data.begin(), data.end());
//	auto min = std::min_element(data.begin(), data.end());
//	minmax.SetMax(*max);
//	minmax.SetMin(*min);
//	vector<double> quartiles = Quantile<double>(data, { 0.25, 0.5, 0.75 });
//
//	// drawing
//	glColor4d(color.R, color.G, color.B, 0.5f);
//	glPushMatrix();
//	{
//		//CGLScissorTest glScissorTest(region);
//		region.top -= tbmargin;
//		region.bottom += tbmargin;
//
//		//-----------------------------------------------------------------------
//		// Draw interquartile range (IQR): 25th to the 75th percentile
//		double dQ1 = quartiles[0];
//		double dQ2 = quartiles[1];
//		double dQ3 = quartiles[2];
//		double dIQR = dQ3 - dQ1;
//		double dnQ1 = GetRelativeTargetValue(region.right, region.left, 1.0, -1.0, dQ1);
//		double dnQ2 = GetRelativeTargetValue(region.right, region.left, 1.0, -1.0, dQ2);
//		double dnQ3 = GetRelativeTargetValue(region.right, region.left, 1.0, -1.0, dQ3);
//		CDRect rtQuartile(dnQ1, region.bottom, dnQ3, region.top);
//		drawObject(rtQuartile, GL_LINE_STRIP);
//		glLineWidth(1.5f);
//		drawLine(CDPoint(dnQ2, rtQuartile.top), CDPoint(dnQ2, rtQuartile.bottom));
//		glLineWidth(1.0);
//
//		//-----------------------------------------------------------------------
//		// Draw semi interquartile range (IQR)
//		double dSemiIQR = dIQR / 2;
//		double dQ1_SemiIQR = dQ1 - dSemiIQR;
//		double dQ3_SemiIQR = dQ3 + dSemiIQR;
//		double dnQ1_SemiIQR = GetRelativeTargetValue(region.right, region.left, 1.0, -1.0, dQ1_SemiIQR);
//		double dnQ3_SemiIQR = GetRelativeTargetValue(region.right, region.left, 1.0, -1.0, dQ3_SemiIQR);
//		drawLine(CDPoint(dnQ1_SemiIQR, rtQuartile.top), CDPoint(dnQ1_SemiIQR, rtQuartile.bottom));
//		drawLine(CDPoint(dnQ3_SemiIQR, rtQuartile.top), CDPoint(dnQ3_SemiIQR, rtQuartile.bottom));
//
//		glEnable(GL_LINE_STIPPLE);
//		glLineStipple(1, 0x0F0F);  /*  dotted  */
//		drawLine(CDPoint(dnQ1_SemiIQR, rtQuartile.GetVerticalCenter()),
//			CDPoint(rtQuartile.left, rtQuartile.GetVerticalCenter()));
//		drawLine(CDPoint(dnQ3_SemiIQR, rtQuartile.GetVerticalCenter()),
//			CDPoint(rtQuartile.right, rtQuartile.GetVerticalCenter()));
//		glDisable(GL_LINE_STIPPLE);
//
//		// determine possible outliers
//		for (int i = 0; i<data.size(); i++)
//		{
//			if (data[i] < dQ1_SemiIQR || data[i] > dQ3_SemiIQR)
//			{
//				CDPoint pt;
//				pt.x = GetRelativeTargetValue(region.right, region.left, 1.0, -1.0, data[i]);
//				pt.y = rtQuartile.GetVerticalCenter();
//				drawCircle(pt, rtQuartile.Height() * 0.4);
//			}
//		}
//	}
//	glPopMatrix();
//}


void CGLDraw::drawHorzStandardBoxPlot(CDRect region, double dnQ2, double dnQ1_SemiIQR, double dnQ3_SemiIQR)
{
	//       Q1   median  Q3
	//        +-----------+				
	// |------+     |	  +------ +		
	//        +-----------+			
	//      IQR (Interquartile)

	// Draw boxplot outline
	drawObject(region, GL_LINE_STRIP);

	// Draw median 
	glLineWidth(1.5f);
	drawLine(CDPoint(dnQ2, region.top), CDPoint(dnQ2, region.bottom));
	glLineWidth(1.0);

	// Draw semi interquartile range (IQR)
	drawLine(CDPoint(dnQ1_SemiIQR, region.top), CDPoint(dnQ1_SemiIQR, region.bottom));
	drawLine(CDPoint(dnQ3_SemiIQR, region.top), CDPoint(dnQ3_SemiIQR, region.bottom));

	// Draw minimum (Q1 - IQR) & maximum (Q3 + IQR)
	//glEnable(GL_LINE_STIPPLE);
	//glLineStipple(1, 0x0F0F);  /*  dotted  */
	drawLine(CDPoint(dnQ1_SemiIQR, region.GetVerticalCenter()), CDPoint(region.left, region.GetVerticalCenter()));
	drawLine(CDPoint(dnQ3_SemiIQR, region.GetVerticalCenter()), CDPoint(region.right, region.GetVerticalCenter()));
	//glDisable(GL_LINE_STIPPLE);
}

void CGLDraw::drawHorzNotchBoxPlot(CDRect region, double dnQ2, 
	double dnQ1_SemiIQR, double dnQ3_SemiIQR, double dnLowerLimit, double dnUpperLimit)
{
	//             Q1   median  Q3
	//              +--\     /----+				
    //     whiskers |   \   /	  | whiskers
	// |------------+     |	    +------------+		
	//              |   /   \     |
 	//              +--/     \----+			
	//             IQR (Interquartile)

	
	double diff = region.Height() / 5;
	double mtop = region.top - diff;
	double mbottom = region.bottom + diff;

	// Draw median 
	glLineWidth(1.5f);
	drawLine(CDPoint(dnQ2, mtop), CDPoint(dnQ2, mbottom));
	glLineWidth(1.0);

	// Draw boxplot outline 
	vector<CDPoint> pts;
	pts.push_back(CDPoint(region.left, region.top));
	pts.push_back(CDPoint(dnLowerLimit, region.top));
	pts.push_back(CDPoint(dnQ2, mtop));
	pts.push_back(CDPoint(dnUpperLimit, region.top));
	pts.push_back(CDPoint(region.right, region.top));
	pts.push_back(CDPoint(region.right, region.bottom));
	pts.push_back(CDPoint(dnUpperLimit, region.bottom));
	pts.push_back(CDPoint(dnQ2, mbottom));
	pts.push_back(CDPoint(dnLowerLimit, region.bottom));
	pts.push_back(CDPoint(region.left, region.bottom));
	pts.push_back(CDPoint(region.left, region.top));
	drawObject(pts, GL_LINE_STRIP);

	// Draw semi interquartile range (IQR)
	drawLine(CDPoint(dnQ1_SemiIQR, region.top), CDPoint(dnQ1_SemiIQR, region.bottom));
	drawLine(CDPoint(dnQ3_SemiIQR, region.top), CDPoint(dnQ3_SemiIQR, region.bottom));

	// Draw whiskers - minimum (Q1 - IQR) & maximum (Q3 + IQR)
	//glEnable(GL_LINE_STIPPLE);
	//glLineStipple(1, 0x0F0F);  /*  dotted  */
	drawLine(CDPoint(dnQ1_SemiIQR, region.GetVerticalCenter()), CDPoint(region.left, region.GetVerticalCenter()));
	drawLine(CDPoint(dnQ3_SemiIQR, region.GetVerticalCenter()), CDPoint(region.right, region.GetVerticalCenter()));
	//glDisable(GL_LINE_STIPPLE);
}

void CGLDraw::drawVertNotchBoxPlot(CDRect region, double dnQ2,
	double dnQ1_SemiIQR, double dnQ3_SemiIQR, double dnLowerLimit, double dnUpperLimit)
{
	//        ----- 
	//          |   whiskers
	//       +-----+   Q3
	//       |     | 
	//        \   /  dnUpperLimit
	//         ---   median
	//        /   \  dnLowerLimit
	//       |     | 
	//       +-----+   Q1
	//          |    whiskers
	//        ----- 
	// 
	double diff = region.Width() / 5;
	double mleft = region.left + diff; 
	double mright = region.right - diff;

	// Draw median 
	glLineWidth(1.5f);
	drawLine(CDPoint(mleft, dnQ2), CDPoint(mright, dnQ2));
	glLineWidth(1.0);

	// Draw boxplot outline 
	vector<CDPoint> pts;
	pts.push_back(CDPoint(region.left, region.top));
	pts.push_back(CDPoint(region.right, region.top));
	pts.push_back(CDPoint(region.right, dnUpperLimit));
	pts.push_back(CDPoint(mright, dnQ2));
	pts.push_back(CDPoint(region.right, dnLowerLimit));
	pts.push_back(CDPoint(region.right, region.bottom));
	pts.push_back(CDPoint(region.left, region.bottom));
	pts.push_back(CDPoint(region.left, dnLowerLimit));
	pts.push_back(CDPoint(mleft, dnQ2));
	pts.push_back(CDPoint(region.left, dnUpperLimit));
	pts.push_back(CDPoint(region.left, region.top));
	drawObject(pts, GL_LINE_STRIP);


	// Draw semi interquartile range (IQR)
	drawLine(CDPoint(region.left, dnQ1_SemiIQR), CDPoint(region.right, dnQ1_SemiIQR));
	drawLine(CDPoint(region.left, dnQ3_SemiIQR), CDPoint(region.right, dnQ3_SemiIQR));

	// Draw whiskers - minimum (Q1 - IQR) & maximum (Q3 + IQR)
	//glEnable(GL_LINE_STIPPLE);
	//glLineStipple(1, 0x0F0F);  /*  dotted  */
	drawLine(CDPoint(region.GetHorizontalCenter(), dnQ1_SemiIQR),
		CDPoint(region.GetHorizontalCenter(), region.bottom));
	drawLine(CDPoint(region.GetHorizontalCenter(), dnQ3_SemiIQR),
		CDPoint(region.GetHorizontalCenter(), region.top));
	//glDisable(GL_LINE_STIPPLE);
}


void CGLDraw::drawVertStandardBoxPlot(CDRect region, double dnQ2, double dnQ1_SemiIQR, double dnQ3_SemiIQR)
{
	//        ----- 
	//          |     whiskers
	//       +-----+   Q3
	//       |     |
	//       |-----|   median
	//       |     |
	//       +-----+   Q1
	//          |     whiskers
	//        ----- 
	// 

	// Draw boxplot outline
	drawObject(region, GL_LINE_STRIP);

	// Draw median 
	glLineWidth(1.5f);
	drawLine(CDPoint(region.left, dnQ2), CDPoint(region.right, dnQ2));
	glLineWidth(1.0);

	// Draw semi interquartile range (IQR)
	drawLine(CDPoint(region.left, dnQ1_SemiIQR), CDPoint(region.right, dnQ1_SemiIQR));
	drawLine(CDPoint(region.left, dnQ3_SemiIQR), CDPoint(region.right, dnQ3_SemiIQR));

	// Draw whiskers - minimum (Q1 - IQR) & maximum (Q3 + IQR)
	//glEnable(GL_LINE_STIPPLE);
	//glLineStipple(1, 0x0F0F);  /*  dotted  */
	drawLine(CDPoint(region.GetHorizontalCenter(), dnQ1_SemiIQR),
		CDPoint(region.GetHorizontalCenter(), region.bottom));
	drawLine(CDPoint(region.GetHorizontalCenter(), dnQ3_SemiIQR),
		CDPoint(region.GetHorizontalCenter(), region.top));
	//glDisable(GL_LINE_STIPPLE);
}