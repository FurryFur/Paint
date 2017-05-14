#ifndef __CANVAS_H__
#define __CANVAS_H__

//	Library Includes
#define NOMINMAX
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
	bool Draw(HWND _hwnd);
	// Save Canvas to bitmap
	void Save();
	void AddShape(IShape*);
	int GetWidth() const;
	int GetHeight() const;
	
private:
	CBackBuffer* m_pBackBuffer; // A canvas has a backbuffer.
	std::vector<IShape*> m_vecShapes;
	bool m_bNeedsResize;
	const IShape* m_pkShapeThatTriggeredResize;
	void CheckNeedsResize(const IShape* _pkShape);
	bool Resize(HWND _hwnd, int _iWidth, int _iHeight);
};

#endif