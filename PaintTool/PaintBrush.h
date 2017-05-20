#pragma once
#include <vector>

#include "shape.h"

class CPaintBrush : public IShape
{
public:
	CPaintBrush(int _iStyle, int _iWidth, COLORREF _newColor, int _iStartX, int _iStartY);
	CPaintBrush();
	virtual ~CPaintBrush();

	// Inherited via IShape
	virtual void Draw(HDC _hdc) override;

	void AddPoint(POINT p);

	void SetWidth(int _iNewWidth);

private:
	int m_iStyle;
	int m_iWidth;
	COLORREF m_Color;
	std::vector<POINT> m_vecPointList;
};

