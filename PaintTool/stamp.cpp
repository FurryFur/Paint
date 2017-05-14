#include "stamp.h"
#include "resource.h"

CStamp::CStamp()
{
}

CStamp::CStamp(HINSTANCE _hInstance, wchar_t * filename, int _iStartX, int _iStartY) :
	IShape(_iStartX, _iStartY)
{
	m_hBitMap = LoadBitmap(_hInstance, MAKEINTRESOURCE(IDB_STAMP));

	BITMAP bitmap;
	GetObject(m_hBitMap, sizeof(BITMAP), &bitmap);
	m_iWidth = bitmap.bmWidth;
	m_iHeight = bitmap.bmHeight;
}

CStamp::~CStamp()
{
}



void CStamp::Draw(HDC _hdc)
{
	// Create device context to load bitmap into
	HDC hdcBitmap = CreateCompatibleDC(_hdc);

	// Select bitmap into device context
	HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hdcBitmap, m_hBitMap));

	// Bit blip bitmap device context to main device context
	BitBlt(_hdc, m_iStartX, m_iStartY, m_iWidth, m_iHeight, hdcBitmap, 0, 0, SRCCOPY);

	// Cleanup
	static_cast<HBITMAP>(SelectObject(hdcBitmap, hOldBitmap));
	DeleteDC(hdcBitmap);
}

void CStamp::SetStartX(int _iStartX)
{
	m_iEndX = _iStartX + m_iWidth;
	IShape::SetStartX(_iStartX);
}

void CStamp::SetStartY(int _iStartY)
{
	m_iEndY = _iStartY + m_iWidth;
	IShape::SetStartY(_iStartY);
}
