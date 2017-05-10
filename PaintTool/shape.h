#ifndef __SHAPE_H__
#define __SHAPE_H__

//	Library Includes
#include <windows.h>
#include <windowsx.h>

class IShape
{
public:
	virtual void Draw(HDC _hdc) = 0;

	void SetStartX(int _iStartX);
	void SetEndX(int _iEndX);
	void SetStartY(int _iStartX);
	void SetEndY(int _iStartX);
	//void SetColor(COLORREF _newColor);

protected:
	IShape() {};
	IShape(int _iStartX, int _iStartY);
	virtual ~IShape() {}

	int m_iStartX;
	int m_iEndX;
	int m_iStartY;
	int m_iEndY;
	//COLORREF m_ColorRef;
};

#endif