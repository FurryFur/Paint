#ifndef __POLYGON_H__
#define __POLYGON_H__

//	Library Includes
#define NOMINMAX
#include <vector>
#include <windows.h>
#include <windowsx.h>

#include "shape.h"
#include "Enum.h"

class CPolygon : public IShape
{
public:
	CPolygon::CPolygon(EBRUSHSTYLE _eBrushStyle, COLORREF _FillColor, int _iPenStyle, int _iPenWidth, COLORREF _PenColor, int _iStartX, int _iStartY);
	CPolygon();
	virtual ~CPolygon();

	virtual void Draw(HDC _hdc);
	void SetBrushStyle(EBRUSHSTYLE _eBrushStyle);
	void SetFillColor(COLORREF _newColor);
	void SetPenStyle(int _iPenStyle);
	void SetPenColor(COLORREF _newColor);

	void AddPoint(POINT p);


private:
	std::vector<POINT> m_vecPointList;
	int m_nPoints;

	EBRUSHSTYLE m_eBrushStyle;
	int m_iPenWidth;
	COLORREF m_FillColor;
	int m_iPenStyle;
	COLORREF m_PenColor;
};

#endif