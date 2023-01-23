#pragma once
#include <iostream>

using namespace std;

class CColorRef
{
public:
	CColorRef() { val[0] = 0; val[1] = 0; val[2] = 0; };
	CColorRef(int v1, int v2, int v3) { val[0] = v1, val[1] = v2; val[2] = v3; };
	virtual ~CColorRef() {};

public:
	int val[3];
};

class CGLColor  
{
public:
	CGLColor(){R=G=B=0.0f;A=1.0f;};
	virtual ~CGLColor(){};
	CGLColor(float R, float G, float B, float A=1.0f){
		this->R=R;this->G=G;this->B=B;this->A=A;
	}
	CGLColor(int R, int G, int B, int A=255){
		this->R = ConvertRGB2OpenGLFloat(R);
		this->G = ConvertRGB2OpenGLFloat(G);
		this->B = ConvertRGB2OpenGLFloat(B);
		this->A = ConvertRGB2OpenGLFloat(A);
	}
	CGLColor(int color) {
		this->R = ConvertRGB2OpenGLFloat((color / (256 * 256)));
		this->G = ConvertRGB2OpenGLFloat((color / 256) % 256);
		this->B = ConvertRGB2OpenGLFloat(color % 256);
		this->A = ConvertRGB2OpenGLFloat(255);
	}
	CGLColor(COLORREF crefColor)
	{
		this->R = ConvertRGB2OpenGLFloat(GetRValue(crefColor));
		this->G = ConvertRGB2OpenGLFloat(GetGValue(crefColor));
		this->B = ConvertRGB2OpenGLFloat(GetBValue(crefColor));
	}

	COLORREF GetCOLORREF() { return (RGB(Get255R(), Get255G(), Get255B())); }
	CString GetCOLORREFString()
	{
		CString szTmp;
		szTmp.Format(_T("%ld"), GetCOLORREF());
		return szTmp;
	}
	int Get255R() { return((int)(R * 255)); }
	int Get255G() { return((int)(G * 255)); }
	int Get255B() { return((int)(B * 255)); }
	
	CString GetRGB255String()
	{
		CString szTmp;
		szTmp.Format(_T("R: %.f, G: %.f, B: %.f"), R * 255, G * 255, B * 255);
		return szTmp;
	}

	float ConvertRGB2OpenGLFloat(int attribute) {return ((float)(attribute / 255.)); }  // covert RGB 255 to OpenGL Float Color
	int ConvertOpenGLFloat2RGB(float attribute) { return (int)(attribute * 255); }  // covert OpenGL Float Color to RGB 255

	int GetRGB255() {
		return(256 * 256 * ConvertOpenGLFloat2RGB(R) + 256 * ConvertOpenGLFloat2RGB(G) + ConvertOpenGLFloat2RGB(B)); 
	}

	CGLColor &operator=(const CGLColor &color){R = color.R; G = color.G; B = color.B; A = color.A; return *this;};

	bool operator==(const CGLColor &color) {
		return (R == color.R && G == color.G && B == color.B && A == color.A);
	}
	bool operator!=(const CGLColor &color) {
		return (R != color.R || G != color.G || B != color.B || A != color.A);
	}
	friend ostream & operator << (ostream &out, const CGLColor &c) {
		out << "r:" << c.R << " g: " << c.G << " b: " << c.B << " a: " << c.A << endl;
		return out;
	}

public:
	float R;
	float G;
	float B;
	float A;//alpha

	// C = 256^2* R + 256* G + B;
	//R = C / (256 ^ 2);
	//G = (C / 256) % 256;
	//B = C % 256;
};

class CHSVColor
{
public:
	CHSVColor() { H = S = V = A = 0; };
	CHSVColor(int H, int S, int V, int A) {
		this->H = H; this->S = S; this->V = V; this->A = A;
	}
	virtual ~CHSVColor() {};

	int H;
	int S;
	int V;
	int A;//alpha
};

class CColorSpace
{
public:
	CColorSpace(void);
	virtual ~CColorSpace(void);
	CGLColor GetGLColor() { return CGLColor(m_r,m_g,m_b,255); }
	CColorRef GetColorRef() { return CColorRef(m_r, m_g, m_b); }
	CHSVColor GetHSVcolor() { return CHSVColor(m_h,m_s,m_v,255); }
	void SetRGB(int r, int g, int b);
	void SetRGB_R(int r) { SetHSV(r, m_g, m_b); }
	void SetRGB_G(int g) { SetHSV(m_r, g, m_b); }
	void SetRGB_B(int b) { SetHSV(m_r, m_g, b); }
	
	void SetHSV(int h, int s, int v);
	void SetHSV_H(int h) { SetHSV(h, m_s, m_v); }
	void SetHSV_S(int s) { SetHSV(m_h, s, m_v); }
	void SetHSV_V(int v) { SetHSV(m_h, m_s, v); }

	void CaculateRGB();
	void CaculateHSV();
	//CGLColor GetOppositeColor();

public:
	int m_r, m_g, m_b;
	int m_h, m_s, m_v;
};

CGLColor rgbComplimentary(float r, float g, float b);