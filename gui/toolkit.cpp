
/*
** Toolkit for SDL Screen Object User Interface
*/

#include "scrobui.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef WIN32
  #include <io.h>
  #define snprintf _snprintf
  #ifndef CYGWIN
    #define realpath(srcpath, destpath) fullpath(destpath, srcpath, MAX_PATH)
  #endif
  #define DIR_SEPARATOR "\\"
#else
  #include <dirent.h>
  #include <sys/stat.h>
  #define DIR_SEPARATOR "/"
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) >= (b)) ? (a) : (b))
#endif


//using namespace ScrobUI;

/******* Box ********/

Box::Box(Scrob* pParent, const Rect& r, int color, int bkcolor)
	 : Scrob()
{
	 m_nColor = color;
	 m_nBkColor = bkcolor;
	 Create(pParent, r, color, bkcolor);
}

bool Box::Create(Scrob *pParent, const Rect& r, int color, int bkcolor)
{
	 m_nColor = color;
	 m_nBkColor = bkcolor;
	 return Scrob::Create(pParent, r);
}

void Box::Draw()
{
	dt.SetCurrentObject(this);
	Rect r(0,0,m_Rect.Width(),m_Rect.Height());
	if (m_nBkColor != -1)  dt.FillRect(r, m_nBkColor);
	if (m_nColor != -1)    dt.DrawRect(r, m_nColor);
}

/******* Handle ********/

Handle::Handle() : Scrob()
{
	m_bGotMouse = false;
	m_bVertical = true;
	m_bHorizontal = true;
	m_nMinX = m_nMaxX = m_nMinY = m_nMaxY = -1;
}

Handle::Handle(Scrob *pParent, const Rect& r) : Scrob(pParent, r)
{
	m_bGotMouse = false;
	m_bVertical = true;
	m_bHorizontal = true;
	m_nMinX = m_nMaxX = m_nMinY = m_nMaxY = -1;
}

void Handle::Draw()
{
	dt.SetCurrentObject(this);
	Rect r(0,0,m_Rect.Width(),m_Rect.Height());
	dt.FillRect(r,2);
	dt.DrawEdge(r,!m_bGotMouse);
}

void Handle::OnMouseDown(Point mouse)
{
	gui.CaptureMouse(this);
	m_bGotMouse = true;
	m_ptGrab = mouse;
	SetDirty();
}

void Handle::OnMouseUp(Point mouse)
{
	gui.ReleaseMouse();
	m_bGotMouse = false;
	SetDirty();
}

void Handle::OnMouseMove(Point mouse)
{
	if (m_bGotMouse)
	{
		Rect r(m_Rect);
		r.Offset(mouse-m_ptGrab);
		if (m_nMinX!=-1 && m_nMaxX!=-1) {
			if (r.x1 < m_nMinX) {
				r.x2 += m_nMinX - r.x1;
				r.x1 = m_nMinX;
			}
			if (r.x2 > m_nMaxX) {
				r.x1 -= r.x2 - m_nMaxX;
				r.x2 = m_nMaxX;
			}
		}
		if (m_nMinY!=-1 && m_nMaxY!=-1) {
			if (r.y1 < m_nMinY) {
				r.y2 += m_nMinY - r.y1;
				r.y1 = m_nMinY;
			}
			if (r.y2 > m_nMaxY) {
				r.y1 -= r.y2 - m_nMaxY;
				r.y2 = m_nMaxY;
			}
		}
		if (m_bHorizontal) {
			m_Rect.x1 = r.x1;
			m_Rect.x2 = r.x2;
		}
		if (m_bVertical) {
			m_Rect.y1 = r.y1;
			m_Rect.y2 = r.y2;
		}
		SetDirty();
		m_pParent->SetDirty();
	}
}

void Handle::SetLimits(bool bVertical, bool bHorizontal,
					   int minx, int miny, int maxx, int maxy)
{
	m_bVertical = bVertical;
	m_bHorizontal = bHorizontal;
	m_nMinX = minx;
	m_nMaxX = maxx;
	m_nMinY = miny;
	m_nMaxY = maxy;
}

/******* Slider ********/

Slider::Slider() : Scrob()
{
}

Slider::Slider(Scrob *pParent, const Rect& r, bool bVertical) : Scrob()
{
	Create(pParent, r, bVertical);
}

bool Slider::Create(Scrob *pParent, const Rect& r, bool bVertical)
{
	Scrob::Create(pParent, r);
	int handleSize = bVertical ? r.Width() : r.Height();

	m_pHandle = new Handle;
	Rect r2(0, 0, handleSize, handleSize);
    if (!m_pHandle->Create(this, r2))
		return false;

	m_pHandle->SetLimits(bVertical, !bVertical, 0, 0, r.Width(), r.Height());
	AddChild(m_pHandle);

	m_bVertical = bVertical;
	m_nColor = 1;

	return true;
}

void Slider::Draw()
{
	dt.SetCurrentObject(this);
	dt.FillRect(Rect(0,0,m_Rect.Width(),m_Rect.Height()), m_nColor);
}

void Slider::OnMouseDown(Point mouse)
{
}

void Slider::OnMouseUp(Point mouse)
{
}

int Slider::GetValue()
{
	if (m_bVertical)
		return m_Rect.Height()-m_pHandle->GetRect().Height()-m_pHandle->GetRect().y1;
	else
		return m_pHandle->GetRect().x1;
}

int Slider::GetMaxValue()
{
	if (m_bVertical)
		return m_Rect.Height()-m_pHandle->GetRect().Height();
	else
		return m_Rect.Width()-m_pHandle->GetRect().Width();
}

void Slider::SetValue(int nValue)
{
	int size;
	Rect &r = m_pHandle->GetRect();
	if (m_bVertical)
	{
		size = r.Height();
		r.y1 = max(0, m_Rect.Height()-size-nValue);
		r.y2 = r.y1 + size;
	}
	else
	{
		size = r.Width();
		r.x1 = min(nValue, m_Rect.Width()-size);
		r.x2 = r.x1 + size;
	}

	m_pHandle->SetRect(r);

	SetDirty();
}

void Slider::SetColor(int color)
{
	 m_nColor = color;
	 SetDirty();
}

/********* Label ********/

Label::Label() : Scrob()
{
	m_strText = NULL;
	m_nAllocated = 0;
	m_nColor = 0;
	m_nBkColor = 0;
}

Label::Label(Scrob *pParent, const Rect& r, const char *strText, int color, int bkcolor)
	: Scrob(pParent, r)
{
	m_strText = NULL;
	m_nAllocated = 0;
	m_nColor = 0;
	m_nBkColor = 0;
	Create(pParent, r, strText, color, bkcolor);
}

Label::~Label()
{
	if (m_strText)
		delete m_strText;
}

bool Label::Create(Scrob *pParent, const Rect& r, const char *strText, int color, int bkcolor)
{
	Scrob::Create(pParent, r);

	if (m_strText)
		delete m_strText;

	m_nColor = color;
	m_nBkColor = bkcolor;

	if (strText) {
		 m_nAllocated = strlen(strText);
		 m_strText = new char[m_nAllocated+1];
		 if (!m_strText)
			  return false;
		 strcpy(m_strText, strText);
	}
	else {
		 m_nAllocated=0;
		 m_strText = NULL;
	}

	return true;
}

void Label::Draw()
{
	dt.SetCurrentObject(this);
	if (m_nBkColor > -1)
		 dt.FillRect(Rect(0,0,m_Rect.Width(), m_Rect.Height()), m_nBkColor);
	if (m_strText)
		 dt.TextOut(Point(0,(m_Rect.Height()-dt.GetFontHeight())/2), m_strText, m_nColor);
}

void Label::SetText(const char *strText)
{
	int len = strlen(strText);
	if (len > m_nAllocated)
	{
		if (m_strText) delete m_strText;
		m_strText = new char[len+1];
		if (!m_strText)
			return;

		m_nAllocated = len;
	}

	strcpy(m_strText, strText);
	SetDirty();
}

void Label::SetColor(int color)
{
	m_nColor = color;
	SetDirty();
}

void Label::SetBkColor(int bkcolor)
{
	m_nBkColor = bkcolor;
	SetDirty();
}

void Label::SetInteger(int value)
{
	if (m_nAllocated < 10)
	{
		if (m_strText)
			delete m_strText;
		m_strText = new char[11];
		m_nAllocated = 10;
	}

	if (!m_strText)
		return;

	sprintf(m_strText, "%d", value);
	SetDirty();
}

/******** Image *********/

Image::Image()
{
	 m_nWidth = 0;
	 m_nHeight = 0;
	 m_pData = NULL;
}

Image::Image(Scrob *pParent, const Rect& r, int width, int height, const char *data)
{	 
	 m_nWidth = 0;
	 m_nHeight = 0;
	 m_pData = NULL;
	 Create(pParent, r, width, height, data);
}

bool Image::Create(Scrob *pParent, const Rect& r, int width, int height, const char *data)
{
	 Scrob::Create(pParent, r);

	 m_nWidth = width;
	 if (m_nWidth < 0) m_nWidth = 0;
	 m_nHeight = height;
	 if (m_nHeight < 0) m_nHeight = 0;
	 m_pData = data;
	 return true;
}

void Image::Draw()
{
	 if (!m_pData) return;

	 int x_off = (m_Rect.Width()-m_nWidth)/2;
	 int y_off = (m_Rect.Height()-m_nHeight)/2;

	 dt.SetCurrentObject(this);
	 dt.SetClipRect(Rect(0,0,m_Rect.Width(),m_Rect.Height()));

	 for (int y=0; y<m_nHeight; y++) {
		  for (int x=0; x<m_nWidth; x++)
			   if (m_pData[x+y*m_nWidth]!=0)
					dt.PutPixel(Point(x+x_off, y+y_off), m_pData[x+y*m_nWidth]);
	 }

	 dt.SetClipRect();
}


/******** Button *********/

Button::Button()
  : Label()
{
  m_bToggle = false;
  m_bPressed = false;
  m_bGotMouse = false;
  m_nCommand = 0;
  m_nCommandValue = NULL;
}

Button::Button(Scrob *pParent, const Rect& r, const char *strText, int color, int bkcolor,
			   int command, void* command_value, bool bToggle)
  : Label(pParent, r, strText, color, bkcolor)
{
  m_bToggle = bToggle;
  m_bPressed = false;
  m_bGotMouse = false;
  m_nCommand = command;
  m_nCommandValue = command_value;
}

Button::~Button()
{
}

bool Button::Create(Scrob *pParent, const Rect& r, const char *strText, int color, int bkcolor,
					int command, void* command_value, bool bToggle)
{
  Label::Create(pParent, r, strText, color, bkcolor);
  m_nCommand = command;
  m_nCommandValue = command_value;
  m_bToggle = bToggle;

  return true;
}

void Button::SetPressed(bool bPressed)
{
	 m_bPressed = bPressed;
	 SetDirty();
}

void Button::Draw()
{
  dt.SetCurrentObject(this);
  dt.FillRect(Rect(1,1,m_Rect.Width()-1, m_Rect.Height()-1), m_nBkColor + (m_bPressed ? 1 : 0));
  dt.DrawEdge(Rect(0,0,m_Rect.Width(), m_Rect.Height()), !m_bPressed);

  Point p((m_Rect.Width()-dt.GetTextWidth(m_strText,strlen(m_strText)))/2,
		  (m_Rect.Height()-dt.GetFontHeight())/2);
  p.x = max(p.x, 2);
  if (m_bPressed) {
	p.x++;
	p.y++;
  }
  dt.SetClipRect(Rect(3,2,m_Rect.Width()-2,m_Rect.Height()-3));
  dt.TextOut(p, m_strText, m_nColor);
  dt.SetClipRect();

  // If SetPresed() was called previously for a non-Toggle button,
  // send OnMouseUp() to handle unpressing the button.
  if (m_bPressed && !m_bToggle)
	   OnMouseUp(Point(0,0));
}

void Button::OnMouseUp(Point mouse)
{
  gui.ReleaseMouse();
  m_bGotMouse = false;

  if (m_bToggle || m_bPressed)
	gui.SetCommand(m_nCommand, m_nCommandValue);

  if (m_bToggle)
	m_bPressed = !m_bPressed;
  else
	m_bPressed = false;

  SetDirty();
}

void Button::OnMouseDown(Point mouse)
{
  gui.CaptureMouse(this);
  m_bGotMouse = true;

  if (!m_bToggle)
	m_bPressed = true;

  SetDirty();
}

void Button::OnMouseMove(Point mouse)
{
  bool b=m_bPressed;
  
  if (m_bGotMouse && !m_bToggle) {
	Rect r(m_Rect);
	r.Offset(-m_Rect.TopLeft());
	b = r.PointInRect(mouse);
  }

  if (b!=m_bPressed) {
	m_bPressed = b;
	SetDirty();
  }
}

/******** Field ********/

Field::Field() : Scrob()
{
	m_nMaxChars = 0;
	m_strText = NULL;
	m_nColor = 0;
	m_nBkColor = 0;
	m_nCursorPos = 0;
	m_nViewPos = 0;
}

Field::Field(Scrob *pParent, const Rect& r, const char *strText, int nMaxChars, int color, int bkcolor)
	: Scrob(pParent, r)
{
	m_nMaxChars = 0;
	m_strText = NULL;
	m_nColor = 0;
	m_nBkColor = 0;
	m_nCursorPos = 0;
	m_nViewPos = 0;
	Create(pParent, r, strText, nMaxChars, color, bkcolor);
}

Field::~Field()
{
	if (m_strText)
		delete m_strText;
}

bool Field::Create(Scrob *pParent, const Rect& r, const char *strText, int nMaxChars, int color, int bkcolor)
{
	Scrob::Create(pParent, r);
	m_nMaxChars = nMaxChars;
	m_nColor = color;
	m_nBkColor = bkcolor;
	m_strText = new char[nMaxChars];
	if (!m_strText)
		return false;

	if (strText)
		strncpy(m_strText, strText, m_nMaxChars);
	return true;
}

void Field::Draw()
{
	dt.SetCurrentObject(this);
	Rect r(0,0,m_Rect.Width(), m_Rect.Height());
	dt.FillRect(r, m_nBkColor);

	r.x2-=1;
	r.y2-=1;
	dt.SetClipRect(r);
	int space = dt.GetTextWidth(" ", 1);
	int pos = dt.GetTextWidth(m_strText, m_nCursorPos+1) + space;
	m_nViewPos = pos - m_Rect.Width();
	if (m_nViewPos < 0)
		m_nViewPos = 0;

	int y = (m_Rect.Height()-dt.GetFontHeight())/2;

	dt.TextOut(Point(-m_nViewPos,y), m_strText, m_nColor);

	if (gui.GetFocus()==this)
	{
		Rect r(0, dt.GetFontHeight()+y, 0, dt.GetFontHeight()+y+1);
		r.x1 = dt.GetTextWidth(m_strText, m_nCursorPos) - m_nViewPos;
		r.x2 = r.x1 + dt.GetTextWidth(&m_strText[m_nCursorPos], 1);
		if (r.x1 == r.x2)
			r.x2 = r.x1 + space;
		dt.FillRect(r, m_nColor);
	}
	dt.SetClipRect();
}

void Field::SetColor(int color)
{
	m_nColor = color;
	SetDirty();
}

void Field::SetBkColor(int bkcolor)
{
	m_nBkColor = bkcolor;
	SetDirty();
}

void Field::SetText(const char *strText)
{
	if (!m_strText)
		return;

	if (!strText)
		m_strText[0] = 0;
	else
		strncpy(m_strText, strText, m_nMaxChars);
	SetDirty();
}

void Field::SetInteger(int value)
{
	snprintf(m_strText, m_nMaxChars, "%d", value);
	SetDirty();
}

void Field::OnKeyDown(SDL_keysym* pKeySym)
{
	int i, len;
	SDLKey key = pKeySym->sym;
	switch (key)
	{
	case SDLK_LEFT:
		m_nCursorPos--;
		if (m_nCursorPos < 0)
			m_nCursorPos = 0;
		break;
	case SDLK_RIGHT:
		len = strlen(m_strText);
		m_nCursorPos++;
		if (m_nCursorPos > len)
			m_nCursorPos = len;
		SetDirty();
		break;
	case SDLK_BACKSPACE:
		if (m_nCursorPos > 0) {
			len = strlen(m_strText);
			for (i=m_nCursorPos-1; i<len; i++)
				m_strText[i] = m_strText[i+1];
			m_nCursorPos--;
		}
		break;
	case SDLK_DELETE:
		len = strlen(m_strText);
		if (m_nCursorPos < len) {
			for (i=m_nCursorPos; i<len; i++)
				m_strText[i] = m_strText[i+1];
		}
		break;
	case SDLK_HOME:
		m_nCursorPos = 0;
		break;
	case SDLK_END:
		m_nCursorPos = strlen(m_strText);
		break;
	default:
		char c = (char)pKeySym->unicode;
		if (isprint(c))
		{
			int len = strlen(m_strText);
			if (len < m_nMaxChars)
			{
				for (int i=len; i>m_nCursorPos; i--)
					m_strText[i] = m_strText[i-1];
				m_strText[len+1] = 0;
				m_strText[m_nCursorPos] = c;
				m_nCursorPos++;
			}
		}
	}

	SetDirty();
}

/******** FileBrowser *********/


FileBrowser::FileBrowser() : Scrob()
{
	m_nRows = 0;
	m_nColumns = 0;
	m_bShowDir = true;
	m_pBox = NULL;
	m_pFolder = NULL;
	m_nItemWidth = 0;
	m_nDirCommand = -1;
	m_nFileCommand = -1;
	m_bExt = false;
	m_bBase = false;
	m_nNames = 0;
	m_nDrawFileOffset = 0;
}

FileBrowser::FileBrowser(Scrob *pParent, const Rect& r, const char *strDir, int filecommand, int dircommand, bool showDir)
	: Scrob(pParent, r)
{
	m_nRows = 0;
	m_nColumns = 0;
	m_pBox = NULL;
	m_pFolder = NULL;
	m_nItemWidth = 0;
	m_bExt = false;
	m_bBase = false;
	m_nNames = 0;
	m_nDrawFileOffset = 0;
	Create(pParent, r, strDir, filecommand, dircommand, showDir);
}

FileBrowser::~FileBrowser()
{
}

bool FileBrowser::Create(Scrob *pParent, const Rect& r, const char *strDir, int filecommand, int dircommand, bool showDir)
{
	Scrob::Create(pParent, r);

	m_nFileCommand = filecommand;
	m_nDirCommand = dircommand;
	m_bShowDir = showDir;

	int fh=0;
	Rect r2;
	if (m_bShowDir) {
		 fh = dt.GetFontHeight()+3;
		 r2.Set(0,0,r.Width(), fh);
		 m_pFolder = new Field(this, r2, strDir, MAX_PATH, 3, 0);
		 if (!m_pFolder) return false;
		 AddChild(m_pFolder);
	}

	r2.Set(0,fh,r.Width(),r.Height()-fh);

	// Calculate number of rows and columns
	// Assume max 10 characters per name
	m_nItemWidth = dt.GetTextWidth("O",1)*11;
	m_nColumns = r2.Width()/m_nItemWidth;
	m_nRows = r2.Height()/(dt.GetFontHeight()+2);

	SetDirectory(strDir);

	return true;
}

void FileBrowser::Draw()
{
	dt.SetCurrentObject(this);

	dt.FillRect(Rect(0,0,m_Rect.Width(), m_Rect.Height()), 0);

	if (m_bLoading)
		 return;

	int r,c;
	int h = dt.GetFontHeight()+2;
	int i=m_nDrawFileOffset;
	int x=(m_nDrawFileOffset>0) ? 7 : 0;
	for (c=0; c<m_nColumns && i<m_nNames; c++)
		 for (r=0; r<m_nRows && i<m_nNames; r++, i++) {
			  dt.SetClipRect(Rect(c*m_nItemWidth+x, r*h+1,
								  c*m_nItemWidth+m_nItemWidth+x-2, r*h+1+h));
			  dt.TextOut(Point(c*m_nItemWidth+x,r*h+1),
						 m_names[i],
						 m_isdir[i] ? 2 : 3);
		 }
	dt.SetClipRect();

	m_bMore = (i<m_nNames);
	m_bLess = m_nDrawFileOffset > 0;
	
	// arrow more
	if (m_bMore) {
		 int x = m_Rect.Width()-2;
		 int y = m_Rect.Height()/2;
		 dt.PutPixel(Point(x,   y ),  2);
		 dt.PutPixel(Point(x-1, y ),  2);
		 dt.PutPixel(Point(x-1, y-1), 2);
		 dt.PutPixel(Point(x-1, y+1), 2);
		 dt.PutPixel(Point(x-2, y  ), 2);
		 dt.PutPixel(Point(x-2, y-1), 2);
		 dt.PutPixel(Point(x-2, y+1), 2);
		 dt.PutPixel(Point(x-2, y-2), 2);
		 dt.PutPixel(Point(x-2, y+2), 2);
	}

	// arrow less
	if (m_bLess) {
		 int x = 1;
		 int y = m_Rect.Height()/2;
		 dt.PutPixel(Point(x,   y ),  2);
		 dt.PutPixel(Point(x+1, y ),  2);
		 dt.PutPixel(Point(x+1, y-1), 2);
		 dt.PutPixel(Point(x+1, y+1), 2);
		 dt.PutPixel(Point(x+2, y  ), 2);
		 dt.PutPixel(Point(x+2, y-1), 2);
		 dt.PutPixel(Point(x+2, y+1), 2);
		 dt.PutPixel(Point(x+2, y-2), 2);
		 dt.PutPixel(Point(x+2, y+2), 2);
	}
}

THREADFUNC FileBrowser::setDirectoryThread(void* fileBrowser)
{
#ifndef WIN32
	 pthread_detach(pthread_self());
#endif
	 LOWPRIORITY();

	 if (!fileBrowser)
		  return NULL;

	 FileBrowser* fb = (FileBrowser*)fileBrowser;

#ifdef WIN32

	char realdir[MAX_PATH];
	if (!realpath(fb->m_strDir, realdir))
		return 0;
	strncpy(fb->m_strDir, realdir, MAX_PATH);
	if (fb->m_pFolder)
		fb->m_pFolder->SetText(fb->m_strDir);

	// Are we in base directory?
	bool bBaseDir = fb->m_bBase ? strcmp(fb->m_strDir, fb->m_strBase)==0 : false;

	// Append search item
	strcat(realdir, DIR_SEPARATOR);
	strcat(realdir, "*");

	// Read directory
	_finddata_t fd;
	long lSearch = _findfirst(realdir, &fd);
	if (lSearch==-1)
		return;

	m_nNames=0;
	char pathstr[MAX_PATH];

	int extlen=0;
	if (m_bExt)
		extlen = strlen(fb->m_strExt);

	do {
		if (fb->m_bExt
			&& stricmp(fd.name+strlen(fd.name)-extlen, fb->m_strExt)!=0
			&& !(fd.attrib & _A_SUBDIR))
			continue;

		if (strcmp(fd.name, ".")==0) continue;
		if (bBaseDir && strcmp(fd.name, "..")==0) continue;

		strcpy(pathstr, fb->m_strDir);
		strcat(pathstr, DIR_SEPARATOR);
		strcat(pathstr, fd.name);
		fb->m_isdir[fb->m_nNames] = (fd.attrib & _A_SUBDIR)!=0;
		strcpy(fb->m_names[fb->m_nNames++], fd.name);
	} while ((_findnext(lSearch, &fd)==0) && (fb->m_nNames < MAX_FB_NAMES));

	_findclose(lSearch);

#else

	 char realdir[MAX_PATH];
	 if (!realpath(fb->m_strDir, realdir))
		  return NULL;
	 strncpy(fb->m_strDir, realdir, MAX_PATH);
	 if (fb->m_pFolder)
		  fb->m_pFolder->SetText(fb->m_strDir);

	 // Are we in base directory?
	 bool bBaseDir = fb->m_bBase ? strcmp(fb->m_strDir, fb->m_strBase)==0 : false;

	 // Read directory
	DIR *dir;
	dir = opendir(fb->m_strDir);
	fb->m_nNames=0;
	char pathstr[MAX_PATH];
	struct stat st;
	if (dir) {
		 struct dirent *de;
		 int extlen=0;
		 if (fb->m_bExt)
			  extlen = strlen(fb->m_strExt);

		 while ((de = readdir(dir)) && fb->m_nNames<MAX_FB_NAMES)
		 {
			  bool is_dir = false;
			  strcpy(pathstr, fb->m_strDir);
			  strcat(pathstr, DIR_SEPARATOR);
			  strcat(pathstr, de->d_name);

			  if ((stat(pathstr, &st)==0) && S_ISDIR(st.st_mode))
			    is_dir = true;

			  if (is_dir || (fb->m_bExt ?
							 (strcasecmp(de->d_name+strlen(de->d_name)-extlen,
										 fb->m_strExt)==0)
							 : true))
			  {
				   if (strcmp(de->d_name, ".")==0) continue;
				   if (bBaseDir && strcmp(de->d_name, "..")==0) continue;
				   fb->m_isdir[fb->m_nNames] = is_dir;
				   strcpy(fb->m_names[fb->m_nNames++], de->d_name);
			  }
		 }
		 closedir(dir);
	}

#endif

	fb->m_nDrawFileOffset = 0;

	fb->SetDirty();
	fb->m_bLoading = false;

	return NULL;
}

void FileBrowser::SetDirectory(const char *strDir)
{
	 if (m_bLoading) return;
	 m_bLoading = true;

	 if (strDir)
		  strcpy(m_strDir, strDir);
	 else
		  strcpy(m_strDir, ".");

	 HTHREAD thread;
	 if (!(CREATETHREAD(thread, setDirectoryThread, this)))
		  m_bLoading = false;
}

void FileBrowser::SetDirectoryFromBase(const char *strDir)
{
	 char path[MAX_PATH];
	 strcpy(path, m_strBase);
	 strcat(path, DIR_SEPARATOR);
	 strcat(path, strDir);
	 SetDirectory(path);
}

void FileBrowser::SetExtension(const char *strExt)
{
	 if (strExt) {
		  int len = strlen(strExt);
		  if (len > 14) len = 14;
		  memcpy(m_strExt, strExt, len);
		  m_strExt[len] = 0;
		  m_bExt = true;
	 }
	 else {
		  m_bExt = false;
	 }
}

void FileBrowser::SetBase(const char *strBase)
{
	 if (strBase) {
		  if (realpath(strBase, m_strBase))
			   m_bBase = true;
	 }
	 else {
		  m_bBase = false;
	 }

	 if (m_bBase) {
		  SetDirectoryFromBase(".");
	 }
}

void FileBrowser::OnMouseUp(Point mouse)
{
	 int r, c;
	 c = mouse.x / m_nItemWidth;
	 r = mouse.y / (dt.GetFontHeight()+2);

	 if (m_bMore && (mouse.x >= m_Rect.Width()-5)) {
		  m_nDrawFileOffset += m_Rect.Height()/(dt.GetFontHeight()+2);
		  SetDirty();
		  return;
	 }

	 if (m_bLess && (mouse.x <= 7)) {
		  m_nDrawFileOffset -= m_Rect.Height()/(dt.GetFontHeight()+2);
		  if (m_nDrawFileOffset < 0)
			   m_nDrawFileOffset = 0;
		  SetDirty();
		  return;
	 }

	 int n = c*m_nRows + r + m_nDrawFileOffset;
	 if (n < m_nNames)
	 {
		  if (m_isdir[n]) {
			   char path[MAX_PATH];
			   strcpy(path, m_strDir);
			   if (path[strlen(path)-1]!='/')
					strcat(path, DIR_SEPARATOR);
			   strcat(path, m_names[n]);
			   SetDirectory(path);
			   if (m_nDirCommand!=-1)
					gui.SetCommand(m_nDirCommand, (void*)m_strDir);
		  }
		  else {
			   strcpy(m_strFile, m_strDir);
			   if (m_strFile[strlen(m_strFile)-1]!=DIR_SEPARATOR[0])
					strcat(m_strFile, DIR_SEPARATOR);

			   strcat(m_strFile, m_names[n]);
			   if (m_nFileCommand!=-1)
					gui.SetCommand(m_nFileCommand, (void*)m_strFile);
		  }
	 }
}

