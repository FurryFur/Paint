#ifndef __ELLIPSE_H__
#define __ELLIPSE_H__

//	Library Includes
#include <windows.h>
#include <windowsx.h>

#include "shape.h"
#include "Enum.h"

class CEllipse : public IShape
{
public:
	CEllipse(EBRUSHSTYLE _eBrushStyle, int _iHatchStyle, COLORREF _FillColor, int _iPenStyle, int _iPenWidth, COLORREF _PenColor, int _iStartX, int _iStartY);
	CEllipse();
	virtual ~CEllipse();

	virtual void Draw(HDC _hdc);
	void SetBrushStyle(EBRUSHSTYLE _eBrushStyle);
	void SetFillColor(COLORREF _newColor);
	void SetPenStyle(int _iPenStyle);
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