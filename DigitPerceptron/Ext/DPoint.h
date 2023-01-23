// Point.h: interface for the CDPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINT_H__BD8246D7_339F_420D_A06A_D480C43C4B2A__INCLUDED_)
#define AFX_POINT_H__BD8246D7_339F_420D_A06A_D480C43C4B2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ColorSpace.h"
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

class CDPoint  
{
public:
	CDPoint(){x = y = std::nan("0");};
	CDPoint(double x, double y){this->x = x;this->y = y;};
	virtual ~CDPoint(){};
	
	double GetX() { return x; }
	double GetY() { return y; }

	//void operator=(const CDPoint &pt){x=pt.x; y=pt.y;};
	bool operator==(const CDPoint &pt){
		return (x == pt.x && y == pt.y);}
	bool operator!=(const CDPoint &pt){
		return (x != pt.x || y != pt.y);}
	CDPoint operator*(const double &scalar) {
		CDPoint pt;
		pt.x = this->x * scalar;
		pt.y = this->y * scalar;
		return pt;
	}
	CDPoint operator-(const CDPoint &pt) {
		CDPoint newpt;
		newpt.x = this->x - pt.x;
		newpt.y = this->y - pt.y;
		return newpt;
	}
	CDPoint operator+(const CDPoint &pt) {
		CDPoint newpt;
		newpt.x = this->x + pt.x;
		newpt.y = this->y + pt.y;
		return newpt;
	}
	
	bool IsEmpty() { if (isnan(x) || isnan(y)) return TRUE; return FALSE; };
	void Null() { x = y = std::nan("0"); };

	friend ostream & operator << (ostream &out, const CDPoint &c) {
		out << "x:" << c.x << " y: " << c.y << endl;
		return out;
	}

public:
	double x;
	double y;
};
typedef vector<CDPoint>POINTLIST;

class C2DVector : public CDPoint
{
public:
	C2DVector():CDPoint(){ };
	C2DVector(double x, double y) :CDPoint(x, y) {};
};

class C2DMinMax: public CDPoint
{
public:
	C2DMinMax() :CDPoint() { };
	C2DMinMax(double x, double y) :CDPoint(x, y) {};
	friend ostream & operator << (ostream &out, const C2DMinMax &c){
		out << "min:" << c.x << " max: " << c.y << endl;
		return out;
	}

	void SetValue(double val) {
		if (IsEmpty()) {
			SetMax(val); SetMin(val);
		}
		else{
			if (val < GetMin()) SetMin(val);
			if (val > GetMax()) SetMax(val);
		}
	}
	double GetMin() { return GetX(); }
	double GetMax() { return GetY(); }
	void SetMin(double val) { x = val; }
	void SetMax(double val) { y = val; }
	double GetDiff() { return (GetMax() - GetMin()); } // max - min
};

class CDPoint3D
{
public:
	CDPoint3D(){x=y=z= std::nan("0");};
	CDPoint3D(double x, double y, double z){this->x = x;this->y = y;this->z = z;};
	virtual ~CDPoint3D(){};

	bool IsEmpty() { if (isnan(x) || isnan(y) || isnan(z)) return TRUE; return FALSE; };

	void operator=(const CDPoint3D &pt){x=pt.x; y=pt.y; z=pt.z;};
	bool operator==(const CDPoint3D &pt){
		return (x == pt.x && y == pt.y && z == pt.z);}
	bool operator!=(const CDPoint3D &pt){
		return (x != pt.x || y != pt.y || z != pt.z);}

public:
	double x;
	double y;
	double z;
	CGLColor color;
};

#endif // !defined(AFX_POINT_H__BD8246D7_339F_420D_A06A_D480C43C4B2A__INCLUDED_)
