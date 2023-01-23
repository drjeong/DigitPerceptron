// ClutterManager.h: interface for the CTextClutterManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLUTTERMANAGER_H__D2076DA4_F8F1_49A2_B04A_5592DD2F6D12__INCLUDED_)
#define AFX_CLUTTERMANAGER_H__D2076DA4_F8F1_49A2_B04A_5592DD2F6D12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <GL\gl.h>
#include "DRect.h"
#include "ColorSpace.h"

using namespace std;

class CftGLFontWrapper;

class CTextInfo: public CDRect
{
public:
	double				m_xpos;
	double				m_ypos;
	double				m_zpos;
	CGLColor			m_color;
	CftGLFontWrapper*	m_pFont;
	CString 			m_szInfo;

public:
	CTextInfo(){m_pFont=NULL;};
	virtual ~CTextInfo(){};
};

class CTextClutterManager
{
public:
	CTextClutterManager(){	};
	virtual ~CTextClutterManager(){removeAll();};

	void draw();
	void removeAll(){m_vecManagedSpace.clear();};
	void addNode(double x, double y, double z, CGLColor color, CString str, CftGLFontWrapper *font);
	
private:
	vector<CTextInfo> m_vecManagedSpace;
};

class CDataItemInfo: public CDRect
{
public:
	unsigned int	m_callist;
	CGLColor		m_color;
	double			m_zpos;
	size_t			m_rkey;		// item row index (0 ~ n-1)
	int				m_cindex;	// Cluster Index (default: -1)
	BOOL			m_highlight;

public:
	CDataItemInfo(){m_cindex=-1;};
	virtual ~CDataItemInfo(){};
};

class CDataItemClutterManager
{
public:
	CDataItemClutterManager();
	virtual ~CDataItemClutterManager();

	void Init(CDRect region);
	void removeAll() {
		m_vecManagedSpace.clear();
	}
	BOOL IsEmpty() {
		return m_vecManagedSpace.size()==0;
	}
	void Draw(GLenum mode);
	void DrawObject(CDRect rt, GLenum mode);
	void DrawObject(unsigned int gllist, CDRect rt, double zpos);
	void AddNode_EvaluateOverlap(size_t rkey/*item index*/, size_t cidx/*cluster index*/,
		unsigned int callist, CDRect rt, double zpos, CGLColor color, BOOL bhilight = FALSE);
	void AddNode_noOverlapCheck(size_t rkey/*item index*/, size_t cidx/*cluster index*/,
		unsigned int callist, const CDRect rt, double zpos, CGLColor color, BOOL bhilight = FALSE);
	BOOL IsPossible2DrawObject(const CDRect rt);

	BOOL IsPossible2AddObject(CDRect rt);

	int FindObject(double mx, double my);

	void SetVisualComplexity(double complexity) {
		m_vecManagedSpace.clear();
		m_dVisualComplexity = complexity;
	}
	double GetVisualComplexity() {
		return m_dVisualComplexity;
	}

private:
	double m_dVisualComplexity;
	vector<CDataItemInfo> m_vecManagedSpace;
	CftGLFontWrapper*	m_pFont;

	Eigen::MatrixXi m_RenderingBuffer;	// rendering buffer - used to check visual cluttering
	CDRect m_glScreenSpace;
};

#endif // !defined(AFX_CLUTTERMANAGER_H__D2076DA4_F8F1_49A2_B04A_5592DD2F6D12__INCLUDED_)
