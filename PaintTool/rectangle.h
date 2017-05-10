#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

//	Library Includes
#include <windows.h>
#include <windowsx.h>
#include "shape.h"


enum EBRUSHSTYLE
{
	NOSTYLE = -1,
	SOLID,
	HATCH
};

class CRectangle : public IShape
{
public:
	CRectangle(EBRUSHSTYLE _eBrushStyle, int _iHatchStyle, COLORREF _FillColor, int _iPenStyle, int _iPenWidth, COLORREF _PenColor, int _iStartX, int _iStartY);
	CRectangle();
	virtual ~CRectangle();

	virtual void Draw(HDC _hdc);
	void SetBrushStyle(EBRUSHSTYLE _eBrushStyle);
	void SetFillColor(COLORREF _newColor);
	void SetPenStyle (int _iPenStyle);
	void SetPenColor(COLORREF _newColor);
	void SetHatchStyle(int _iHatchStyle);
	

private:
	EBRUSHSTYLE m_eBrushStyle;
	int m_iHatchStyle;
	int m_iPenWidth;
	COLORREF m_FillColor;
	int m_iPenStyle;
	COLORREF m_PenColor;
};

#endif