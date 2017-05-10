#include "canvas.h"
#include "shape.h"
#include "stamp.h"

CCanvas::CCanvas()
{
}

CCanvas::~CCanvas()
{
}

bool CCanvas::Initialise(HWND _hwnd, int _iWidth, int _iHeight)
{
	return true;
}

CBackBuffer * CCanvas::GetBackBuffer()
{
	return nullptr;
}

bool CCanvas::Draw(HDC _hdc)
{
	// Draw all shapes
	for (auto it = m_vecShapes.begin(); it != m_vecShapes.end(); ++it)
	{
		(*it)->Draw(_hdc);
	}

	// Draw all stamps
	for (auto it = m_vecStamps.begin(); it != m_vecStamps.end(); ++it)
	{
		(*it)->Draw(_hdc);
	}

	return true;
}

void CCanvas::Save(HWND _hwnd)
{

}

void CCanvas::AddShape(IShape * _pShape)
{
	m_vecShapes.push_back(_pShape);
}

void CCanvas::AddStamp(CStamp * _pStamp)
{
	m_vecStamps.push_back(_pStamp);
}
