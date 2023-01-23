// Rect.h: interface for the CDRect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRECT_H__76CC92ED_EA59_41B7_BE67_2FF8A489E7E2__INCLUDED_)
#define AFX_DRECT_H__76CC92ED_EA59_41B7_BE67_2FF8A489E7E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include <map>
using namespace std;

#include "DPoint.h"

typedef struct tagDRECT
{
    double    left;
    double    top;
    double    right;
    double    bottom;
	tagDRECT()
	{
		left = std::nan("0");
		top = std::nan("0");
		right = std::nan("0");
		bottom = std::nan("0");
	}
} DRECT, *PDRECT;

class CDRect : public tagDRECT
{

public:
	CDRect(DRECT pt){left=pt.left; bottom=pt.bottom; right=pt.right; top=pt.top;};
	CDRect(PDRECT pt){left=pt->left; bottom=pt->bottom; right=pt->right; top=pt->top;};
	CDRect(){left = std::nan("0"); bottom = std::nan("0"); right = std::nan("0"); top = std::nan("0");};
	CDRect(double l, double b, double r, double t){left=l; bottom=b; right=r; top=t;};
	CDRect(CDPoint center, double h, double w){
		VERIFY(h > 0 && w > 0);
		left = center.GetX() - w/2;
		top = center.GetY() + h/2;
		right = center.GetX() + w/2;
		bottom = center.GetY() - h/2;
	}

	virtual ~CDRect(){};
	double  Width(){return (right-left);};
	double  Height(){return (top-bottom);};
	double	GetHorizontalCenter(){return (left + right)/2.;};
	double	GetVerticalCenter(){return (top + bottom)/2.;};
	CDPoint	GetCenterPos(){
		return CDPoint(GetHorizontalCenter(), GetVerticalCenter());
	};
	CString GetString()
	{
		CString szTmp;
		szTmp.Format(_T("l:%.1f r:%.1f t:%.1f b:%.1f"), left, right, top, bottom);
		return szTmp;
	}
	void	SetRectEmpty(){Empty();};
	void	Empty(){left = right = top = bottom = std::nan("0");};
	bool	IsRectEmpty(){return (isnan(left) || isnan(right) || isnan(top) || isnan(bottom));};
	bool	IsInside(double x, double y){return (x>left&&x<right&&y<top&&y>bottom);};
	bool	IsRectsOverlap(CDRect trt){
		if ((this->left > trt.right) || (this->right < trt.left)) return false;
		if ((this->bottom > trt.top) || (this->top < trt.bottom)) return false;
		return true;
	};
	bool	IsOverLapped(CDRect trt){
		CDPoint spt = this->GetCenterPos();
		CDPoint tpt = trt.GetCenterPos();
		if (this->IsInside(tpt.GetX(), tpt.GetY()))
			return true;
		if (trt.IsInside(spt.GetX(), spt.GetY()))
			return true;
		// left top
		spt = CDPoint(this->left, this->top);
		tpt = CDPoint(trt.left, trt.top);
		if (this->IsInside(tpt.GetX(), tpt.GetY()))
			return true;
		if (trt.IsInside(spt.GetX(), spt.GetY()))
			return true;
		// left bottom
		spt = CDPoint(this->left, this->bottom);
		tpt = CDPoint(trt.left, trt.bottom);
		if (this->IsInside(tpt.GetX(), tpt.GetY()))
			return true;
		if (trt.IsInside(spt.GetX(), spt.GetY()))
			return true;
		// right top
		spt = CDPoint(this->right, this->top);
		tpt = CDPoint(trt.right, trt.top);
		if (this->IsInside(tpt.GetX(), tpt.GetY()))
			return true;
		if (trt.IsInside(spt.GetX(), spt.GetY()))
			return true;
		//right bottom
		spt = CDPoint(this->right, this->bottom);
		tpt = CDPoint(trt.right, trt.bottom);
		if (this->IsInside(tpt.GetX(), tpt.GetY()))
			return true;
		if (trt.IsInside(spt.GetX(), spt.GetY()))
			return true;
		return false;
	};

//	DPOINT	CenterPoint(){DPOINT pt; pt.GetX() = GetHorizonCenter(); pt.GetY() = GetVerticalCenter(); return pt;};
	bool	IsVerticalInside(double &v){return (v<top&&v>bottom);};
	bool	IsHorizontalInside(double &v){return (v>left&&v<right);};
	
	void operator-=(const CDRect &rt){left-=rt.left;right-=rt.right;top-=rt.top;bottom-=rt.bottom;};
	void operator-(const CDRect &rt){left-=rt.left;right-=rt.right;top-=rt.top;bottom-=rt.bottom;};
	CDRect &operator=(const CDRect &rt){left=rt.left;right=rt.right;top=rt.top;bottom=rt.bottom; return *this;};
	bool operator==(const CDRect &rt){
		return (left == rt.left && top == rt.top && right == rt.right && bottom == rt.bottom);}
	bool operator!=(const CDRect &rt){
		return (left != rt.left || top != rt.top || right != rt.right || bottom != rt.bottom);}

	void	SetRect(double l, double b, double r, double t){left=l;top=t;right=r;bottom=b;};
	void	SetRect(const CDRect &rt){left=rt.left;top=rt.top;right=rt.right;bottom=rt.bottom;};
	void	SetRect(CDPoint pt, double h, double w){left=pt.GetX();top=pt.GetY()+h;right=pt.GetX()+w;bottom=pt.GetY();};
	void	SetHorizonCenter(double v){double g = v - (left+right)/2;	right += g; left += g;};
	void	SetVerticalCenter(double v){double g = v - (top+bottom)/2;	top += g; bottom += g;};

	void	SetCenterPos(CDPoint pt)
	{
		double h = Height();	double w = Width();
		double mh = Height()/2;	double mw = Width()/2;
		top = pt.GetY() + mh; bottom = pt.GetY() - mh; left = pt.GetX() - mw; right = pt.GetX() + mw;
	};

	void	SetCenterPos(double x, double y, CDRect *ref = NULL)
	{
		double h = Height();	double w = Width();
		double mh = Height()/2;	double mw = Width()/2;
		top = y + mh; bottom = y - mh; left = x - mw; right = x + mw;
		if (ref != NULL)
		{
			if (top > ref->top)
			{
				top = ref->top;
				bottom = top - h;
			}
			if (left < ref->left)
			{
				left = ref->left;
				right =left + w;
			}
			if (bottom < ref->bottom)
			{
				bottom = ref->bottom;
				top = bottom + h;
			}
			if (right > ref->right)
			{
				right = ref->right;
				left =right - w;
			}
		}
	}
	
	// expand
	void	InflateRect(const CDRect &rt){left-=rt.left;top+=rt.top;right+=rt.right;bottom-=rt.bottom;};
	void	InflateRect(double x, double y){left-=x;right+=x;top+=y;bottom-=y;};
	void	InflateRect(double l, double b, double r, double t){left-=l;top+=t;right+=r;bottom-=b;};
	void	InflateLeft(double v) { left = left - v; };
	void	InflateRight(double v) { right = right + v; };

	// shink
	void	DeflateRect(double x, double y){left+=x;right-=x;top-=y;bottom+=y;};
	void	DeflateRect(double l, double b, double r, double t){left+=l;top-=t;right-=r;bottom+=b;};
	void	DeflateLeft(double v) { left = left + v; };
	void	DeflateRight(double v) { right = right - v; };

	// offset change
	void	OffsetPos(double l, double b, double r, double t){left+=l; bottom+=b; right+=r; top+=t;};
	void	OffsetRect(double x, double y){ left+=x; right+=x; top+=y; bottom+=y;};
	void	OffsetLeft(double v){right-=(left-v); left=v; };
	void	OffsetRight(double v){left-=(right-v); right=v; };
	void	OffsetTop(double v){bottom-=(top-v); top=v; };
	void	OffsetBottom(double v){top-=(bottom-v); bottom=v; };

	// move
	void	MoveLeft(double v){right-=v; left-=v; };
	void	MoveRight(double v){left+=v; right+=v; };
	void	MoveDown(double v){bottom-=v; top-=v; };
	void	MoveUp(double v){bottom+=v; top+=v; };

	void	AlignWithin(const CDRect rt) {
		// making sure the region is inside of the reference region
		if (this->left < rt.left)
			this->MoveRight(abs(rt.left - this->left));
		if (this->right > rt.right)
			this->MoveLeft(abs(this->right - rt.right));
		if (this->top > rt.top)
			this->MoveDown(abs(this->top - rt.top));
		if (this->bottom < rt.bottom)
			this->MoveUp(abs(rt.bottom - this->bottom));
	}

	inline void CDRect::NormalizeRect() throw()
	{
		double nTemp;
		if (left > right) // if left value is smaller than its right value
		{
			nTemp = left;
			left = right;
			right = nTemp;
		}
		if (top > bottom) // if top value is smaller than its bottom value
		{
			nTemp = top;
			top = bottom;
			bottom = nTemp;
		}
	}

	friend ostream & operator << (ostream &out, const CDRect &c) {
		out << "l:" << c.left << " r: " << c.right << " t: " << c.top << " b: " << c.bottom << endl;
		return out;
	}
};

typedef std::vector<CDRect>DRECT_LIST;
typedef std::map<int, CDRect>DRECTnIDX_LIST;

#endif // !defined(AFX_DRECT_H__76CC92ED_EA59_41B7_BE67_2FF8A489E7E2__INCLUDED_)
    
