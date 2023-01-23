#include "pch.h"
#include "colorspace.h"
#include <math.h>

#define RADIUS		100
#define PI			3.14159265358

CColorSpace::CColorSpace(void)
{
}

CColorSpace::~CColorSpace(void)
{
}

void CColorSpace::SetRGB(int r, int g, int b)
{
	m_r = r; m_g = g; m_b = b;
	CaculateHSV();
}

void CColorSpace::SetHSV(int h, int s, int v)
{
	m_h = h; m_s = s; m_v = v;
	CaculateRGB();
}

void CColorSpace::CaculateRGB()
{
	if(!m_h  && !m_s)
	{
		m_r = m_g = m_b = m_v;
	}
	double min,max,delta,hue;

	max = m_v;
	delta = (max * m_s)/255.0;
	min = max - delta;

	hue = m_h;
	if(m_h > 300 || m_h <= 60)
	{
		m_r = (int)max;
		if(m_h > 300)
		{
			m_g = (int)min;
			hue = (hue - 360.0)/60.0;
			m_b = (int)((hue * delta - min) * -1);
		}
		else
		{
			m_b = (int)min;
			hue = hue / 60.0;
			m_g = (int)(hue * delta + min);
		}
	}
	else
		if(m_h > 60 && m_h < 180)
		{
			m_g = (int)max;
			if(m_h < 120)
			{
				m_b = (int)min;
				hue = (hue/60.0 - 2.0 ) * delta;
				m_r = (int)(min - hue);
			}
			else
			{
				m_r = (int)min;
				hue = (hue/60 - 2.0) * delta;
				m_b = (int)(min + hue);
			}
		}
		else
		{
			m_b = (int)max;
			if(m_h < 240)
			{
				m_r = (int)min;
				hue = (hue/60.0 - 4.0 ) * delta;
				m_g = (int)(min - hue);
			}
			else
			{
				m_g = (int)min;
				hue = (hue/60 - 4.0) * delta;
				m_r = (int)(min + hue);
			}
		}
}


void CColorSpace::CaculateHSV()
{
	double min,max,delta,temp;
	min = __min(m_r,__min(m_g,m_b));	
	max = __max(m_r,__max(m_g,m_b));
	delta = max - min;

	m_v = (int)max;
	if(!delta)
	{
		m_h = m_s = 0;
	}
	else
	{
		temp = delta/max;
		m_s = (int)(temp*255);

		if(m_r == (int)max)
		{
			temp = (double)(m_g-m_b)/delta;
		}
		else
			if(m_g == (int)max)
			{
				temp = 2.0 + ((double)(m_b-m_r)/delta);
			}
			else
			{
				temp = 4.0 + ((double)(m_r-m_g)/delta);
			}
			temp *= 60;
			if(temp < 0)
			{
				temp+=360;
			}
			if(temp == 360)
			{
				temp = 0;
			}
			m_h = (int)temp;
	}
}

//CGLColor CColorSpace::GetOppositeColor()
//{
//	int hsv[3];
//	hsv[0] = m_h;
//	hsv[1] = m_s;
//	hsv[2] = m_v;
//
//	hsv[0] = (hsv[0] + 128) % 256;
//
//	// black or white? return opposite
//	if (hsv[2] == 0)
//		return CGLColor(1.0f, 1.0f, 1.0f);
//	else if (hsv[2] == 255)
//		return CGLColor(0.0f, 0.0f, 0.0f);
//
//	// low value? otherwise, adjust that too
//	if (hsv[2] < 128) {
//		hsv[2] = (hsv[2] + 128) % 256;
//	}
//
//	CColorSpace cspace;
//	cspace.SetHSV(hsv[0], hsv[1], hsv[2]);
//
//	return cspace.GetGLColor();
//}

double hue2rgb(double p, double q, double t)
{
	if (t < 0) t += 1;
	if (t > 1) t -= 1;
	if (t < 1 / 6.) return p + (q - p) * 6. * t;
	if (t < 1 / 2.) return q;
	if (t < 2 / 3.) return p + (q - p) * (2 / 3. - t) * 6.;
	return p;
}

CGLColor rgbComplimentary(float r, float g, float b)
{
	// Get array of RGB values
	double max = max(r, g, b);
	double min = min(r, g, b);
	double h, s, l = (max + min) / 2.0;
	
	if (max == min) {
		h = s = 0;  //achromatic
	}
	else {
		double d = max - min;
		s = (l > 0.5 ? d / (2.0 - max - min) : d / (max + min));

		if (max == r && g >= b) {
			h = 1.0472 * (g - b) / d;
		}
		else if (max == r && g < b) {
			h = 1.0472 * (g - b) / d + 6.2832;
		}
		else if (max == g) {
			h = 1.0472 * (b - r) / d + 2.0944;
		}
		else if (max == b) {
			h = 1.0472 * (r - g) / d + 4.1888;
		}
	}

	h = h / 6.2832 * 360.0 + 0;

	// Shift hue to opposite side of wheel and convert to [0-1] value
	h += 180;
	if (h > 360) { h -= 360; }
	h /= 360;

	// Convert h s and l values into r g and b values
	if (s == 0) {
		r = g = b = l; // achromatic
	}
	else {
		double q = l < 0.5 ? l * (1 + s) : l + s - l * s;
		double p = 2 * l - q;

		r = hue2rgb(p, q, h + 1.0 / 3.0);
		g = hue2rgb(p, q, h);
		b = hue2rgb(p, q, h - 1.0 / 3.0);
	}

	return CGLColor((float)r, (float)g, (float)b);
}
