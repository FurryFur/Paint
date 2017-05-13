#include "shape.h"

IShape::IShape(int _iStartX, int _iStartY) :
	m_iStartX(_iStartX),
	m_iStartY(_iStartY)
{
}

void IShape::NotifyUpdated()
{
	if (m_fnUpdateListener)
	{
		m_fnUpdateListener(this);
	}
}

void IShape::SetStartX(int _iStartX)
{
	NotifyUpdated();
	m_iStartX = _iStartX;
}

void IShape::SetEndX(int _iEndX)
{
	NotifyUpdated();
	m_iEndX = _iEndX;
}

void IShape::SetStartY(int _iStartY)
{
	NotifyUpdated();
	m_iStartY = _iStartY;
}

void IShape::SetEndY(int _iEndY)
{
	NotifyUpdated();
	m_iEndY = _iEndY;
}

int IShape::GetStartX() const
{
	return  m_iStartX;
}

int IShape::GetStartY() const
{
	return m_iStartY;
}

int IShape::GetEndX() const
{
	return m_iEndX;
}

int IShape::GetEndY() const
{
	return m_iEndY;
}

void IShape::SetUpdateListener(std::function<void(const IShape*)> _fn)
{
	m_fnUpdateListener = _fn;

	NotifyUpdated();
}
