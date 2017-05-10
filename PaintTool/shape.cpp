#include "shape.h"

IShape::IShape(int _iStartX, int _iStartY) :
	m_iStartX(_iStartX),
	m_iStartY(_iStartY)
{
}

void IShape::SetStartX(int _iStartX)
{
	m_iStartX = _iStartX;
}

void IShape::SetEndX(int _iEndX)
{
	m_iEndX = _iEndX;
}

void IShape::SetStartY(int _iStartY)
{
	m_iStartY = _iStartY;
}

void IShape::SetEndY(int _iEndY)
{
	m_iEndY = _iEndY;
}
