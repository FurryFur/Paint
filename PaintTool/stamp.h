#ifndef __STAMP_H__
#define __STAMP_H__

#include <Windows.h>

#include "shape.h"
class CStamp : public IShape
{
public:
	CStamp();
	CStamp(HINSTANCE hInstance, wchar_t* filename, int, int );
	virtual ~CStamp();

	virtual void Draw(HDC _hdc);

private:
	HBITMAP m_hBitMap;
	BITMAP m_bitmapStructure;
};

#endif