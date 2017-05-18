#ifndef __STAMP_H__
#define __STAMP_H__

#include <Windows.h>
#include <string>

#include "shape.h"
class CStamp : public IShape
{
public:
	CStamp();
	CStamp(HINSTANCE hInstance, HWND _hwnd, const std::string& _rkstrFilename, int, int );
	virtual ~CStamp();

	virtual void Draw(HDC _hdc) override;
	virtual void SetStartX(int _iStartX) override;
	virtual void SetStartY(int _iStartY) override;

private:
	HBITMAP m_hBitMap;
	BITMAP m_bitmapStructure;
};

#endif