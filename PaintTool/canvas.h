#ifndef __CANVAS_H__
#define __CANVAS_H__

//	Library Includes
#include <windows.h>
#include <windowsx.h>
#include <vector>

typedef enum
{
	LINE = 0,
	BOX,
	ELLIPSE,
	MAX_SHAPE
}EShape;

class IShape; //Forward Declaration
class CLine;
class CStamp;
class CBackBuffer;

class CCanvas
{
public:
	CCanvas();
	~CCanvas();
	bool Initialise(HWND _hwnd, int _iWidth, int _iHeight);
	CBackBuffer* GetBackBuffer();
	bool Draw(HDC _hdc);
	// Save Canvas to bitmap
	void Save(HWND _hwnd);
	void AddShape(IShape*);
	void AddStamp(CStamp*);
	
private:
	CBackBuffer* m_pBackBuffer; // A canvas has a backbuffer.
	std::vector<CStamp*> m_vecStamps;
	std::vector<IShape*> m_vecShapes;
};

#endif