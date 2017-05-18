#ifndef __CANVAS_H__
#define __CANVAS_H__

//	Library Includes
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <vector>

#include "IUpdateListener.h"

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

class CCanvas : public IUpdateListener
{
public:
	CCanvas();
	virtual ~CCanvas() override;
	bool Initialise(HWND _hwnd, int _iWidth, int _iHeight);
	CBackBuffer* GetBackBuffer();
	bool Draw(HWND _hwnd);
	bool DrawBackground();
	// Save Canvas to bitmap
	void Save(HWND _hwnd);
	void AddShape(IShape*);
	void PopShape();
	int GetWidth() const;
	int GetHeight() const;
	void SetBackground(HBITMAP _hBitmapBackground);

	static CCanvas* Open(HINSTANCE _hInstance, HWND _hwnd);

	// Notify the canvas that the specified shape has been updated (moved)
	virtual void NotifyUpdated(const IShape* _pkShape) override;
	
private:
	CBackBuffer* m_pBackBuffer; // A canvas has a backbuffer.
	std::vector<IShape*> m_vecpShapes;
	bool m_bNeedsResize;
	const IShape* m_pkShapeThatTriggeredResize;
	bool Resize(HWND _hwnd, int _iWidth, int _iHeight);
	HBITMAP m_hbBackground;
};

#endif