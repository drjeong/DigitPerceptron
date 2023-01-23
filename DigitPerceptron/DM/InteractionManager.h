#pragma once

#include <vector>
#include <map>
#include <algorithm>
using namespace std;

#define NULLSELECTED -1

class CInteractionManager
{
public:
	CInteractionManager(void *parent);
	~CInteractionManager(void);

	typedef enum { OBJECT_INSTANCE, OBJECT_BIN } OBJ_TYPE;
	void Init();

	void SetPCAXCoord(int coord){m_nCheckedPCACoordX=coord;}
	void SetPCAYCoord(int coord){m_nCheckedPCACoordY=coord;}
	void SetSelectItem(int rkey/*row key*/);
	void SetSelectUnselectItem(int rkey/*row key*/);
	void SetDeleteItem(int rkey/*row key*/);
	void SetSelectUnselectDimension(int ckey);
	void SetHighlightDimension(int ckey){m_nHighlightedDimension=ckey;}
	void SetHighlightObject(int rkey, OBJ_TYPE type = OBJECT_INSTANCE){
		m_nHighlightedObject = rkey;
		m_nHighlightedObjectType = type;
	}
	void SetDataManipulationItem(int rkey){ m_nDataManipulationItem =rkey;}

	int GetPCAXCoord(){return m_nCheckedPCACoordX;}
	int GetPCAYCoord(){return m_nCheckedPCACoordY;}
	int GetNumDeletedItems(){return (int)m_nDeletedItems.size();}
	int GetNumSelectedItems(){return (int)m_nSelectedItems.size();}
	int GetSelectedDimension(){return m_nSelectedDimension;}
	int GetHighlightedDimension(){return m_nHighlightedDimension;}
	int GetHighlightedObject(){return m_nHighlightedObject;}
	OBJ_TYPE GetHighlightedObjectType() { return m_nHighlightedObjectType; }
	int GetDataManipulationItem() { return m_nDataManipulationItem; }

	auto GetDeletedItems() { return &m_nDeletedItems; }
	auto GetSelectedItems(){ return &m_nSelectedItems; }

	BOOL IsItemSelectionEnabled() {
		return m_bItemSelection;
	}
	BOOL IsItemSelected(int rkey/*row key*/);
	BOOL IsItemHighlighted(int rkey/*row key*/){return (m_nHighlightedObject==rkey);}

	void ResetHighlightedItem() { m_nHighlightedObject = -1; }
	BOOL ResetSelectedItems();
	BOOL ResetDeletedItems();

	void RefreshViews();

	void SetShowLabel() { m_bShowLabel ^= TRUE; }
	void SetTrail() { m_bEnableTrail ^= TRUE; }
	BOOL IsShowLabel() { return m_bShowLabel; }
	BOOL IsTrailEnabled() { return m_bEnableTrail; }

private:
	BOOL	m_bShowLabel;
	BOOL	m_bEnableTrail;
	BOOL	m_bItemSelection;

	void *m_pParent; // CDataManager
	int	m_nCheckedPCACoordX;	// col key (1st PCA dimension)
	int m_nCheckedPCACoordY;	// col key (2nd PCA dimension)
	int m_nSelectedDimension; // Selected Variable (col key)
	vector<int/*row key*/> m_nSelectedItems;
	vector<int/*row key*/> m_nDeletedItems;
	int m_nHighlightedDimension;
	int m_nHighlightedObject; // highlighted instance
	OBJ_TYPE m_nHighlightedObjectType; // highlighted object type (OBJ_INSTANCE, OBJ_BIN)
	int m_nDataManipulationItem; // currently selected instance for data manipulation
};

