
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
	HDC hdc;        // Handle to a device context.
	static ESHAPE s_eCurShapeTool = LINESHAPE;
	static int s_iCurPenWidth = 5;
	static EBRUSHSTYLE s_eCurBrushStyle = HATCH;
	static int s_iCurPenStyle = PS_SOLID;
	static CCanvas* s_pCanvas;
	static IShape* s_pCurShape = nullptr;
	static int s_iCurMouseX;
	static int s_iCurMouseY;
	static COLORREF s_colCurPen = RGB(40, 128, 255);
	static COLORREF s_colCurBrush = RGB(0, 255, 0);

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

			// Trigger redraw
			InvalidateRect(_hwnd, NULL, FALSE);
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
			DisablePenWidthMenu(hMenu);
		}
		else
		{
			EnablePenWidthMenu(hMenu);
		}

		// Create new shape
		switch (s_eCurShapeTool)
		{
		case FREEHAND:
			break;
		case LINESHAPE:
			// TODO: Pen width should be set to 1 for all non-solid 
			s_pCurShape = new CLine(s_iCurPenStyle, s_iCurPenWidth, s_colCurPen, s_iCurMouseX, s_iCurMouseY);
			break;
		case RECTANGLESHAPE:
			s_pCurShape = new CRectangle(s_eCurBrushStyle, 0, s_colCurBrush, s_iCurPenStyle, s_iCurPenWidth, s_colCurPen, s_iCurMouseX, s_iCurMouseY);
			break;
		case ELLIPSESHAPE:
			s_pCurShape = new CEllipse(s_eCurBrushStyle, 0, s_colCurBrush, s_iCurPenStyle, s_iCurPenWidth, s_colCurPen, s_iCurMouseX, s_iCurMouseY);
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

		// Add new shape to canvas
		s_pCanvas->AddShape(s_pCurShape);

		// Trigger redraw
		InvalidateRect(_hwnd, NULL, FALSE);

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
			s_pCanvas->Save();

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
