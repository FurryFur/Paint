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

	_pShape->SetUpdateListener(this);
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

void CCanvas::Save(HWND _hwnd)
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

	// Get File to save to 
	char arrcFilename[ MAX_PATH ];
	OPENFILENAMEA ofn;
	ZeroMemory(&arrcFilename, sizeof(arrcFilename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
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