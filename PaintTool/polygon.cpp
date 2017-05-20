#include <algorithm>

#include "polygon.h"

CPolygon::CPolygon(EBRUSHSTYLE _eBrushStyle, COLORREF _FillColor, int _iPenStyle, int _iPenWidth, COLORREF _PenColor, int _iStartX, int _iStartY) :
	m_eBrushStyle(_eBrushStyle),
	m_PenColor(_PenColor),
	m_FillColor(_FillColor),
	m_iPenStyle(_iPenStyle),
	m_iPenWidth(_iPenWidth),
	m_vecPointList{{_iStartX, _iStartY}},
	IShape(_iStartX, _iStartY)
{
}

CPolygon::CPolygon()
{
}

CPolygon::~CPolygon()
{
}

void CPolygon::Draw(HDC _hdc)
{
	HPEN hPen = CreatePen(m_iPenStyle, m_iPenWidth, m_PenColor);
	HPEN hOldPen = static_cast<HPEN>(SelectObject(_hdc, hPen));

	HBRUSH hBrush;
	switch (m_eBrushStyle)
	{
	case HATCH:
		hBrush = CreateHatchBrush(HS_DIAGCROSS, m_FillColor);
		break;
	case NOSTYLE:
		hBrush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
		break;
	case SOLID:
	default:
		hBrush = CreateSolidBrush(m_FillColor);
		break;
	}
	HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(_hdc, hBrush));

	Polygon(_hdc, m_vecPointList.data(), m_vecPointList.size());
	//RECT rectToFill{ m_iStartX, m_iStartY, m_iEndX, m_iEndY };
	//FillRect(_hdc,l &rectToFill, hBrush);

	SelectObject(_hdc, hOldBrush);
	SelectObject(_hdc, hOldPen);
	DeleteObject(hBrush);
	DeleteObject(hPen);
}

void CPolygon::AddPoint(POINT p)
{
	m_vecPointList.push_back(p);

	// Update bounding rect to notify canvas with
	m_iStartX = std::min(p.x, static_cast<long>(m_iStartX));
	m_iStartY = std::min(p.y, static_cast<long>(m_iStartY));
	m_iEndX = std::max(p.x, static_cast<long>(m_iEndX));
	m_iEndY = std::max(p.y, static_cast<long>(m_iEndY));

	// Notify canvas of update to shape
	NotifyUpdated();
}

void CPolygon::SetBrushStyle(EBRUSHSTYLE _eBrushStyle)
{
	m_eBrushStyle = _eBrushStyle;
}

void CPolygon::SetFillColor(COLORREF _newColor)
{
	m_FillColor = _newColor;
}

void CPolygon::SetPenStyle(int _iPenStyle)
{
	m_iPenStyle = _iPenStyle;
}

void CPolygon::SetPenColor(COLORREF _newColor)
{
	m_PenColor = _newColor;
}
