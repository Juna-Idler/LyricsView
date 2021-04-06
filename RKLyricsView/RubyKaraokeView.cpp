
#include <Windows.h>

#include "../Juna/Picture/Font.h"
#include "../Juna/Picture/ExpandedFont.h"
#include "../Juna/Lyrics/RubyKaraokeViewer.h"
#include "../Juna/Picture/DIBImage.h"
#include "../Juna/Picture/Blit.h"
#include "../Juna/Picture/ElasticBlit.h"


static Juna::Picture::BufferedFont Font;
static Juna::Picture::BufferedExpandedFont EFont;
static Juna::Picture::BufferedFont RubyFont;
static Juna::Picture::BufferedExpandedFont ERubyFont;

static Juna::Lyrics::RubyKaraokeViewer Viewer;
static Juna::Picture::DIBImage Image;

static Juna::Picture::Color BGColor(0,0,0);
static Juna::Picture::DIBImage BGImage;
static Juna::Picture::DIBImage DisplayBGImage;

bool __stdcall KaraokeView_IsValid(void)
{
	return Viewer.IsValid();
}

void __stdcall KaraokeView_SetLyrics(const wchar_t *lyrics)
{
	Viewer.SetLyrics(lyrics);
}

bool __stdcall KaraokeView_SetFont(unsigned int fontquality,HFONT hfont,bool outline,int slip)
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
	if (outline)
	{
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
		return Viewer.SetFont(&Font,&RubyFont,&EFont,&ERubyFont);
	}
	return Viewer.SetFont(&Font,&RubyFont);
}

bool __stdcall KaraokeView_SetFontOutline(bool outline,int slip)
{
	if (!Font.IsValid())
		return false;
	if (outline)
	{
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
		return Viewer.SetFont(&Font,&RubyFont,&EFont,&ERubyFont);
	}
	return Viewer.SetFont(&Font,&RubyFont);
}


void __stdcall KaraokeView_Terminalize(void)
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


void __stdcall KaraokeView_Draw(HDC hdc,int x,int y,int width,int height,int playtime_ms)
{
	if (Image.width() != width || Image.height() != height)
	{
		Image.Initialize(width,height);
	}
	Juna::Picture::FillColor(Image,BGColor);
	if (BGImage.isValid())
	{
		float targetrate = float(Image.width()) / Image.height();
		float bgrate = float(BGImage.width()) / BGImage.height();
		if (targetrate > bgrate)
		{
			if (Image.height() != DisplayBGImage.height())
			{
				int width = BGImage.width() * Image.height() / BGImage.height();
				DisplayBGImage.Initialize(width,Image.height());
				Juna::Picture::ElasticBlit(DisplayBGImage,BGImage);
			}
		}
		else
		{
			if (Image.width() != DisplayBGImage.width())
			{
				int height = BGImage.height() * Image.width() / BGImage.width();
				DisplayBGImage.Initialize(Image.width(),height);
				Juna::Picture::ElasticBlit(DisplayBGImage,BGImage);
			}
		}

		int bgx = (width - DisplayBGImage.width()) / 2;
		int bgy = (height - DisplayBGImage.height()) / 2;
		Juna::Picture::Blit(Image,DisplayBGImage,bgx,bgy);
	}
	Viewer.Draw(Image,playtime_ms);
	::BitBlt(hdc,x,y,width,height,HDC(Image.GetDC()),0,0,SRCCOPY);
}



void __stdcall KaraokeView_SetCurrentColor(unsigned char r,unsigned char g,unsigned char b,
											unsigned char or,unsigned char og,unsigned char ob)
{
	Viewer.CurrentTextColor.Set(r,g,b);
	Viewer.CurrentTextOutlineColor.Set(or,og,ob);
}

void __stdcall KaraokeView_SetActiveColor(unsigned char r,unsigned char g,unsigned char b,
											unsigned char or,unsigned char og,unsigned char ob)
{
	Viewer.ActiveTextColor.Set(r,g,b);
	Viewer.ActiveTextOutlineColor.Set(or,og,ob);
}

void __stdcall KaraokeView_SetOtherColor(unsigned char r,unsigned char g,unsigned char b,
											unsigned char or,unsigned char og,unsigned char ob)
{
	Viewer.OtherTextColor.Set(r,g,b);
	Viewer.OtherTextOutlineColor.Set(or,og,ob);
}

void __stdcall KaraokeView_SetBGColor(unsigned char r,unsigned char g,unsigned char b)
{
	BGColor.Set(r,g,b);
}
void __stdcall KaraokeView_SetBGImage(HBITMAP hbitmap)
{
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


void __stdcall KaraokeView_SetLineBackColor(unsigned char a,unsigned char r,unsigned char g,unsigned char b)
{
	Viewer.CurrentLineBackColor.Set(a,r,g,b);
}

void __stdcall KaraokeView_SetScrollTime(int scroll_time,int fade_time)
{
	Viewer.ScrollTime = scroll_time;
	Viewer.ScrollFadeTime = fade_time;
}


void __stdcall KaraokeView_SetVAlignment(int alignment)
{
	if (alignment < 0)
		Viewer.VAlignment = Juna::Lyrics::VA_Bottom;
	else if (alignment > 0)
		Viewer.VAlignment = Juna::Lyrics::VA_Top;
	else
		Viewer.VAlignment = Juna::Lyrics::VA_Center;
}

void __stdcall KaraokeView_SetHAlignment(int alignment)
{
	if (alignment < 0)
		Viewer.HAlignment = Juna::Lyrics::HA_Right;
	else if (alignment > 0)
		Viewer.HAlignment = Juna::Lyrics::HA_Left;
	else
		Viewer.HAlignment = Juna::Lyrics::HA_Center;
}

void __stdcall KaraokeView_SetSpace(int linespace,int charspace,int norubylinetopspace,int rubybottomspace)
{
	Viewer.LineSpace = linespace;
	Viewer.SetCharSpace(charspace);
	Viewer.NoRubyLineTopSpace = norubylinetopspace;
	Viewer.RubyBottomSpace = rubybottomspace;
}



void __stdcall KaraokeView_SetMargin(int left,int top,int right,int bottom)
{
	Viewer.LeftMargin = left;
	Viewer.TopMargin = top;
	Viewer.RightMargin = right;
	Viewer.BottomMargin = bottom;
}

void __stdcall KaraokeView_SetRubyLineMode(int mode)
{
	switch (mode)
	{
	case 1:
		Viewer.RubyLineMode = Juna::Lyrics::RubyKaraokeViewer::RLM_Force;
		break;
	case 2:
		Viewer.RubyLineMode = Juna::Lyrics::RubyKaraokeViewer::RLM_Ignore;
		break;
	default:
		Viewer.RubyLineMode = Juna::Lyrics::RubyKaraokeViewer::RLM_Default;
		break;
	}
}
