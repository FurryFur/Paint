#include <algorithm>

#include "PaintBrush.h"

CPaintBrush::CPaintBrush(int _iStyle, int _iWidth, COLORREF _newColor, int _iStartX, int _iStartY) :
	m_iStyle(_iStyle),
	m_iWidth(_iWidth),
	m_Color(_newColor),
	IShape(_iStartX, _iStartY)
{
}

CPaintBrush::CPaintBrush() :
	IShape()
{
}


CPaintBrush::~CPaintBrush()
{
}

void CPaintBrush::Draw(HDC _hdc)
{
	for (int i = 1; i < m_vecPointList.size(); ++i)
	{
		POINT& rP1 = m_vecPointList[i - 1];
		POINT& rP2 = m_vecPointList[i];

		HPEN hPen = CreatePen(m_iStyle, m_iWidth, m_Color);
		HPEN hOldPen = static_cast<HPEN>(SelectObject(_hdc, hPen));

		MoveToEx(_hdc, rP1.x, rP1.y, nullptr);
		LineTo(_hdc, rP2.x, rP2.y);

		SelectObject(_hdc, hOldPen);
		DeleteObject(hPen);
	}
}

void CPaintBrush::AddPoint(POINT p)
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

void CPaintBrush::SetWidth(int _iNewWidth)
{
	m_iWidth = _iNewWidth;
}
