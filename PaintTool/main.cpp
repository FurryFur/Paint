
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>   // Include all the windows headers.
#include <windowsx.h>  // Include useful macros.
#include <typeinfo>

#include "resource.h"
#include "shape.h"
#include "line.h"
#include "canvas.h"
#include "rectangle.h"
#include "ellipse.h"
#include "polygon.h"
#include "stamp.h"
#include "backBuffer.h"

#define WINDOW_CLASS_NAME L"WINCLASS1"

//Global variables
HINSTANCE g_hInstance;
HMENU g_hMenu;

//Enum to decalre the type of tool supported by the application.
enum ESHAPE
{
	FREEHAND = 0,
	LINESHAPE,
	RECTANGLESHAPE,
	ELLIPSESHAPE,
	POLYGONSHAPE,
	STAMP
};


void GameLoop()
{
	//One frame of game logic occurs here...
}

void CheckShapeMenuItem(HMENU _hMenu, UINT _uIDCheckItem)
{
	// Uncheck other menu items
	CheckMenuItem(_hMenu, ID_SHAPE_LINE, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_SHAPE_R, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_SHAPE_ELLIPSE, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_SHAPE_POLYGON, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_ADD_STAMP, MF_UNCHECKED);

	// Check selected menu item
	CheckMenuItem(_hMenu, _uIDCheckItem, MF_CHECKED);
}

void CheckPenWidthMenuItem(HMENU _hMenu, UINT _uIDCheckItem)
{
	// Uncheck other menu items
	CheckMenuItem(_hMenu, ID_WIDTH_SMALL, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_WIDTH_MEDIUM, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_WIDTH_LARGE, MF_UNCHECKED);

	// Check selected menu item
	CheckMenuItem(_hMenu, _uIDCheckItem, MF_CHECKED);
}

LRESULT CALLBACK WindowProc(HWND _hwnd,
	UINT _msg,
	WPARAM _wparam,
	LPARAM _lparam)
{
	// This is the main message handler of the system.
	PAINTSTRUCT ps; // Used in WM_PAINT.
	HDC hdc;        // Handle to a device context.
	static ESHAPE s_eCurShapeTool = LINESHAPE;
	static int s_iCurPenWidth = 5;
	static EBRUSHSTYLE s_eCurBrushStyle = HATCH;
	static int s_iCurPenStyle = PS_SOLID;
	static CCanvas* s_pCanvas;
	static IShape* s_pCurShape = nullptr;
	static int s_iCurMouseX;
	static int s_iCurMouseY;
	HMENU hMenu = GetMenu(_hwnd);
	
	switch (_msg)
	{
	case WM_CREATE:
	{
		// Do initialization stuff here.
		s_pCanvas = new CCanvas();



		// Return Success.
		return (0);
		break;
	}
	case WM_PAINT:
	{
		hdc = BeginPaint(_hwnd, &ps);
		// You would do all your painting here...

		s_pCanvas->Draw(hdc);

		EndPaint(_hwnd, &ps);

		// Return Success.
		return (0);
		break;
	}
	case WM_MOUSEMOVE:
	{
		s_iCurMouseX = static_cast<int>(LOWORD(_lparam));
		s_iCurMouseY = static_cast<int>(HIWORD(_lparam));

		if (s_pCurShape != nullptr)
		{
			if (typeid(*s_pCurShape) == typeid(CStamp))
			{
				s_pCurShape->SetStartX(s_iCurMouseX);
				s_pCurShape->SetStartY(s_iCurMouseY);
			}
			else
			{
				s_pCurShape->SetEndX(s_iCurMouseX);
				s_pCurShape->SetEndY(s_iCurMouseY);
			}

			InvalidateRect(_hwnd, NULL, TRUE);
		}

		return 0;
		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (s_iCurPenStyle != PS_SOLID)
		{
			s_iCurPenWidth = 1;
			CheckPenWidthMenuItem(hMenu, ID_WIDTH_SMALL);
		}

		switch (s_eCurShapeTool)
		{
		case FREEHAND:
			break;
		case LINESHAPE:
			// TODO: Pen width should be set to 1 for all non-solid 
			s_pCurShape = new CLine(s_iCurPenStyle, s_iCurPenWidth, RGB(0, 0, 0), s_iCurMouseX, s_iCurMouseY);
			break;
		case RECTANGLESHAPE:
			s_pCurShape = new CRectangle(s_eCurBrushStyle, 0, RGB(0, 255, 0), s_iCurPenStyle, s_iCurPenWidth, RGB(0, 0, 0), s_iCurMouseX, s_iCurMouseY);
			break;
		case ELLIPSESHAPE:
			s_pCurShape = new CEllipse(s_eCurBrushStyle, 0, RGB(0, 255, 0), s_iCurPenStyle, s_iCurPenWidth, RGB(0, 0, 0), s_iCurMouseX, s_iCurMouseY);
			break;
		case POLYGONSHAPE:
			break;
		case STAMP:
			s_pCurShape = new CStamp(g_hInstance, L"dummy", s_iCurMouseX, s_iCurMouseY);
			break;
		default:
			break;
		}

		s_pCurShape->SetEndX(s_iCurMouseX);
		s_pCurShape->SetEndY(s_iCurMouseY);

		s_pCanvas->AddShape(s_pCurShape);

		InvalidateRect(_hwnd, NULL, TRUE);

		return 0;
		break;
	}
	case WM_LBUTTONUP:
	{
		s_pCurShape = nullptr;

		return 0;
		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(_wparam))
		{
		case ID_FILE_SAVE:
		{
			HDC hdc = GetDC(_hwnd);

			// Create device context to load bitmap into
			HDC hdcCapture = CreateCompatibleDC(hdc);

			// Create capture bitmap
			RECT rect;
			GetClientRect(_hwnd, &rect);
			int iWidth = rect.right - rect.left;
			int iHeight = rect.bottom - rect.top;
			// TODO: Backbuffer and saving out off screen shapes
			HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hdc, iWidth, iHeight);

			// Select bitmap into device context
			HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hdcCapture, hCaptureBitmap));

			// Bit blip from window to capture bitmap 
			BitBlt(hdcCapture, 0, 0, iWidth, iHeight, hdc, 0, 0, SRCCOPY);

			// Save the bitmap
			BITMAP bitmapCapture;
			GetObject(hCaptureBitmap, sizeof(BITMAP), &bitmapCapture);

			// Create bitmap header
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

			// Save to file
			BITMAPFILEHEADER bmfh;
			LONG lImageSize = bmih.biSizeImage;
			LPVOID lpBits = malloc(lImageSize);

			// Get Bits
			GetDIBits(hdc, hCaptureBitmap, 0, bmih.biHeight, lpBits, &bmi, DIB_RGB_COLORS);

			// Save to file
			FILE* pFile = fopen("test.bmp", "wb");
			int iBitsOffset = sizeof(BITMAPFILEHEADER) + bmih.biSize;
			LONG lFileSize = iBitsOffset + lImageSize;
			bmfh.bfType = 'B' + ('M' << 8);
			bmfh.bfOffBits = iBitsOffset;
			bmfh.bfSize = lFileSize;

			fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), pFile);
			fwrite(&bmih, 1, sizeof(BITMAPINFOHEADER), pFile);
			fwrite(lpBits, 1, lImageSize, pFile);

			// Cleanup
			fclose(pFile);
			free(lpBits);
			static_cast<HBITMAP>(SelectObject(hdcCapture, hOldBitmap));
			DeleteDC(hdcCapture);
			DeleteObject(hCaptureBitmap);
			ReleaseDC(_hwnd, hdc);

			break;
		}
		case ID_FILE_EXIT:
		{
			PostQuitMessage(0);
			break;
		}
		case ID_SHAPE_LINE:
		{
			s_eCurShapeTool = LINESHAPE;

			CheckShapeMenuItem(hMenu, ID_SHAPE_LINE);

			break;
		}
		case ID_SHAPE_R:
		{
			s_eCurShapeTool = RECTANGLESHAPE;

			CheckShapeMenuItem(hMenu, ID_SHAPE_R);

			break;
		}
		case ID_SHAPE_ELLIPSE:
		{
			s_eCurShapeTool = ELLIPSESHAPE;

			CheckShapeMenuItem(hMenu, ID_SHAPE_ELLIPSE);

			break;
		}
		case ID_SHAPE_POLYGON:
		{
			s_eCurShapeTool = POLYGONSHAPE;

			CheckShapeMenuItem(hMenu, ID_SHAPE_POLYGON);

			break;
		}
		case ID_ADD_STAMP:
		{
			s_eCurShapeTool = STAMP;

			CheckShapeMenuItem(hMenu, ID_ADD_STAMP);

			break;
		}
		case ID_WIDTH_SMALL:
		{
			s_iCurPenWidth = 1;

			CheckPenWidthMenuItem(hMenu, ID_WIDTH_SMALL);

			break;
		}
		case ID_WIDTH_MEDIUM:
		{
			s_iCurPenWidth = 5;

			CheckPenWidthMenuItem(hMenu, ID_WIDTH_MEDIUM);

			break;
		}
		case ID_WIDTH_LARGE:
		{
			s_iCurPenWidth = 10;

			CheckPenWidthMenuItem(hMenu, ID_WIDTH_LARGE);

			break;
		}
		case ID_STYLE_SOLID:
		{
			s_iCurPenStyle = PS_SOLID;
			break;
		}
		case ID_STYLE_DOT:
		{
			s_iCurPenStyle = PS_DOT;
			break;
		}
		case ID_STYLE_DASH:
		{
			s_iCurPenStyle = PS_DASH;
			break;
		}
		case ID_STYLE_DASHDOT:
		{
			s_iCurPenStyle = PS_DASHDOT;
			break;
		}
		case ID_STYLE_DASHDOTDOT:
		{
			s_iCurPenStyle = PS_DASHDOTDOT;
			break;
		}
		case ID_HELP_ABOUT:
		{
			MessageBox(_hwnd, L"This paint tool was developed by Lance Chaney", L"Author Information", MB_OK | MB_ICONINFORMATION);
			break;
		}
		default:
			break;
		}

		return(0);
		break;
	}
	case WM_DESTROY:
	{
		// Kill the application, this sends a WM_QUIT message.
		PostQuitMessage(0);

		// Return success.
		return (0);
		break;
	}
	default:
		break;
	} // End switch.

	  // Process any messages that we did not take care of...

	return (DefWindowProc(_hwnd, _msg, _wparam, _lparam));
}

int WINAPI WinMain(HINSTANCE _hInstance,
	HINSTANCE _hPrevInstance,
	LPSTR _lpCmdLine,
	int _nCmdShow)
{
	WNDCLASSEX winclass; // This will hold the class we create.
	HWND hwnd;           // Generic window handle.
	MSG msg;             // Generic message.

	g_hInstance = _hInstance;
	// First fill in the window class structure.
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = _hInstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground =
		static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register the window class
	if (!RegisterClassEx(&winclass))
	{
		return (0);
	}

	g_hMenu = LoadMenu(_hInstance, MAKEINTRESOURCE(IDR_MENU1));
	// create the window
	hwnd = CreateWindowEx(NULL, // Extended style.
		WINDOW_CLASS_NAME,      // Class.
		L"Lance's Paint Tool",   // Title.
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100, 100,                    // Initial x,y.
		1500, 800,                // Initial width, height.
		NULL,                   // Handle to parent.
		g_hMenu,                   // Handle to menu.
		_hInstance,             // Instance of this application.
		NULL);                  // Extra creation parameters.

	if (!(hwnd))
	{
		return (0);
	}



	// Enter main event loop
	while (true)
	{
		// Test if there is a message in queue, if so get it.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Test if this is a quit.
			if (msg.message == WM_QUIT)
			{
				break;
			}

			// Translate any accelerator keys.
			TranslateMessage(&msg);
			// Send the message to the window proc.
			DispatchMessage(&msg);
		}

		// Main game processing goes here.
		GameLoop(); //One frame of game logic occurs here...
	}

	// Return to Windows like this...
	return (static_cast<int>(msg.wParam));
}
