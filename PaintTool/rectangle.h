#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

//	Library Includes
#include <windows.h>
#include <windowsx.h>
#include "shape.h"
#include "Enum.h"

class CRectangle : public IShape
{
public:
	CRectangle(EBRUSHSTYLE _eBrushStyle, COLORREF _FillColor, int _iPenStyle, int _iPenWidth, COLORREF _PenColor, int _iStartX, int _iStartY);
	CRectangle();
	virtual ~CRectangle();

	virtual void Draw(HDC _hdc) override;
	void SetBrushStyle(EBRUSHSTYLE _eBrushStyle);
	void SetFillColor(COLORREF _newColor);
	void SetPenStyle (int _iPenStyle);
	void SetPenColor(COLORREF _newColor);
	

private:
	EBRUSHSTYLE m_eBrushStyle;
	int m_iPenWidth;
	COLORREF m_FillColor;
	int m_iPenStyle;
	COLORREF m_PenColor;
};

#endif