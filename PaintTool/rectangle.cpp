#include "rectangle.h"

CRectangle::CRectangle(EBRUSHSTYLE _eBrushStyle, int _iHatchStyle, COLORREF _FillColor, int _iPenStyle, int _iPenWidth, COLORREF _PenColor, int _iStartX, int _iStartY) :
	m_eBrushStyle(_eBrushStyle),
	m_iHatchStyle(_iHatchStyle),
	m_PenColor(_PenColor),
	m_FillColor(_FillColor),
	m_iPenStyle(_iPenStyle),
	m_iPenWidth(_iPenWidth),
	IShape(_iStartX, _iStartY)
{
}

CRectangle::CRectangle()
{
}

CRectangle::~CRectangle()
{
}

void CRectangle::Draw(HDC _hdc)
{
	// TODO: How to get pen width?
	HPEN hPen = CreatePen(PS_SOLID, m_iPenWidth, m_PenColor);
	HPEN hOldPen = static_cast<HPEN>(SelectObject(_hdc, hPen));

	HBRUSH hBrush;
	switch (m_eBrushStyle)
	{
	case HATCH:
		hBrush = CreateHatchBrush(HS_DIAGCROSS, m_FillColor);
		break;
	case NOSTYLE:
	case SOLID:
	default:
		hBrush = CreateSolidBrush(m_FillColor);
		break;
	}
	HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(_hdc, hBrush));
	
	Rectangle(_hdc, m_iStartX, m_iStartY, m_iEndX, m_iEndY);
	//RECT rectToFill{ m_iStartX, m_iStartY, m_iEndX, m_iEndY };
	//FillRect(_hdc,l &rectToFill, hBrush);

	SelectObject(_hdc, hOldBrush);
	SelectObject(_hdc, hOldPen);
	DeleteObject(hOldBrush);
	DeleteObject(hPen);
}

void CRectangle::SetBrushStyle(EBRUSHSTYLE _eBrushStyle)
{
	m_eBrushStyle =_eBrushStyle;
}

void CRectangle::SetFillColor(COLORREF _newColor)
{
	m_FillColor = _newColor;
}

void CRectangle::SetPenStyle(int _iPenStyle)
{
	m_iPenStyle = _iPenStyle;
}

void CRectangle::SetPenColor(COLORREF _newColor)
{
	m_PenColor = _newColor;
}

void CRectangle::SetHatchStyle(int _iHatchStyle)
{
	m_iHatchStyle = _iHatchStyle;
}
