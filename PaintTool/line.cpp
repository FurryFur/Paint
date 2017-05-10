#include "line.h"

CLine::CLine(int _iStyle, int _iWidth, COLORREF _newColor, int _iStartX, int _iStartY) :
	m_iStyle(_iStyle),
	m_iWidth(_iWidth),
	m_Color(_newColor),
	IShape(_iStartX, _iStartY)
{
}

CLine::CLine()
{
}

CLine::~CLine()
{
}

void CLine::Draw(HDC _hdc)
{
	HPEN hPen = CreatePen(PS_SOLID, m_iWidth, m_Color);
	HPEN hOldPen = static_cast<HPEN>(SelectObject(_hdc, hPen));

	MoveToEx(_hdc, m_iStartX, m_iStartY, nullptr);
	LineTo(_hdc, m_iEndX, m_iEndY);

	SelectObject(_hdc, hOldPen);
	DeleteObject(hPen);
}

void CLine::SetWidth(int _iNewWidth)
{
	m_iWidth = _iNewWidth;
}
