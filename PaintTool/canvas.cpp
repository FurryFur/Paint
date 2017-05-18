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
	while (m_vecpShapes.size() > 0)
	{
		PopShape();
	}
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

		// Make sure the resize encloses the triggering shape
		int iStartX = m_pkShapeThatTriggeredResize->GetStartX();
		int iStartY = m_pkShapeThatTriggeredResize->GetStartY();
		int iEndX = m_pkShapeThatTriggeredResize->GetEndX();
		int iEndY = m_pkShapeThatTriggeredResize->GetEndY();
		int iWidth = std::max(std::max(iStartX, iEndX), static_cast<int>(rect.right - rect.left));
		int iHeight = std::max(std::max(iStartY, iEndY), static_cast<int>(rect.bottom - rect.top));

		Resize(_hwnd, iWidth, iHeight);
	}

	// Clear the back buffer
	m_pBackBuffer->Clear();

	// Draw canvas background
	DrawBackground();

	// Draw all shapes to back buffer
	for (auto it = m_vecpShapes.begin(); it != m_vecpShapes.end(); ++it)
	{
		(*it)->Draw(m_pBackBuffer->GetBFDC());
	}

	// Present the back buffer
	m_pBackBuffer->Present();

	return true;
}

bool CCanvas::DrawBackground()
{
	if (m_hbBackground)
	{
		BITMAP bitmapInfo;
		GetObject(m_hbBackground, sizeof(BITMAP), &bitmapInfo);

		// Create device context to load bitmap into
		HDC hdcBitmap = CreateCompatibleDC(GetBackBuffer()->GetBFDC());

		// Select bitmap into device context
		HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hdcBitmap, m_hbBackground));

		// Bit blip bitmap device context to main device context
		BitBlt(GetBackBuffer()->GetBFDC(), 0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, hdcBitmap, 0, 0, SRCCOPY);

		// Cleanup
		static_cast<HBITMAP>(SelectObject(hdcBitmap, hOldBitmap));
		DeleteDC(hdcBitmap);

		return true;
	}

	return false;
}

void CCanvas::AddShape(IShape * _pShape)
{
	m_vecpShapes.push_back(_pShape);

	_pShape->SetUpdateListener(this);
}

void CCanvas::PopShape()
{
	if (m_vecpShapes.size() > 0)
	{
		delete m_vecpShapes.back();
		m_vecpShapes.pop_back();
	}
}

int CCanvas::GetWidth() const
{
	return m_pBackBuffer->GetWidth();
}

int CCanvas::GetHeight() const
{
	return m_pBackBuffer->GetHeight();
}

void CCanvas::NotifyUpdated(const IShape* _pkShape)
{
	// Check if we need to resize the canvas to enclose the updated shape
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

	// Recreate backbuffer
	delete m_pBackBuffer;
	m_pBackBuffer = new CBackBuffer();
	return m_pBackBuffer->Initialise(_hwnd, _iWidth, _iHeight);
}

void CCanvas::SetBackground(HBITMAP _hbBackground)
{
	m_hbBackground = _hbBackground;
}

CCanvas* CCanvas::Open(HINSTANCE _hInstance, HWND _hwnd)
{
	CCanvas* pCanvas = nullptr;

	char arrcFilename[MAX_PATH];
	OPENFILENAMEA ofn;
	ZeroMemory(&arrcFilename, sizeof(arrcFilename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = _hwnd;  // If you have a window to center over, put its HANDLE here
	ofn.lpstrFilter = "Bitmap Files\0*.bmp\0Any File\0*.*\0";
	ofn.lpstrDefExt = "bmp";
	ofn.lpstrFile = arrcFilename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Open";
	ofn.Flags = OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn))
	{
		HBITMAP hBitMap = static_cast<HBITMAP>(LoadImageA(_hInstance, arrcFilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
		if (!hBitMap)
		{
			MessageBoxA(_hwnd, "Error opening file", "Error", MB_ICONERROR);
		}
		else
		{
			pCanvas = new CCanvas();
			BITMAP bitmapInfo;
			GetObject(hBitMap, sizeof(BITMAP), &bitmapInfo);
			pCanvas->Initialise(_hwnd, bitmapInfo.bmWidth, bitmapInfo.bmHeight);
			pCanvas->SetBackground(hBitMap);
		}
	}

	return pCanvas;
}

void CCanvas::Save(HWND _hwnd)
{
	// Get bitmap info
	BITMAP bitmapInfo;
	GetObject(m_pBackBuffer->GetHBitmap(), sizeof(BITMAP), &bitmapInfo);

	// Create bitmap info header
	BITMAPINFO bmi;
	BITMAPINFOHEADER& bmih = bmi.bmiHeader;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = bitmapInfo.bmWidth;
	bmih.biHeight = bitmapInfo.bmHeight;
	bmih.biPlanes = bitmapInfo.bmPlanes;
	bmih.biBitCount = bitmapInfo.bmBitsPixel;
	// Our bitmap is uncrompressed
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = bitmapInfo.bmWidth * bitmapInfo.bmHeight * 4;

	// Create bitmap file header
	BITMAPFILEHEADER bmfh;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + bmih.biSize;
	bmfh.bfType = 'B' + ('M' << 8);
	bmfh.bfSize = bmfh.bfOffBits + bmih.biSizeImage;

	// Get Bits
	LPVOID lpBits = malloc(bmih.biSizeImage);
	GetDIBits(m_pBackBuffer->GetBFDC(), m_pBackBuffer->GetHBitmap(), 0, bmih.biHeight, lpBits, &bmi, DIB_RGB_COLORS);

	// Get File to save to 
	char arrcFilename[ MAX_PATH ];
	OPENFILENAMEA ofn;
	ZeroMemory(&arrcFilename, sizeof(arrcFilename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = _hwnd;  // If you have a window to center over, put its HANDLE here
	ofn.lpstrFilter = "Bitmap Files\0*.bmp\0Any File\0*.*\0";
	ofn.lpstrDefExt = "bmp";
	ofn.lpstrFile = arrcFilename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Save As";

	// Open file for writing if the user didn't cancel
	if (GetSaveFileNameA(&ofn))
	{
		// Check file opened properly
		FILE* pFile = fopen(arrcFilename, "wb");
		if (!pFile)
		{
			MessageBoxA(_hwnd, "Error opening file", "Error", MB_ICONERROR);
		}
		else
		{
			// Write to file
			fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), pFile);
			fwrite(&bmih, 1, sizeof(BITMAPINFOHEADER), pFile);
			fwrite(lpBits, 1, bmih.biSizeImage, pFile);

			// Cleanup
			fclose(pFile);
		}
	}

	//Cleanup
	free(lpBits);
}