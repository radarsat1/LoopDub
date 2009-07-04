
#ifndef _SCROBUI_TOOLKIT_H_
#define _SCROBUI_TOOLKIT_H_

#include <stdlib.h>
#include <limits.h>
#include "../platform.h"

#undef TextOut

/*
** Toolkit for SDL Screen Object User Interface
*/

#define MAX_FB_NAMES 50

class Box : public Scrob
{
public:
	Box() : Scrob() {}
	Box(Scrob* pParent, const Rect& r, int color=3, int bkcolor=-1);
	bool Create(Scrob *pParent, const Rect& r, int color=3, int bkcolor=-1);
	virtual void Draw();

protected:
	int m_nColor;
	int m_nBkColor;
};

//! Base class for objects which hold textual information.
class TextInput
{
  public:
	TextInput(Scrob& owner) : m_Scrob(owner)
		{ m_strText = NULL; m_nAllocated = 0; }
	~TextInput() { if (m_strText) delete[] m_strText; }
	virtual void SetText(const char *strText);
	virtual char* GetText() { return m_strText; }
	virtual void SetInteger(int value);
	virtual int GetInteger() { return atoi(m_strText); }

  protected:
	Scrob &m_Scrob;
	char *m_strText;
	int m_nAllocated;
};

//! Base class for objects which hold integer values.
class IntegerInput
{
  public:
	IntegerInput(Scrob& owner) : m_Scrob(owner) {};
	virtual void SetValue(int value) { m_nValue = value; }
	virtual int GetValue() { return m_nValue; }

  protected:
	Scrob &m_Scrob;
	int m_nValue;
};

//! Base class for objects which hold an integer value and have a range.
class IntegerInputRange : public IntegerInput
{
  public:
	IntegerInputRange(Scrob& owner) : IntegerInput(owner) {};
	virtual void SetValueRange(int min, int max)
		{ m_nMin = min; m_nMax = max; }
	virtual int GetValueMin() { return m_nMin; }
	virtual int GetValueMax() { return m_nMax; }

  protected:
	int m_nMin, m_nMax;
};

class Handle : public Scrob
{
public:
	Handle();
	Handle(Scrob* pParent, const Rect& r);
	virtual void Draw();
	void OnMouseUp(Point mouse);
	void OnMouseDown(Point mouse);
	void OnMouseMove(Point mouse);
	void SetLimits(bool bVertical, bool bHorizontal,
				   int minx, int miny, int maxx, int maxy);

protected:
	bool m_bGotMouse;
	Point m_ptGrab;

	// movement limits
	bool m_bVertical;
	bool m_bHorizontal;
	int m_nMinX, m_nMaxX;
	int m_nMinY, m_nMaxY;
};

class Slider : public Scrob, public IntegerInputRange
{
public:
	Slider();
	Slider(Scrob* pParent, const Rect& r, bool bVertical);
	bool Create(Scrob *pParent, const Rect& r, bool bVertical);
	void Draw();
	void OnMouseUp(Point mouse);
	void OnMouseDown(Point mouse);

	int GetValue();
	void SetValue(int nValue);

	void SetColor(int color);

protected:
	Handle *m_pHandle;
	bool m_bVertical;
	int m_nColor;
};

class Label : public Scrob, public TextInput
{
public:
	Label();
	Label(Scrob *pParent, const Rect& r, const char *strText, int color=3, int bkcolor=0);
	~Label();
	bool Create(Scrob *pParent, const Rect& r, const char *strText, int color=3, int bkcolor=0);
	void Draw();

	void SetColor(int color);
	void SetBkColor(int bkcolor);

protected:
	int m_nColor;
	int m_nBkColor;
};

class Image : public Scrob
{
  public:
	Image();
	Image(Scrob *pParent, const Rect& r, int width, int height, const char* data);
	bool Create(Scrob *pParent, const Rect& r, int width, int height, const char* data);
	
	void Draw();

  protected:
	int m_nWidth;
	int m_nHeight;
	const char* m_pData;
};

class Button : public Label, public IntegerInput
{
public:
	Button();
	Button(Scrob *pParent, const Rect& r, const char *strText, int color, int bkcolor=0,
		   int command=0, void* command_value=NULL, bool bToggle=false);
	~Button();
	bool Create(Scrob *pParent, const Rect& r, const char *strText, int color, int bkcolor=0,
				int command=0, void* command_value=NULL, bool bToggle=false);
	void Draw();

	void OnMouseUp(Point mouse);
	void OnMouseDown(Point mouse);
	void OnMouseMove(Point mouse);

	bool IsPressed() { return m_nValue!=0; }
	void SetPressed(bool bPressed);

protected:
	bool m_bToggle;
	bool m_bGotMouse;
	int m_nCommand;
	void *m_nCommandValue;
};

class Field : public Scrob, public TextInput
{
public:
	Field();
	Field(Scrob *pParent, const Rect& r, const char *strText, int nMaxChars, int color, int bkcolor=0);
	~Field();
	bool Create(Scrob *pParent, const Rect& r, const char *strText, int nMaxChars, int color, int bkcolor=0);
	void Draw();

	void SetColor(int color);
	void SetBkColor(int bkcolor);

	void OnKeyDown(SDL_keysym* pKeySym);

protected:
	char *m_strText;
	int m_nMaxChars;
	int m_nColor;
	int m_nBkColor;
	int m_nCursorPos;
	int m_nTextPos;
	int m_nViewPos;
	bool m_bFocus;
};

class FileBrowser : public Scrob
{
public:
	FileBrowser();
	FileBrowser(Scrob *pParent, const Rect& r, const char *strDir=NULL, int filecommand=-1, int dircommand=-1, bool showDir=true);
	~FileBrowser();
	bool Create(Scrob *pParent, const Rect& r, const char *strDir=NULL, int filecommand=-1, int dircommand=-1, bool showDir=true);

	void Draw();
	void OnMouseUp(Point mouse);

	void SetDirectory(const char *strDir);
	void SetDirectoryFromBase(const char *strDir);
	const char* GetDirectory() { return m_strDir; }

	// extension for filtering listed entries
	void SetExtension(const char *strExt);
	const char* GetExtension() { return m_strExt; }

	// set a base directory, bottom-level directory that user
    // cannot browse out of.
	void SetBase(const char *strBase);
	const char* GetBase() { return m_bBase ? m_strBase : 0; };

protected:
	int m_nColumns;
	int m_nRows;
	int m_nItemWidth;
	int m_nFileCommand;
	int m_nDirCommand;
	bool m_bShowDir;
	bool m_bMore, m_bLess;
	int m_nDrawFileOffset;

	char m_strDir[MAX_PATH];
	char m_strFile[MAX_PATH];
	char m_strExt[15];
	bool m_bExt;
	char m_strBase[MAX_PATH];
	bool m_bBase;
	bool m_isdir[MAX_FB_NAMES];
	char m_names[MAX_FB_NAMES][256];
	int m_nNames;
	bool m_bLoading;

	Box *m_pBox;
	Field *m_pFolder;

	static THREADFUNC setDirectoryThread(void* fileBrowser);
};

#endif // _SCROBUI_TOOLKIT_H_
