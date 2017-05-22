
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>   // Include all the windows headers.
#include <windowsx.h>  // Include useful macros.
#include <typeinfo>
#include <string>

#include "resource.h"
#include "shape.h"
#include "line.h"
#include "canvas.h"
#include "rectangle.h"
#include "ellipse.h"
#include "polygon.h"
#include "stamp.h"
#include "backBuffer.h"
#include "PaintBrush.h"

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
	STAMP,
	BRUSHSHAPE
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
	CheckMenuItem(_hMenu, ID_STAMP_DEFAULT, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_STAMP_FROMFILE, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_SHAPE_PAINTBRUSH, MF_UNCHECKED);

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

void CheckPenStyleMenuItem(HMENU _hMenu, UINT _uIDCheckItem)
{
	// Uncheck other menu items
	CheckMenuItem(_hMenu, ID_STYLE_SOLID, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_STYLE_DOT, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_STYLE_DASH, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_STYLE_DASHDOT, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_STYLE_DASHDOTDOT, MF_UNCHECKED);

	// Check selected menu item
	CheckMenuItem(_hMenu, _uIDCheckItem, MF_CHECKED);
}

void CheckBrushStyleMenuItem(HMENU _hMenu, UINT _uIDCheckItem)
{
	// Uncheck other menu items
	CheckMenuItem(_hMenu, ID_STYLE_TRANSPARENT, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_STYLE_FILL, MF_UNCHECKED);
	CheckMenuItem(_hMenu, ID_STYLE_HATCH, MF_UNCHECKED);

	// Check selected menu item
	CheckMenuItem(_hMenu, _uIDCheckItem, MF_CHECKED);
}

void DisablePenWidthMenu(HMENU _hMenu)
{
	EnableMenuItem(_hMenu, ID_WIDTH_SMALL, MF_GRAYED);
	EnableMenuItem(_hMenu, ID_WIDTH_MEDIUM, MF_GRAYED);
	EnableMenuItem(_hMenu, ID_WIDTH_LARGE, MF_GRAYED);
}

void EnablePenWidthMenu(HMENU _hMenu)
{
	EnableMenuItem(_hMenu, ID_WIDTH_SMALL, MF_ENABLED);
	EnableMenuItem(_hMenu, ID_WIDTH_MEDIUM, MF_ENABLED);
	EnableMenuItem(_hMenu, ID_WIDTH_LARGE, MF_ENABLED);
}

void ColorChooser(HWND _hwnd, COLORREF& _colCur)
{
	CHOOSECOLOR cc;                 // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 

									// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = _hwnd;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = _colCur;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE)
	{
		_colCur = cc.rgbResult;
	}
}

LRESULT CALLBACK WindowProc(HWND _hwnd,
	UINT _msg,
	WPARAM _wparam,
	LPARAM _lparam)
{
	// This is the main message handler of the system.
	static ESHAPE s_eCurShapeTool = LINESHAPE;
	static int s_iCurPenWidth = 5;
	static int s_iSavedPenWidth = -1;
	static EBRUSHSTYLE s_eCurBrushStyle = HATCH;
	static int s_iCurPenStyle = PS_SOLID;
	static CCanvas* s_pCanvas;
	static IShape* s_pCurShape = nullptr;
	static int s_iCurMouseX;
	static int s_iCurMouseY;
	static COLORREF s_colCurPen = RGB(40, 128, 255);
	static COLORREF s_colCurBrush = RGB(0, 255, 0);
	static std::string s_strStampFilename;

	HMENU hMenu = GetMenu(_hwnd);
	
	switch (_msg)
	{
	case WM_CREATE:
	{
		// Do initialization stuff here.
		s_pCanvas = new CCanvas();
		RECT rect;
		GetClientRect(_hwnd, &rect);
		s_pCanvas->Initialise(_hwnd, rect.right - rect.left, rect.bottom - rect.top);

		// Return Success.
		return (0);
		break;
	}
	case WM_PAINT:
	{
		s_pCanvas->Draw(_hwnd);

		// Return Success.
		return (0);
		break;
	}
	case WM_MOUSEMOVE:
	{
		s_iCurMouseX = static_cast<int>(LOWORD(_lparam));
		s_iCurMouseY = static_cast<int>(HIWORD(_lparam));

		if (s_pCurShape != nullptr && s_eCurShapeTool != POLYGONSHAPE)
		{
			// Add points to brush stroke instead if we are still making a paint brush shape
			CPaintBrush* pPaintBrush = dynamic_cast<CPaintBrush*>(s_pCurShape);
			if (s_eCurShapeTool == BRUSHSHAPE && pPaintBrush)
			{
				pPaintBrush->AddPoint({ s_iCurMouseX, s_iCurMouseY });
			}
			else if (s_eCurShapeTool == STAMP)
			{
				s_pCurShape->SetStartX(s_iCurMouseX);
				s_pCurShape->SetStartY(s_iCurMouseY);
			}
			else
			{
				s_pCurShape->SetEndX(s_iCurMouseX);
				s_pCurShape->SetEndY(s_iCurMouseY);
			}

			// Trigger redraw
			InvalidateRect(_hwnd, NULL, FALSE);
		}

		return 0;
		break;
	}
	case WM_RBUTTONDOWN:
	{
		// Stop drawing current polygon on right mouse click
		if (s_eCurShapeTool == POLYGONSHAPE || s_eCurShapeTool == BRUSHSHAPE)
		{
			s_pCurShape = nullptr;
		}
	}
	case WM_LBUTTONDOWN:
	{
		// Add points to polygon instead if we are still making a polygon
		CPolygon* pPolygon = dynamic_cast<CPolygon*>(s_pCurShape);
		if (s_eCurShapeTool == POLYGONSHAPE && pPolygon)
		{
			pPolygon->AddPoint({ s_iCurMouseX, s_iCurMouseY });
		}
		else
		{
			// Create new shape
			switch (s_eCurShapeTool)
			{
			case FREEHAND:
				break;
			case LINESHAPE:
				s_pCurShape = new CLine(s_iCurPenStyle, s_iCurPenWidth, s_colCurPen, s_iCurMouseX, s_iCurMouseY);
				break;
			case RECTANGLESHAPE:
				s_pCurShape = new CRectangle(s_eCurBrushStyle, s_colCurBrush, s_iCurPenStyle, s_iCurPenWidth, s_colCurPen, s_iCurMouseX, s_iCurMouseY);
				break;
			case ELLIPSESHAPE:
				s_pCurShape = new CEllipse(s_eCurBrushStyle, s_colCurBrush, s_iCurPenStyle, s_iCurPenWidth, s_colCurPen, s_iCurMouseX, s_iCurMouseY);
				break;
			case POLYGONSHAPE:
				s_pCurShape = new CPolygon(s_eCurBrushStyle, s_colCurBrush, s_iCurPenStyle, s_iCurPenWidth, s_colCurPen, s_iCurMouseX, s_iCurMouseY);
				break;
			case BRUSHSHAPE:
				s_pCurShape = new CPaintBrush(s_iCurPenStyle, s_iCurPenWidth, s_colCurPen, s_iCurMouseX, s_iCurMouseY);
				break;
			case STAMP:
				s_pCurShape = new CStamp(g_hInstance, _hwnd, s_strStampFilename, s_iCurMouseX, s_iCurMouseY);
				break;
			default:
				break;
			}

			// Add new shape to canvas
			s_pCanvas->AddShape(s_pCurShape);
		}

		// Trigger redraw
		InvalidateRect(_hwnd, NULL, FALSE);

		return 0;
		break;
	}
	case WM_KEYDOWN:
	{
	case 0x5A: // Z Key
	{
		if (s_pCurShape)
		{
			s_pCurShape = nullptr;
		}

		s_pCanvas->PopShape();

		InvalidateRect(_hwnd, NULL, FALSE);

		break;
	}
	}
	case WM_LBUTTONUP:
	{
		if (s_eCurShapeTool != POLYGONSHAPE)
		{
			s_pCurShape = nullptr;
		}

		return 0;
		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(_wparam))
		{
		case ID_FILE_SAVE:
		{
			s_pCanvas->Save(_hwnd);

			break;
		}
		case ID_FILE_OPEN:
		{
			CCanvas* pNewCanvas = CCanvas::Open(g_hInstance, _hwnd);
			if (pNewCanvas)
			{
				delete s_pCanvas;
				s_pCanvas = pNewCanvas;

				InvalidateRect(_hwnd, NULL, FALSE);
			}

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
		case ID_SHAPE_PAINTBRUSH:
		{
			s_eCurShapeTool = BRUSHSHAPE;
			CheckShapeMenuItem(hMenu, ID_SHAPE_PAINTBRUSH);
			break;
		}
		case ID_STAMP_FROMFILE:
		{
			s_eCurShapeTool = STAMP;

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
				s_strStampFilename = arrcFilename;
			}

			CheckShapeMenuItem(hMenu, ID_STAMP_FROMFILE);

			break;
		}
		case ID_STAMP_DEFAULT:
		{
			s_eCurShapeTool = STAMP;
			s_strStampFilename = "";

			CheckShapeMenuItem(hMenu, ID_STAMP_DEFAULT);

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
		case ID_PEN_COLOR:
		{
			ColorChooser(_hwnd, s_colCurPen);
			break;
		}
		case ID_BRUSH_COLOR:
		{
			ColorChooser(_hwnd, s_colCurBrush);
			break;
		}
		case ID_STYLE_TRANSPARENT:
		{
			CheckBrushStyleMenuItem(hMenu, ID_STYLE_TRANSPARENT);
			s_eCurBrushStyle = NOSTYLE;
			break;
		}
		case ID_STYLE_FILL:
		{
			CheckBrushStyleMenuItem(hMenu, ID_STYLE_FILL);
			s_eCurBrushStyle = SOLID;
			break;
		}
		case ID_STYLE_HATCH:
		{
			CheckBrushStyleMenuItem(hMenu, ID_STYLE_HATCH);
			s_eCurBrushStyle = HATCH;
			break;
		}
		case ID_STYLE_SOLID:
		{
			if (s_iSavedPenWidth > 0)
			{
				EnablePenWidthMenu(hMenu);
				s_iCurPenWidth = s_iSavedPenWidth;
				s_iSavedPenWidth = -1;
			}

			CheckPenStyleMenuItem(hMenu, ID_STYLE_SOLID);
			s_iCurPenStyle = PS_SOLID;
			break;
		}
		case ID_STYLE_DOT:
		{
			if (s_iSavedPenWidth < 0)
			{
				s_iSavedPenWidth = s_iCurPenWidth;
			}
			s_iCurPenWidth = 1;
			DisablePenWidthMenu(hMenu);

			CheckPenStyleMenuItem(hMenu, ID_STYLE_DOT);
			s_iCurPenStyle = PS_DOT;
			break;
		}
		case ID_STYLE_DASH:
		{
			if (s_iSavedPenWidth < 0)
			{
				s_iSavedPenWidth = s_iCurPenWidth;
			}
			s_iCurPenWidth = 1;
			DisablePenWidthMenu(hMenu);

			CheckPenStyleMenuItem(hMenu, ID_STYLE_DASH);
			s_iCurPenStyle = PS_DASH;
			break;
		}
		case ID_STYLE_DASHDOT:
		{
			if (s_iSavedPenWidth < 0)
			{
				s_iSavedPenWidth = s_iCurPenWidth;
			}
			s_iCurPenWidth = 1;
			DisablePenWidthMenu(hMenu);

			CheckPenStyleMenuItem(hMenu, ID_STYLE_DASHDOT);
			s_iCurPenStyle = PS_DASHDOT;
			break;
		}
		case ID_STYLE_DASHDOTDOT:
		{
			if (s_iSavedPenWidth < 0)
			{
				s_iSavedPenWidth = s_iCurPenWidth;
			}
			s_iCurPenWidth = 1;
			DisablePenWidthMenu(hMenu);

			CheckPenStyleMenuItem(hMenu, ID_STYLE_DASHDOTDOT);
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
