
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>   // Include all the windows headers.
#include <windowsx.h>  // Include useful macros.

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
	static CCanvas* s_pCanvas;
	static IShape* s_pCurShape = nullptr;
	static int s_iCurMouseX;
	static int s_iCurMouseY;
	
	switch (_msg)
	{
	case WM_CREATE:
		// Do initialization stuff here.
		s_pCanvas = new CCanvas();

		

		// Return Success.
		return (0);
		break;
	case WM_PAINT:
		hdc = BeginPaint(_hwnd, &ps);
		// You would do all your painting here...

		s_pCanvas->Draw(hdc);
		
		EndPaint(_hwnd, &ps);

		// Return Success.
		return (0);
		break;
	case WM_MOUSEMOVE:
		s_iCurMouseX = static_cast<int>(LOWORD(_lparam));
		s_iCurMouseY = static_cast<int>(HIWORD(_lparam));

		if (s_pCurShape != nullptr)
		{
			s_pCurShape->SetEndX(s_iCurMouseX);
			s_pCurShape->SetEndY(s_iCurMouseY);
			
			InvalidateRect(_hwnd, NULL, TRUE);
		}

		return 0;
		break;
	case WM_LBUTTONDOWN:
		switch (s_eCurShapeTool)
		{
		case FREEHAND:
			break;
		case LINESHAPE:
			s_pCurShape = new CLine(PS_SOLID, s_iCurPenWidth, RGB(0, 0, 0), s_iCurMouseX, s_iCurMouseY);
			break;
		case RECTANGLESHAPE:
			s_pCurShape = new CRectangle(EBRUSHSTYLE::HATCH, 0, RGB(0, 255, 0), PS_SOLID, s_iCurPenWidth, RGB(0, 0, 0), s_iCurMouseX, s_iCurMouseY);
			break;
		case ELLIPSESHAPE:
			break;
		case POLYGONSHAPE:
			break;
		case STAMP:
			break;
		default:
			break;
		}
		
		s_pCanvas->AddShape(s_pCurShape);

		return 0;
		break;
	case WM_LBUTTONUP:
		s_pCurShape = nullptr;
		
		return 0;
		break;
	case WM_COMMAND:
		switch (LOWORD(_wparam))
		{
		case ID_FILE_EXIT:
		{
			PostQuitMessage(0);
			break;
		}
		case ID_SHAPE_LINE:
		{
			s_eCurShapeTool = LINESHAPE;
			break;
		}
		case ID_WIDTH_SMALL:
		{
			s_iCurPenWidth = 1;
			break;
		}
		case ID_WIDTH_MEDIUM:
		{
			s_iCurPenWidth = 5;
			break;
		}
		case ID_WIDTH_LARGE:
		{
			s_iCurPenWidth = 10;
			break;
		}
		case ID_SHAPE_R:
		{
			s_eCurShapeTool = RECTANGLESHAPE;
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
	case WM_DESTROY:
		// Kill the application, this sends a WM_QUIT message.
		PostQuitMessage(0);

		// Return success.
		return (0);
		break;

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
