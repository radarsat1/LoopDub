#ifndef _SCROBUI_H_
#define _SCROBUI_H_

#include <SDL.h>

#ifdef TextOut
#undef TextOut
#endif

//namespace ScrobUI {

/*
** SDL Screen Object User Interface
*/

// Points are x-y vectors into the screen
class Point
{
public:
	Point(int _x=0, int _y=0)
		{ x = _x; y = _y; }

	Point operator+(Point rhs) const
		{ return Point(x+rhs.x, y+rhs.y); }

	Point operator-(Point rhs) const
		{ return Point(x-rhs.x, y-rhs.y); }

	Point operator-() const
		{ return Point(-x, -y); }

	Point operator*(int n) const
		{ return Point(x*n, y*n); }

	void operator+=(Point rhs)
		{ x+=rhs.x; y+=rhs.y; }

	void operator-=(Point rhs)
		{ x-=rhs.x; y-=rhs.y; }

	void operator*=(int n)
		{ x*=n; y*=n; }

	Point operator=(Point rhs)
		{ x=rhs.x; y=rhs.y; return *this; }

	void Set(int _x, int _y)
		{ x=_x; y=_y; }

	void Print() const
		{ printf("(%d,%d)", x, y); }

public:
	short x, y;
};

// Rectangles are simply rectangular areas of the screen
class Rect
{
public:
	Rect(int _x1=0, int _y1=0, int _x2=0, int _y2=0)
		{ x1 = _x1; y1 = _y1; x2 = _x2; y2 = _y2; }

	void Offset(Point p)
		{ x1 += p.x; y1 += p.y; x2 += p.x; y2 += p.y; }

	void Intersect(const Rect& r)
		{ x1 = (x1>r.x1)?x1:r.x1; y1 = (y1>r.y1)?y1:r.y1; x2 = (x2<r.x2)?x2:r.x2; y2 = (y2<r.y2)?y2:r.y2; }

	void Union(const Rect& r)
		{
			if (IsValid()) {
				if (r.x1 < x1) x1 = r.x1;
				if (r.y1 < y1) y1 = r.y1;
				if (r.x2 > x2) x2 = r.x2;
				if (r.y2 > y2) y2 = r.y2;
			}
			else
				Set(r.x1, r.y1, r.x2, r.y2);
		}

	bool PointInRect(Point p) const
		{ return p.x >= x1 && p.x <= x2 && p.y >= y1 && p.y <= y2; }

	void Set(int _x1, int _y1, int _x2, int _y2)
		{ x1=_x1; y1=_y1; x2=_x2; y2=_y2; }

	bool IsValid() const
		{ return (x1 <= x2) && (y1 <= y2); }

	Rect& operator=(const Rect& rhs)
		{ x1=rhs.x1; y1=rhs.y1; x2=rhs.x2; y2=rhs.y2; return *this; }

	int Width() const { return x2-x1; }
	int Height() const { return y2-y1; }

	Point TopLeft() const
		{ return Point(x1, y1); }

	Point BottomRight() const
		{ return Point(x2, y2); }

	Point TopRight() const
		{ return Point(x2, y1); }

	Point BottomLeft() const
		{ return Point(x1, y2); }

	void Print() const
		{ printf("(%d,%d,%d,%d)", x1, y1, x2, y2); }

	operator SDL_Rect() const
		{ SDL_Rect r; r.x=x1; r.y=y1; r.w=Width(); r.h=Height(); return r; }

public:
	short x1, y1, x2, y2;
	SDL_Rect sdlr;
};

// Screen objects represent rectangular areas of the screen.
// They can display themselves and react to messages.
// They may contain sub-objects.
class Scrob
{
public:
	Scrob();
	virtual ~Scrob();
	Scrob(Scrob *pParent, const Rect& r);

	Scrob *GetFirstChild();
	Scrob *GetNextSibling() { return m_pNextSibling; }
	Scrob *GetPrevSibling() { return m_pPrevSibling; }
	Scrob *GetParent() { return m_pParent; }
	Scrob *GetHitChild(Point p);
	Rect& GetRect() { return m_Rect; }
	bool IsDirty() { return m_bDirty; }
	void SetDirty();
	bool IsVisible() { return m_bVisible; }
	void SetVisible(bool visible);
	void SetParentDirty(bool bSetParentDirty) { m_bSetParentDirty = bSetParentDirty; }
	void DrawFamily();	// draw self and children if dirty
	Point ScreenToScrob(Point p);
	Point ScrobToScreen(Point p);

	virtual bool Create(Scrob *pParent, const Rect& r);
	virtual bool AddChild(Scrob* pScrob);
	virtual void SetRect(const Rect& r) { m_Rect = r; SetDirty(); }
	virtual void Draw() {}
	virtual void OnMouseUp(Point mouse) { if (m_pParent) m_pParent->OnMouseUp(mouse); }
	virtual void OnMouseDown(Point mouse);
	virtual void OnMouseMove(Point mouse)  { if (m_pParent) m_pParent->OnMouseMove(mouse); }
	virtual void OnKeyUp(SDL_keysym* pKeySym) {}
	virtual void OnKeyDown(SDL_keysym* pKeySym) {}

protected:
	Rect m_Rect;

	Scrob *m_pChildList;
	Scrob *m_pNextSibling;
	Scrob *m_pPrevSibling;
	Scrob *m_pLastChild;
	Scrob *m_pParent;

	bool m_bSetParentDirty;
	bool m_bDirty;
	bool m_bVisible;
};

// GUI object is the master control of the GUI.
// Contains the principle list of scrobs, starting with an
// initial scrob covering the whole screen/window
class GUI
{
public:
	GUI();
	~GUI();

	bool Initialize(const char *strTitle=NULL, bool bFullScreen=false,
		int nWidth=800, int nHeight=600, unsigned int extraSDLflags=0);
	void Cleanup();
	Scrob* GetScrobList() { return m_pScrobList; }

	void WaitEvent();
	bool CheckEvent();
	bool ProcessEvent(); // return true when time to quit
	void UpdateScreen();
	bool SetFullScreen(bool bFullScreen);
	bool GetFullScreen() { return m_bFullScreen; }
	SDL_Event *GetEvent() { return &m_Event; }
	void SetPalette();

	// Return true when there is a command waiting to be processed
	bool GetCommand(int* command, void** value=NULL);

public:
	void CaptureMouse(Scrob *pScrob) { m_pScrobGotMouse = pScrob; }
	void ReleaseMouse() { m_pScrobGotMouse = NULL; }
	void SetFocus(Scrob *pScrob) { if (m_pScrobFocus) m_pScrobFocus->SetDirty(); m_pScrobFocus = pScrob; }
	Scrob* GetFocus() { return m_pScrobFocus; }
	void SetCommand(int command, void* value) { m_nCommand=command; m_nCommandValue=value; }
	void SetRectDirty(Rect &r) { m_rDirty.Union(r); }

protected:
	Scrob *m_pScrobList;
	SDL_Surface *m_pSurface;
	SDL_Event m_Event;
	Scrob *m_pScrobGotMouse;
	Scrob *m_pScrobFocus;
	bool m_bFullScreen;
	int m_nWidth, m_nHeight;
	int m_nCommand;
	void* m_nCommandValue;
	Rect m_rDirty;
};

// There is one and only one master GUI object.
#ifndef _SCROBUI_CPP_
extern
#endif
GUI gui;

// Drawing tools allow us to visually represent the GUI
class DrawingTools
{
public:
	DrawingTools();
	~DrawingTools();
	bool LoadCharacterData();

	Scrob* SetCurrentObject(Scrob* pScrob);
	Scrob* GetCurrentObject() { return m_pCurrentObject; }
	void SetPosition(Point p);
	void DrawRect(const Rect& r, int color);
	void DrawEdge(const Rect& r, bool bBevel, bool bFlat=true);
	void FillRect(Rect r, int color);
	void TextOut(Point pt, const char *str, int color);
	void SetClipRect(Rect pr);
	void SetClipRect();
	void PutPixel(Point pt, int color);
	int  GetPixel(Point pt);

	void SetSurface(SDL_Surface *pSurface) { m_pSurface = pSurface; }

	int GetFontHeight() { return m_nFontHeight; }
	int GetTextWidth(const char *str, int nChars);

protected:
	Scrob *m_pCurrentObject;
	Point m_ptPosition;
	Point m_ptOffset;
	Rect m_rClip;
	bool m_bClip;
	SDL_Surface *m_pSurface;
	unsigned char* m_pCharacterData;
	SDL_Surface *m_pCharacterSurface;
	unsigned char* m_nCharWidths;
	unsigned short* m_nCharOffsets;
	unsigned int m_nFontHeight;
};

// There is one and only one master DrawingTools object.
#ifndef _SCROBUI_CPP_
extern
#endif
DrawingTools dt;

#include "toolkit.h"

//}; // namespace ScrobUI

#endif // _SCROBUI_H_
