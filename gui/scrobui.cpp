
#include <stdio.h>

#define _SCROBUI_CPP_
#include "scrobui.h"
#include "dotum10.h"
#include <iostream>
#include <fstream>

#undef TextOut

using namespace std;

//using namespace ScrobUI;

/*
** SDL Screen Object User Interface
*/

Scrob::Scrob()
{
	m_pChildList=NULL;
	m_pNextSibling=NULL;
	m_pPrevSibling=NULL;
	m_pLastChild=NULL;
	m_pParent=NULL;
	m_bDirty=false;
	m_bVisible=true;
}

Scrob::~Scrob()
{
	Scrob *pTmpScrob, *pScrob = m_pChildList;
	while (pScrob)
	{
		pTmpScrob = pScrob;
		pScrob = pScrob->m_pNextSibling;
		delete pTmpScrob;
	}

	m_pChildList = NULL;
	m_pLastChild = NULL;
	m_pPrevSibling = NULL;
}

Scrob::Scrob(Scrob *pParent, const Rect& r)
{
	m_pChildList=NULL;
	m_pNextSibling=NULL;
	m_pPrevSibling=NULL;
	m_pLastChild=NULL;
	m_pParent=NULL;
	m_bDirty=false;
	m_bVisible=true;
	Create(pParent, r);
}

bool Scrob::Create(Scrob *pParent, const Rect& r)
{
	m_pParent = pParent;
	m_Rect = r;
	m_bDirty = true;
	m_bVisible = true;
	return true;
}

bool Scrob::AddChild(Scrob* pScrob)
{
	if (m_pChildList && m_pLastChild)
	{
		m_pLastChild->m_pNextSibling = pScrob;
		pScrob->m_pPrevSibling = m_pLastChild;
		m_pLastChild = pScrob;
	}
	else
	{
		m_pChildList = pScrob;
		m_pLastChild = pScrob;		
	}
	pScrob->m_pNextSibling = NULL;

	return true;
}

void Scrob::DrawFamily()
{
	if (!m_bVisible)
	    return;

	if (m_bDirty)
	{
		dt.SetCurrentObject(this);

		Point p = m_Rect.TopLeft();
		if (m_pParent) p = m_pParent->ScrobToScreen(p);
		Rect r(p.x, p.y, p.x+m_Rect.Width(), p.y+m_Rect.Height());
		gui.SetRectDirty(r);

		Draw();
		m_bDirty = false;
	}

	Scrob *pScrob = m_pChildList;
	while (pScrob)
	{
		pScrob->DrawFamily();
		pScrob = pScrob->m_pNextSibling;
	}
}

void Scrob::SetDirty()
{
	m_bDirty = true;
	Scrob *pScrob = m_pChildList;
	while (pScrob)
	{
		pScrob->SetDirty();
		pScrob = pScrob->GetNextSibling();
	}
}

void Scrob::SetVisible(bool visible)
{
	 if (visible==m_bVisible)
		  return;

	 m_bVisible = visible;
	 SetDirty();
}

Scrob *Scrob::GetHitChild(Point p)
{
	// Note: this assumes all childing are *inside*
    // the scrob's rectangle, which really should be the case anyway
	if (!(m_bVisible && m_Rect.PointInRect(p)))
		return NULL;
	else
	{
		 // Decend children in reverse order -- because later siblings
		 // are drawn on top of previous siblings
		Scrob *pScrob = m_pLastChild, *pHitScrob=NULL;
		while (pScrob && !pHitScrob)
		{
			pHitScrob = pScrob->GetHitChild(p-m_Rect.TopLeft());
			pScrob = pScrob->GetPrevSibling();
		}
		return pHitScrob ? pHitScrob : this;
	}
}

void Scrob::OnMouseDown(Point mouse)
{
	 gui.SetFocus(this);
	 if (m_pParent) m_pParent->OnMouseDown(mouse);
}

Point Scrob::ScreenToScrob(Point p)
{
	if (m_pParent)
		return m_pParent->ScreenToScrob(p) - m_Rect.TopLeft();
	else
		return p;
}

Point Scrob::ScrobToScreen(Point p)
{
	if (m_pParent)
		return m_Rect.TopLeft() + m_pParent->ScrobToScreen(p);
	else
		return p + m_Rect.TopLeft();
}

GUI::GUI()
{
	m_pScrobList = NULL;
	m_pScrobGotMouse = NULL;
	m_nCommand = 0;
	m_nCommandValue = NULL;
}

GUI::~GUI()
{
	Cleanup();
}

bool GUI::Initialize(const char *strTitle, bool bFullScreen, int nWidth, int nHeight, unsigned int extraSDLflags)
{
	// Initialize SDL
	if (SDL_Init ( SDL_INIT_VIDEO | extraSDLflags )!=0)
	{
		fprintf(stderr, "Could not initialize SDL.\n");
		return false;
	}

	// Enable Unicode translation
	SDL_EnableUNICODE(1);

	// Load character data
	if (!dt.LoadCharacterData())
		return false;

	// create a window
	m_bFullScreen = bFullScreen;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_pSurface = SDL_SetVideoMode ( nWidth, nHeight, 8, SDL_HWPALETTE | (bFullScreen ? SDL_FULLSCREEN : 0));
	if (!m_pSurface)
	{
		fprintf(stderr, "Couldn't set video mode.\n");
		return false;
	}

	const SDL_VideoInfo *vidinfo = SDL_GetVideoInfo();

	dt.SetSurface(m_pSurface);

	SetPalette();

	// set window title
	if (strTitle)
		SDL_WM_SetCaption(strTitle, NULL);

	// Create initial full-window scrob
	m_pScrobList = new Scrob();
	Rect r(0,0,nWidth-1,nHeight-1);
	m_pScrobList->Create(NULL, r);

	return true;
}

void GUI::SetPalette()
{
	// set the palette
	SDL_Color palette[8];
	palette[0].r = 0x00;	palette[0].g = 0x00;	palette[0].b = 0x00;
	palette[1].r = 0x55;	palette[1].g = 0x55;	palette[1].b = 0x55;
	palette[2].r = 0xAA;	palette[2].g = 0xAA;	palette[2].b = 0xAA;
	palette[3].r = 0xFF;	palette[3].g = 0xFF;	palette[3].b = 0xFF;
	palette[4].r = 0x00;	palette[4].g = 0x33;	palette[4].b = 0x00;
	palette[5].r = 0x00;	palette[5].g = 0x00;	palette[5].b = 0x33;
	palette[6].r = 0x00;	palette[6].g = 0xDD;	palette[6].b = 0x99;
	palette[7].r = 0xFF;	palette[7].g = 0xFF;	palette[7].b = 0x22;
	SDL_SetColors(m_pSurface, palette, 0, 8);
}

void GUI::Cleanup()
{
	// Clean up screen objects (deallocates memory allocated by user program)
	Scrob *pTmpScrob, *pScrob = m_pScrobList;
	while (pScrob)
	{
		pTmpScrob = pScrob;
		pScrob = pScrob->GetNextSibling();
		delete pTmpScrob;
	}
	m_pScrobList = NULL;
	m_pScrobGotMouse = NULL;

	// Clean up SDL
	if (m_pSurface)
	{
		SDL_Quit();
		m_pSurface = NULL;
	}
}

void GUI::WaitEvent()
{
	SDL_WaitEvent( &m_Event );
}

bool GUI::CheckEvent()
{
	return SDL_PollEvent( &m_Event )!=0;
}

bool GUI::ProcessEvent()
{
	// message pump
	if ( m_Event.type == SDL_QUIT )
		return false;
	else if ( (m_Event.type == SDL_KEYUP) && (m_Event.key.keysym.sym==SDLK_ESCAPE) )
		return false;
	else if ( (m_Event.type == SDL_KEYUP) && (m_Event.key.keysym.sym==SDLK_RETURN && m_Event.key.keysym.mod==KMOD_RALT) )
		SetFullScreen(!m_bFullScreen);
	else if ( (m_Event.type == SDL_KEYUP) && m_pScrobFocus)
		m_pScrobFocus->OnKeyUp(&m_Event.key.keysym);
	else if ( (m_Event.type == SDL_KEYDOWN) && m_pScrobFocus)
		m_pScrobFocus->OnKeyDown(&m_Event.key.keysym);
	else if (  (m_Event.type == SDL_MOUSEBUTTONUP)
			|| (m_Event.type == SDL_MOUSEBUTTONDOWN)
			|| (m_Event.type == SDL_MOUSEMOTION) )
	{
		Scrob *pScrob = m_pScrobList, *pHitScrob = NULL;
		Point p((m_Event.type==SDL_MOUSEMOTION) ? m_Event.motion.x : m_Event.button.x,
				(m_Event.type==SDL_MOUSEMOTION) ? m_Event.motion.y : m_Event.button.y);

		if (m_pScrobGotMouse)
			pHitScrob = m_pScrobGotMouse;

		while (pScrob && !pHitScrob)
		{
			pHitScrob = pScrob->GetHitChild(p);
			pScrob = pScrob->GetNextSibling();
		}

		if (pHitScrob)
		{
			switch (m_Event.type)
			{
			case SDL_MOUSEBUTTONUP:
				pHitScrob->OnMouseUp(pHitScrob->ScreenToScrob(p));
				break;
			case SDL_MOUSEBUTTONDOWN:
				pHitScrob->OnMouseDown(pHitScrob->ScreenToScrob(p));
				break;
			case SDL_MOUSEMOTION:
				pHitScrob->OnMouseMove(pHitScrob->ScreenToScrob(p));
				break;
			}
		}
	}

	// update GUI
	// TODO: make sure this isn't called more than 30 times per second or so
	UpdateScreen();

	return true;
}

void GUI::UpdateScreen()
{
	if (!m_pSurface)
		return;

	if (SDL_LockSurface(m_pSurface)!=0)
		fprintf(stderr, "Couldn't lock surface.\n");

	// track dirty rectangles
	m_rDirty.Set(0,0,-1,-1); // invalid rect

	// update GUI
	Scrob *pScrob = m_pScrobList;
	while (pScrob)
	{
		pScrob->DrawFamily();
		pScrob = pScrob->GetNextSibling();
	}

	SDL_UnlockSurface(m_pSurface);

	if (m_rDirty.IsValid())
	  SDL_UpdateRect(m_pSurface, m_rDirty.x1, m_rDirty.y1,
					 m_rDirty.Width(), m_rDirty.Height());
}

bool GUI::SetFullScreen(bool bFullScreen)
{
	SDL_Surface *pSurface = SDL_SetVideoMode ( m_nWidth, m_nHeight, 8, SDL_HWPALETTE | (bFullScreen ? SDL_FULLSCREEN : 0));
	if (!pSurface)
	{
		fprintf(stderr, "Couldn't set video mode.\n");
		return false;
	}

	m_pSurface = pSurface;
	m_bFullScreen = bFullScreen;
	dt.SetSurface(m_pSurface);

	// set the palette
	SetPalette();

	// mark whole screen dirty
	if (m_pScrobList)
		m_pScrobList->SetDirty();

	return true;
}

bool GUI::GetCommand(int* command, void** value)
{
  if (m_nCommand != 0) {
	if (command) *command = m_nCommand;
	if (value) *value = m_nCommandValue;
	m_nCommand = 0;
	m_nCommandValue = NULL;
	return true;
  }

  return false;
}

DrawingTools::DrawingTools()
{
	m_pCurrentObject = NULL;
	m_pSurface = NULL;
	m_pCharacterData = NULL;
	m_pCharacterSurface = NULL;
	m_bClip = false;
}

DrawingTools::~DrawingTools()
{
#ifdef EMBEDDED_FONTS
	if (m_pCharacterData)
		delete m_pCharacterData;
#endif

	if (m_pCharacterSurface)
		SDL_FreeSurface(m_pCharacterSurface);
}

Scrob *DrawingTools::SetCurrentObject(Scrob *pScrob) 
{
	Scrob *p = m_pCurrentObject;
	m_pCurrentObject = pScrob;
	m_ptOffset = m_pCurrentObject->ScrobToScreen(Point(0,0));
	return p;
}

void DrawingTools::DrawRect(const Rect &r, int color)
{
	if (!m_pCurrentObject || !m_pSurface)
		return;

	SDL_Rect sdlr;
	Point p = Rect(r).TopLeft() + m_ptOffset;
	sdlr.x = p.x; sdlr.y = p.y; sdlr.w = r.Width(); sdlr.h = 1;
	SDL_FillRect(m_pSurface, &sdlr, color);
	sdlr.x+=1; sdlr.y = p.y+r.Height()-1; sdlr.w-=1;
	SDL_FillRect(m_pSurface, &sdlr, color);
	sdlr.x = p.x; sdlr.y = p.y; sdlr.w = 1; sdlr.h = r.Height();
	SDL_FillRect(m_pSurface, &sdlr, color);
	sdlr.x = p.x+r.Width()-1; sdlr.y+=1; sdlr.h-=1;
	SDL_FillRect(m_pSurface, &sdlr, color);
}

void DrawingTools::DrawEdge(const Rect& r, bool bBevel, bool bFlat)
{
	if (!m_pCurrentObject || !m_pSurface)
		return;

	int a = 2, b = 1;
	if (!bBevel)
	{
		 a = 1;
		 b = 2;
	}

#ifdef FLAT_GUI
	if (!bFlat)  // 3d edges
#endif
	{ 
		 SDL_Rect sdlr;
		 Point p = Rect(r).TopLeft() + m_ptOffset;
		 sdlr.x = p.x; sdlr.y = p.y; sdlr.w = r.Width(); sdlr.h = 1;
		 SDL_FillRect(m_pSurface, &sdlr, a);
		 sdlr.x+=1; sdlr.y = p.y+r.Height()-1; sdlr.w-=1;
		 SDL_FillRect(m_pSurface, &sdlr, b);
		 sdlr.x = p.x; sdlr.y = p.y; sdlr.w = 1; sdlr.h = r.Height();

		 SDL_FillRect(m_pSurface, &sdlr, a);
		 sdlr.x = p.x+r.Width()-1; sdlr.y+=1; sdlr.h-=1;
		 SDL_FillRect(m_pSurface, &sdlr, b);
		 
		 Rect r2(r);
		 r2.x1+=1; r2.y1+=1; r2.x2-=1; r2.y2-=1;
		 a+=1; b+=1;
		 
		 p = r2.TopLeft() + m_ptOffset;
		 sdlr.x = p.x; sdlr.y = p.y; sdlr.w = r2.Width(); sdlr.h = 1;
		 SDL_FillRect(m_pSurface, &sdlr, a);
		 sdlr.x+=1; sdlr.y = p.y+r2.Height()-1; sdlr.w-=1;
		 SDL_FillRect(m_pSurface, &sdlr, b);
		 sdlr.x = p.x; sdlr.y = p.y; sdlr.w = 1; sdlr.h = r2.Height();
		 SDL_FillRect(m_pSurface, &sdlr, a);
		 sdlr.x = p.x+r2.Width()-1; sdlr.y+=1; sdlr.h-=1;
		 SDL_FillRect(m_pSurface, &sdlr, b);
	}
#ifdef FLAT_GUI
	else   // flat
	{
		 SDL_Rect sdlr;
		 Point p = Rect(r).TopLeft() + m_ptOffset;
		 int w = r.Width(), h = r.Height();
		 sdlr.x = p.x+1; sdlr.y = p.y; sdlr.w = w-2, sdlr.h = 1;
		 SDL_FillRect(m_pSurface, &sdlr, 3);
		 sdlr.x = p.x+1; sdlr.y = p.y+h-1; sdlr.w = w-2, sdlr.h = 1;
		 SDL_FillRect(m_pSurface, &sdlr, 3);
		 sdlr.x = p.x; sdlr.y = p.y+1; sdlr.w = 1, sdlr.h = h-2;
		 SDL_FillRect(m_pSurface, &sdlr, 3);
		 sdlr.x = p.x+w-1; sdlr.y = p.y+1; sdlr.w = 1, sdlr.h = h-2;
		 SDL_FillRect(m_pSurface, &sdlr, 3);

		 sdlr.x = p.x+2; sdlr.y = p.y+1; sdlr.w = w-4, sdlr.h = 1;
		 SDL_FillRect(m_pSurface, &sdlr, b);
		 sdlr.x = p.x+1; sdlr.y = p.y+2; sdlr.w = 1, sdlr.h = h-4;
		 SDL_FillRect(m_pSurface, &sdlr, b);

		 sdlr.x = p.x+2; sdlr.y = p.y+2; sdlr.w = w-4, sdlr.h = 1;
		 SDL_FillRect(m_pSurface, &sdlr, a);
		 sdlr.x = p.x+2; sdlr.y = p.y+2; sdlr.w = 1, sdlr.h = h-4;
		 SDL_FillRect(m_pSurface, &sdlr, a);
		 
		 ((Uint8 *)m_pSurface->pixels)[p.y*m_pSurface->pitch + p.x] = 1;
		 ((Uint8 *)m_pSurface->pixels)[p.y*m_pSurface->pitch + p.x+w-1] = 1;
		 ((Uint8 *)m_pSurface->pixels)[(p.y+h-1)*m_pSurface->pitch + p.x] = 1;
		 ((Uint8 *)m_pSurface->pixels)[(p.y+h-1)*m_pSurface->pitch + p.x+w-1] = 1;
	}
#endif
}

void DrawingTools::FillRect(Rect r, int color)
{
	if (!m_pCurrentObject || !m_pSurface)
		return;

	r.Offset(m_ptOffset);
	if (m_bClip)
		r.Intersect(m_rClip);
	SDL_Rect sr = (SDL_Rect)r;
	if (r.IsValid())
		SDL_FillRect(m_pSurface, &sr, color);
}

#include <assert.h>
bool DrawingTools::LoadCharacterData()
{
	short sx, sy;

	m_nFontHeight = sy = Font_Height;
	sx = Font_BitmapWidth;
	m_pCharacterData = Font_CharacterData;
	m_nCharOffsets = Font_CharOffsets;
	m_nCharWidths = Font_CharWidths;

	m_pCharacterSurface = SDL_CreateRGBSurfaceFrom(m_pCharacterData, sx, sy, 8, sx, 0, 0, 0, 0);
	if (!m_pCharacterSurface)
	{
		fprintf(stderr, "Couldn't create character surface.\n");
		m_pCharacterData = NULL;
		return false;
	}

	return true;
}

void DrawingTools::TextOut(Point pt, const char *str, int color)
{
	int i=0, pos=pt.x;
	char ch, c;
	Uint8 *chars = (Uint8 *)m_pCharacterSurface->pixels;

	while (str[i])
	{
		c = str[i++];
		int w = m_nCharWidths[c-' '];
		int o = m_nCharOffsets[c-' '];
		Point p = m_ptPosition - m_ptOffset;
		for (int y=0; y<m_nFontHeight; y++)
		{
			for (int x=0; x<w; x++)
			{
				ch = chars[y*m_pCharacterSurface->pitch + o + x];
				if (ch == 0)
					PutPixel(Point(pos+x, pt.y+y), color);
			}
		}

		pos += w;
	}
	m_ptPosition += Point(pos, 0);
}

void DrawingTools::SetClipRect(Rect pr)
{
	 m_rClip = pr;
	 m_rClip.Offset(m_ptOffset);

	 m_bClip = true;
}


void DrawingTools::SetClipRect() {
	 m_bClip = false;
}

void DrawingTools::PutPixel(Point pt, int color)
{
	pt += m_ptOffset;
	if (m_bClip)
	{
		if (!m_rClip.PointInRect(pt))
			return;
	}

	((Uint8 *)m_pSurface->pixels)[pt.y*m_pSurface->pitch + pt.x] = color;
}

int DrawingTools::GetPixel(Point pt)
{
	pt += m_ptOffset;
	if (m_bClip)
	{
		if (!m_rClip.PointInRect(pt))
			return 0;
	}

	return ((Uint8 *)m_pSurface->pixels)[pt.y*m_pSurface->pitch + pt.x];
}

int DrawingTools::GetTextWidth(const char *str, int nChars)
{
	int x=0;
	for (int i=0; (i < nChars) && str[i]; i++)
		x += m_nCharWidths[str[i]-' '];
	return x;
}
