
#include <Windows.h>

#include "../Juna/Picture/Font.h"
#include "../Juna/Picture/ExpandedFont.h"
#include "../Juna/Lyrics/RubyLyricsViewer.h"
#include "../Juna/Picture/DIBImage.h"
#include "../Juna/Picture/Blit.h"
#include "../Juna/Picture/ElasticBlit.h"

#include "DisplayText.h"

static Juna::Picture::BufferedFont Font;
static Juna::Picture::BufferedExpandedFont EFont;
static Juna::Picture::BufferedFont RubyFont;
static Juna::Picture::BufferedExpandedFont ERubyFont;

static Juna::Lyrics::RubyLyricsViewer Viewer;
static Juna::Picture::DIBImage Image;

static Juna::Picture::Rect LyricsLayoutRect(0,0,0,0);


static Juna::Picture::Rect BGImageLayoutRect(0,0,0,0);

static Juna::Picture::Color BGColor(0,0,0);
static Juna::Picture::DIBImage BGImage;
static Juna::Picture::DIBImage DisplayBGImage;

static Juna::Picture::ARGB BGIFilter(0,0,0,0);

static bool LimitDisplayX = true;
static bool LimitDisplayY = true;
static bool LimitSourceX = false;
static bool LimitSourceY = false;
static bool LimitMaxX = false;
static bool LimitMaxY = false;

static int BGIMaxX = 0;
static int BGIMaxY = 0;

static int BGIPosition = 5;


static Juna::Lyrics::RubyLyricsViewer::DrawPoint last_draw_point = {0,0,0};
static bool ForceRedraw = true;


static DisplayText Text(Font,&EFont);
static Juna::Picture::Rect TextLayoutRect(0,0,0,0);



/*
bool __stdcall LyricsView_IsValid(void)
{
	return Viewer.IsValid();
}

void __stdcall LyricsView_SetLyrics(const wchar_t *lyrics)
{
	Viewer.SetLyrics(lyrics);
	ForceRedraw = true;
}

bool __stdcall LyricsView_SetFont(unsigned int fontquality,HFONT hfont,bool outline,int slip)
{
	Juna::Picture::Font font;
	if (!font.Initialize(hfont,fontquality))
		return false;
	Juna::Picture::Font rubyfont;
	if (!rubyfont.Initialize(font,font.GetHeight() / 2))
		return false;
	if (!(Font.Initialize(font) && RubyFont.Initialize(rubyfont)))
	{
		Font.Terminalize();
		return false;
	}
	ForceRedraw = true;

	if (slip <= 0)
		slip = 1;
	int rubyslip = slip / 2;
	if (rubyslip == 0)
		rubyslip = 1;
	if (!(EFont.Initialize(Font,slip) && ERubyFont.Initialize(RubyFont,rubyslip)))
	{
		EFont.Terminalize();
		RubyFont.Terminalize();
		Font.Terminalize();
		return false;
	}
	if (outline)
	{
		Text.SetFont(Font,&EFont);
		return Viewer.SetFont(&Font,&RubyFont,&EFont,&ERubyFont);
	}
	Text.SetFont(Font,0);
	return Viewer.SetFont(&Font,&RubyFont);
}

bool __stdcall LyricsView_SetFontOutline(bool outline,int slip)
{
	if (!Font.IsValid())
		return false;

	ForceRedraw = true;

	if (slip <= 0)
		slip = 1;
	int rubyslip = slip / 2;
	if (rubyslip == 0)
		rubyslip = 1;
	if (!(EFont.Initialize(Font,slip) && ERubyFont.Initialize(RubyFont,rubyslip)))
	{
		EFont.Terminalize();
		return false;
	}
	if (outline)
	{
		Text.SetFont(Font,&EFont);
		return Viewer.SetFont(&Font,&RubyFont,&EFont,&ERubyFont);
	}
	Text.SetFont(Font,0);
	return Viewer.SetFont(&Font,&RubyFont);
}


void __stdcall LyricsView_Terminalize(void)
{
	Viewer.Terminalize();
	ERubyFont.Terminalize();
	EFont.Terminalize();
	RubyFont.Terminalize();
	Font.Terminalize();
	Image.Terminalize();

	BGImage.Terminalize();
	DisplayBGImage.Terminalize();
}

void __stdcall LyricsView_Resize(int width,int height)
{
	Image.Initialize(width,height);
	LyricsLayoutRect.Set(0,0,width,height);
	BGImageLayoutRect.Set(0,0,width,height);
	ForceRedraw = true;
}
void __stdcall LyricsView_SetLyricsLayout(int left,int top,int right,int bottom)
{
	LyricsLayoutRect.Set(left,top,right,bottom);
	ForceRedraw = true;
}
void __stdcall LyricsView_SetBGImageLayout(int left,int top,int right,int bottom)
{
	BGImageLayoutRect.Set(left,top,right,bottom);
	ForceRedraw = true;
}


void UpdateBGI(void)
{
	int imagewidth = BGImageLayoutRect.width();
	int imageheight = BGImageLayoutRect.height();

	if (imagewidth <= 0 || imageheight <= 0)
		return;

	int targetwidth = INT_MAX;
	if (LimitDisplayX)
		targetwidth = imagewidth;
	if (LimitSourceX)
		targetwidth = BGImage.width() < targetwidth ? BGImage.width() : targetwidth;
	if (LimitMaxX)
		targetwidth = BGIMaxX < targetwidth ? BGIMaxX : targetwidth;
	if (targetwidth == INT_MAX)
		return;

	int targetheight = INT_MAX;
	if (LimitDisplayY)
		targetheight = imageheight;
	if (LimitSourceY)
		targetheight = BGImage.height() < targetheight ? BGImage.height() : targetheight;
	if (LimitMaxY)
		targetheight = BGIMaxY < targetheight ? BGIMaxY : targetheight;
	if (targetheight == INT_MAX)
		return;

	float targetrate = float(targetwidth) / targetheight;
	float bgrate = float(BGImage.width()) / BGImage.height();
	if (targetrate > bgrate)
	{
		if (targetheight != DisplayBGImage.height())
		{
			int width = BGImage.width() * targetheight / BGImage.height();
			DisplayBGImage.Initialize(width,targetheight);
			Juna::Picture::ElasticAvBlit(DisplayBGImage,BGImage);
			if (BGIFilter.a)
			{
				Juna::Picture::TranslucentFillColor(DisplayBGImage,BGIFilter,BGIFilter.a);
			}
		}
	}
	else
	{
		if (targetwidth != DisplayBGImage.width())
		{
			int height = BGImage.height() * targetwidth / BGImage.width();
			DisplayBGImage.Initialize(targetwidth,height);
			Juna::Picture::ElasticAvBlit(DisplayBGImage,BGImage);
			if (BGIFilter.a)
			{
				Juna::Picture::TranslucentFillColor(DisplayBGImage,BGIFilter,BGIFilter.a);
			}
		}
	}

	int bgx = BGImageLayoutRect.x1;
	int bgy = BGImageLayoutRect.y1;
	switch (BGIPosition % 3)
	{
	case 1: break;
	case 2: bgx += (imagewidth - DisplayBGImage.width()) / 2; break;
	case 0: bgx += imagewidth - DisplayBGImage.width(); break;
	}
	switch ((BGIPosition + 2) / 3)
	{
	case 3: break;
	case 2: bgy += (imageheight - DisplayBGImage.height()) / 2; break;
	case 1: bgy += imageheight - DisplayBGImage.height(); break;
	}

	Juna::Picture::Blit(Image,DisplayBGImage,bgx,bgy);
}

bool __stdcall LyricsView_Update(int playtime_ms)
{
	Juna::Lyrics::RubyLyricsViewer::DrawPoint dp;
	if (!Viewer.GetDrawPoint(dp,playtime_ms))
		return false;

	if (ForceRedraw ||
		dp.index != last_draw_point.index || dp.scroll_y != last_draw_point.scroll_y || dp.scroll_rate != last_draw_point.scroll_rate)
	{
		Juna::Picture::FillColor(Image,BGColor);
		if (BGImage.isValid())
		{
			UpdateBGI();
		}

		Viewer.Draw(Image,LyricsLayoutRect,dp);
		last_draw_point = dp;


		if (!Text.Empty())
		{
			Text.Draw(Image,TextLayoutRect);
		}

		ForceRedraw = false;

		return true;
	}
	return false;
}


bool __stdcall LyricsView_UpdateCS(int playtime_ms)
{
	Juna::Picture::FillColor(Image,BGColor);
	if (BGImage.isValid())
	{
		UpdateBGI();
	}

	Viewer.ContinuousScrollDraw(Image,LyricsLayoutRect,playtime_ms);
	if (!Text.Empty())
	{
		Text.Draw(Image,TextLayoutRect);
	}

	return true;
}



HDC __stdcall LyricsView_GetImage(int *width,int *height)
{
	*width = Image.width();
	*height = Image.height();
	return HDC(Image.GetDC());
}


void __stdcall LyricsView_SetCurrentColor(unsigned char r,unsigned char g,unsigned char b,
											unsigned char or,unsigned char og,unsigned char ob)
{
	Viewer.CurrentTextColor.Set(r,g,b);
	Viewer.CurrentTextOutlineColor.Set(or,og,ob);
	ForceRedraw = true;
}


void __stdcall LyricsView_SetOtherColor(unsigned char r,unsigned char g,unsigned char b,
											unsigned char or,unsigned char og,unsigned char ob)
{
	Viewer.OtherTextColor.Set(r,g,b);
	Viewer.OtherTextOutlineColor.Set(or,og,ob);
	ForceRedraw = true;
}

void __stdcall LyricsView_SetBGColor(unsigned char r,unsigned char g,unsigned char b)
{
	BGColor.Set(r,g,b);
	ForceRedraw = true;
}
void __stdcall LyricsView_SetBGImage(HBITMAP hbitmap)
{
	ForceRedraw = true;
	if (hbitmap == NULL)
	{
		BGImage.Terminalize();
		return;
	}
	BITMAP bitmap;
	if (::GetObject(hbitmap , sizeof (BITMAP) , &bitmap) == 0)
	{
		BGImage.Terminalize();
		return;
	}

	if (BGImage.width() != bitmap.bmWidth || BGImage.height() != bitmap.bmHeight)
	{
		BGImage.Initialize(bitmap.bmWidth,bitmap.bmHeight);
	}
	HDC hdc = ::CreateCompatibleDC(HDC(BGImage.GetDC()));
	HGDIOBJ hdefbm = ::SelectObject(hdc,hbitmap);
	::BitBlt(HDC(BGImage.GetDC()),0,0,bitmap.bmWidth,bitmap.bmHeight,hdc,0,0,SRCCOPY);
	::SelectObject(hdc,hdefbm);
	::DeleteDC(hdc);
	Juna::Picture::FillColor(BGImage,Juna::Picture::ARGB(255,0,0,0),Juna::Picture::ARGB(0,255,255,255));
	DisplayBGImage.Terminalize();
}

void __stdcall LyricsView_SetBGImagePosition(int position)
{
	BGIPosition = position;
	if (BGImage.isValid())
		ForceRedraw = true;
}


void __stdcall LyricsView_SetBGImageFilter(unsigned char a,unsigned char r,unsigned char g,unsigned char b)
{
	BGIFilter.Set(a,r,g,b);
	if (BGImage.isValid())
	{
		ForceRedraw = true;
		DisplayBGImage.Terminalize();
	}
}

void __stdcall LyricsView_SetBGImageLimitFlag(bool display_x,bool display_y,
											  bool source_x,bool source_y,
											  bool maxsize_x,bool maxsize_y)
{
	LimitDisplayX = display_x;
	LimitDisplayY = display_y;
	LimitSourceX = source_x;
	LimitSourceY = source_y;
	LimitMaxX = maxsize_x;
	LimitMaxY = maxsize_y;
	if (BGImage.isValid())
		ForceRedraw = true;
}


void __stdcall LyricsView_SetBGImageMaxSize(int width,int height)
{
	if (width < 0 || height < 0)
		return;

	BGIMaxX = width;
	BGIMaxY = height;
	if (BGImage.isValid())
		ForceRedraw = true;
}



void __stdcall LyricsView_SetLineBackColor(unsigned char a,unsigned char r,unsigned char g,unsigned char b)
{
	Viewer.CurrentLineBackColor.Set(a,r,g,b);
	ForceRedraw = true;
}


void __stdcall LyricsView_SetLineBackPlusUp(int pluswidth)
{
	Viewer.CurrentLineBackPlusUp = pluswidth;
	ForceRedraw = true;
}
void __stdcall LyricsView_SetLineBackPlusDown(int pluswidth)
{
	Viewer.CurrentLineBackPlusDown = pluswidth;
	ForceRedraw = true;
}


void __stdcall LyricsView_SetScrollTime(int scroll_time,int fade_time)
{
	Viewer.ScrollTime = scroll_time;
	Viewer.ScrollFadeTime = fade_time;
	ForceRedraw = true;
}


void __stdcall LyricsView_SetVAlignment(int alignment,int y_offset)
{
	if (alignment < 0)
		Viewer.VAlignment = Juna::Lyrics::VA_Bottom;
	else if (alignment > 0)
		Viewer.VAlignment = Juna::Lyrics::VA_Top;
	else
		Viewer.VAlignment = Juna::Lyrics::VA_Center;
	Viewer.CurrentLineYOffset = y_offset;
	ForceRedraw = true;
}

void __stdcall LyricsView_SetHAlignment(int alignment)
{
	if (alignment < 0)
		Viewer.HAlignment = Juna::Lyrics::HA_Right;
	else if (alignment > 0)
		Viewer.HAlignment = Juna::Lyrics::HA_Left;
	else
		Viewer.HAlignment = Juna::Lyrics::HA_Center;
	ForceRedraw = true;
}

void __stdcall LyricsView_SetSpace(int linespace,int charspace,int norubylinetopspace,int rubybottomspace)
{
	Viewer.LineSpace = linespace;
	Viewer.SetCharSpace(charspace);
	Viewer.NoRubyLineTopSpace = norubylinetopspace;
	Viewer.RubyBottomSpace = rubybottomspace;
	ForceRedraw = true;
}

void __stdcall LyricsView_SetMargin(int left,int top,int right,int bottom)
{
	Viewer.LeftMargin = left;
	Viewer.TopMargin = top;
	Viewer.RightMargin = right;
	Viewer.BottomMargin = bottom;
	ForceRedraw = true;
}

void __stdcall LyricsView_SetRubyLineMode(int mode)
{
	switch (mode)
	{
	case 1:
		Viewer.RubyLineMode = Juna::Lyrics::RubyLyricsViewer::RLM_Force;
		break;
	case 2:
		Viewer.RubyLineMode = Juna::Lyrics::RubyLyricsViewer::RLM_Ignore;
		break;
	default:
		Viewer.RubyLineMode = Juna::Lyrics::RubyLyricsViewer::RLM_Default;
		break;
	}
	ForceRedraw = true;
}
	


void __stdcall LyricsView_SetString(int left,int top,int right,int bottom,
									int valignment,int halignment,const wchar_t *string,
									unsigned char r,unsigned char g,unsigned char b,
									bool outline,
									unsigned char or,unsigned char og,unsigned char ob)
{
	TextLayoutRect.Set(left,top,right,bottom);
	Text.VAlignment = valignment;
	Text.HAlignment = halignment;
	Text.SetText(string);
	Text.Color.Set(r,g,b);
	Text.OutlineColor.Set(or,og,ob);
	if (outline)
		Text.SetFont(Font,&EFont);
	else
		Text.SetFont(Font,0);

	ForceRedraw = true;
}

*/