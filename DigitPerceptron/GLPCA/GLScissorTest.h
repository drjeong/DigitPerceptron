#pragma once

#include "DRect.h"

class CGLScissorTest
{
public:
	CGLScissorTest(CDRect rect);
	~CGLScissorTest();

public:
	double M[16], P[16], px1, py1, px2, py2, pz;
	int V[4];
};

