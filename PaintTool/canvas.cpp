#include <algorithm>

#include "canvas.h"
#include "backBuffer.h"
#include "shape.h"
#include "stamp.h"

CCanvas::CCanvas() :
	m_pBackBuffer(new CBackBuffer),
	m_bNeedsResize(false)
{
}

CCanvas::~CCanvas()
{
}

bool CCanvas::Initialise(HWND _hwnd, int _iWidth, int _iHeight)
{
	return m_pBackBuffer->Initialise(_hwnd, _iWidth, _iHeight);
}

CBackBuffer * CCanvas::GetBackBuffer()
{
	return m_pBackBuffer;
}

bool CCanvas::Draw(HWND _hwnd)
{
	// Check if the canvas needs resizing
	if (m_bNeedsResize)
	{
		RECT rect;
		GetClientRect(_hwnd, &rect);
		Resize(_hwnd, rect.right - rect.left, rect.bottom - rect.top);
	}

	// Clear the back buffer
	m_pBackBuffer->Clear();

	// Draw all shapes to back buffer
	for (auto it = m_vecShapes.begin(); it != m_vecShapes.end(); ++it)
	{
		(*it)->Draw(m_pBackBuffer->GetBFDC());
	}

	// Present the back buffer
	m_pBackBuffer->Present();

	return true;
}

void CCanvas::AddShape(IShape * _pShape)
{
	m_vecShapes.push_back(_pShape);

	using std::placeholders::_1;
	_pShape->SetUpdateListener(std::bind(&CCanvas::CheckNeedsResize, this, _1));
}

int CCanvas::GetWidth() const
{
	return m_pBackBuffer->GetWidth();
}

int CCanvas::GetHeight() const
{
	return m_pBackBuffer->GetHeight();
}

void CCanvas::CheckNeedsResize(const IShape* _pkShape)
{
	int iWidth = GetWidth();
	int iHeight = GetHeight();
	int iStartX = _pkShape->GetStartX();
	int iStartY = _pkShape->GetStartY();
	int iEndX = _pkShape->GetEndX();
	int iEndY = _pkShape->GetEndY();
	if (iWidth < iStartX || iWidth < iEndX || iHeight < iStartY || iHeight < iEndY)
	{
		m_bNeedsResize = true;
		m_pkShapeThatTriggeredResize = _pkShape;
	}
}

bool CCanvas::Resize(HWND _hwnd, int _iWidth, int _iHeight)
{
	m_bNeedsResize = false;

	// Make sure the resize encloses the triggering shape
	m_pkShapeThatTriggeredResize;
	int iStartX = m_pkShapeThatTriggeredResize->GetStartX();
	int iStartY = m_pkShapeThatTriggeredResize->GetStartY();
	int iEndX = m_pkShapeThatTriggeredResize->GetEndX();
	int iEndY = m_pkShapeThatTriggeredResize->GetEndY();
	_iWidth = std::max(std::max(iStartX, iEndX), _iWidth);
	_iHeight = std::max(std::max(iStartY, iEndY), _iHeight);

	// Recreate backbuffer
	delete m_pBackBuffer;
	m_pBackBuffer = new CBackBuffer();
	return m_pBackBuffer->Initialise(_hwnd, _iWidth, _iHeight);
}

void CCanvas::Save()
{
	// Get bitmap info
	BITMAP bitmapCapture;
	GetObject(m_pBackBuffer->GetHBitmap(), sizeof(BITMAP), &bitmapCapture);

	// Create bitmap info header
	BITMAPINFO bmi;
	BITMAPINFOHEADER& bmih = bmi.bmiHeader;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = bitmapCapture.bmWidth;
	bmih.biHeight = bitmapCapture.bmHeight;
	bmih.biPlanes = bitmapCapture.bmPlanes;
	bmih.biBitCount = bitmapCapture.bmBitsPixel;
	// Our bitmap is uncrompressed
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = bitmapCapture.bmWidth * bitmapCapture.bmHeight * 4;

	// Create bitmap file header
	BITMAPFILEHEADER bmfh;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + bmih.biSize;
	bmfh.bfType = 'B' + ('M' << 8);
	bmfh.bfSize = bmfh.bfOffBits + bmih.biSizeImage;

	// Get Bits
	LPVOID lpBits = malloc(bmih.biSizeImage);
	GetDIBits(m_pBackBuffer->GetBFDC(), m_pBackBuffer->GetHBitmap(), 0, bmih.biHeight, lpBits, &bmi, DIB_RGB_COLORS);

	// Write to file
	FILE* pFile = fopen("test.bmp", "wb");
	fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), pFile);
	fwrite(&bmih, 1, sizeof(BITMAPINFOHEADER), pFile);
	fwrite(lpBits, 1, bmih.biSizeImage, pFile);

	//Cleanup
	fclose(pFile);
	free(lpBits);
}