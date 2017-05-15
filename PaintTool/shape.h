#ifndef __SHAPE_H__
#define __SHAPE_H__

//	Library Includes
#include <functional>
#include <windows.h>
#include <windowsx.h>

class IUpdateListener;

class IShape
{
public:
	virtual void Draw(HDC _hdc) = 0;

	virtual void SetStartX(int _iStartX);
	virtual void SetEndX(int _iEndX);
	virtual void SetStartY(int _iStartX);
	virtual void SetEndY(int _iStartX);
	int GetStartX() const;
	int GetStartY() const;
	int GetEndX() const;
	int GetEndY() const;
	void IShape::SetUpdateListener(IUpdateListener* _pUpdateListener);
	//void SetColor(COLORREF _newColor);

protected:
	IShape();
	IShape(int _iStartX, int _iStartY);
	virtual ~IShape() {}

	void NotifyUpdated();

	int m_iWidth;
	int m_iHeight;
	int m_iStartX;
	int m_iEndX;
	int m_iStartY;
	int m_iEndY;
	//COLORREF m_ColorRef;
	IUpdateListener* m_pUpdateListener;
};

#endif