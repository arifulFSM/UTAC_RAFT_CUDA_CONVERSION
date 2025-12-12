#pragma once
#include "MPList.h"
#include "Coor.h"

class CPattern
{
public:
	CMPList MP;
	virtual CMPoint* FindNearestMP(CCoor& point, float& Distance) { return MP.FindNearest(point, Distance); }
	CCoor* GetCoor(short index);
	virtual BOOL DelPoint(CCoor& p);
	void Renumber();
	void UpdateControl(CListCtrl& Ctrl);
	virtual BOOL EditMP(CCoor& Point, float fX, float fY) { return MP.EditLocMP(Point, fX, fY); }  //20251211 Mahmudul Haque
};

