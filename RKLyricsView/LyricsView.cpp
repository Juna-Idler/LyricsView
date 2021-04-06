
#include <Windows.h>

#include "../Juna/Lyrics/LyricsViewer.h"

#include "../Juna/Picture/Blit.h"

//#include "DisplayText.h"


static Juna::Lyrics::LyricsViewer Viewer;



bool __stdcall LyricsView_IsValid(void)
{
	return Viewer.IsValid();
}
int __stdcall LyricsView_GetLyricsMode(void)
{
	return Viewer.GetLyricsMode();
}

void __stdcall LyricsView_SetLyrics(const wchar_t *lyrics)
{
	Viewer.SetLyrics(lyrics);
}
void __stdcall LyricsView_SetKaraoke(const wchar_t *lyrics,bool keephead)
{
	Viewer.SetKaraoke(lyrics,keephead);
}
void __stdcall LyricsView_SetText(const wchar_t *lyrics)
{
	Viewer.SetText(lyrics);
}
void __stdcall LyricsView_SetRubyKaraoke(const wchar_t *lyrics,bool keephead)
{
	Viewer.SetRubyKaraoke(lyrics,keephead);
}


bool __stdcall LyricsView_SetFont(unsigned int fontquality,HFONT hfont,bool outline,int slip)
{
	Juna::Picture::OutlineFont font;
	if (!font.Initialize(hfont,fontquality))
		return false;

	return Viewer.SetFont(fontquality,font,slip);
}



void __stdcall LyricsView_Terminalize(void)
{
	Viewer.Terminalize();
}

void __stdcall LyricsView_Resize(int width,int height)
{
	Viewer.Resize(width,height);
}
void __stdcall LyricsView_SetLyricsLayout(int left,int top,int right,int bottom)
{
	Viewer.LyricsLayoutRect.Set(left,top,right,bottom);
	Viewer.SetForceRedraw();
}
void __stdcall LyricsView_SetBGImageLayout(int left,int top,int right,int bottom)
{
	Viewer.BGImageLayoutRect.Set(left,top,right,bottom);
	Viewer.SetForceRedraw();
}

bool __stdcall LyricsView_Update(int playtime_ms)
{
	return Viewer.Update(playtime_ms);
}


bool __stdcall LyricsView_UpdateCS(int playtime_ms)
{
	return Viewer.UpdateContinuousScroll(playtime_ms);
}

bool __stdcall LyricsView_KaraokeUpdate(int playtime_ms,int bottom_margin)
{
	return Viewer.UpdateKaraoke(playtime_ms,bottom_margin);
}


HDC __stdcall LyricsView_GetCanvasDC(int *width,int *height)
{
	const Juna::Picture::DIBImage &image = Viewer.GetCanvas();
	image.SelectBitmap();
	*width = image.width();
	*height = image.height();
	return HDC(image.GetDC());
}
HBITMAP __stdcall LyricsView_GetCanvasBitmap(int *width,int *height)
{
	const Juna::Picture::DIBImage &image = Viewer.GetCanvas();
	image.DeselectBitmap();
	*width = image.width();
	*height = image.height();
	return HBITMAP(image.GetBitmap());
}

void __stdcall LyricsView_SetCurrentColor(unsigned char r,unsigned char g,unsigned char b,
											unsigned char or,unsigned char og,unsigned char ob)
{
	Viewer.Parameter.CurrentTextColor.Set(r,g,b);
	Viewer.Parameter.CurrentTextOutlineColor.Set(or,og,ob);
	Viewer.SetForceRedraw();
}

void __stdcall LyricsView_SetOtherColor(unsigned char r,unsigned char g,unsigned char b,
											unsigned char or,unsigned char og,unsigned char ob)
{
	Viewer.Parameter.OtherTextColor.Set(r,g,b);
	Viewer.Parameter.OtherTextOutlineColor.Set(or,og,ob);
	Viewer.SetForceRedraw();
}
void __stdcall LyricsView_SetStandbyColor(unsigned char r,unsigned char g,unsigned char b,
										  unsigned char or,unsigned char og,unsigned char ob)
{
	Viewer.Parameter.StandbyTextColor.Set(r,g,b);
	Viewer.Parameter.StandbyTextOutlineColor.Set(or,og,ob);
	Viewer.SetForceRedraw();
}
void __stdcall LyricsView_SetNTTTextColor(unsigned char r,unsigned char g,unsigned char b,
										  unsigned char or,unsigned char og,unsigned char ob)
{
	Viewer.NTTTextColor.Set(r,g,b);
	Viewer.NTTTextOutlineColor.Set(or,og,ob);
	if (Viewer.GetLyricsMode() == Juna::Lyrics::LyricsViewer::LM_Text)
		Viewer.SetForceRedraw();
}
void __stdcall LyricsView_SetBGColor(unsigned char a,unsigned char r,unsigned char g,unsigned char b)
{
	Viewer.BGColor.Set(a,r,g,b);
	Viewer.SetForceRedraw();
}
void __stdcall LyricsView_SetBGImage(HBITMAP hbitmap)
{
	Viewer.SetForceRedraw();
	if (hbitmap == NULL)
	{
		Viewer.BGImage.SourceImage.Terminalize();
		return;
	}
	BITMAP bitmap;
	if (::GetObject(hbitmap , sizeof (BITMAP) , &bitmap) == 0)
	{
		Viewer.BGImage.SourceImage.Terminalize();
		return;
	}

	if (Viewer.BGImage.SourceImage.width() != bitmap.bmWidth || Viewer.BGImage.SourceImage.height() != bitmap.bmHeight)
	{
		Viewer.BGImage.SourceImage.Initialize(bitmap.bmWidth,bitmap.bmHeight);
	}
	HDC hdc = ::CreateCompatibleDC(HDC(Viewer.BGImage.SourceImage.GetDC()));
	HGDIOBJ hdefbm = ::SelectObject(hdc,hbitmap);
	::BitBlt(HDC(Viewer.BGImage.SourceImage.GetDC()),0,0,bitmap.bmWidth,bitmap.bmHeight,hdc,0,0,SRCCOPY);
	::SelectObject(hdc,hdefbm);
	::DeleteDC(hdc);
	Juna::Picture::FillColor(Viewer.BGImage.SourceImage,Juna::Picture::ARGB(255,0,0,0),Juna::Picture::ARGB(0,255,255,255));
	Viewer.BGImage.DisplayImage.Terminalize();
}

void __stdcall LyricsView_SetBGImagePosition(int position)
{
	Viewer.BGImage.Position = position;
	if (Viewer.BGImage.SourceImage.isValid())
		Viewer.SetForceRedraw();
}


void __stdcall LyricsView_SetBGImageFilter(unsigned char a,unsigned char r,unsigned char g,unsigned char b,
											int translucent_alpha)
{
	Viewer.BGImage.TranslucentAlpha = translucent_alpha;
	Viewer.BGImage.Filter.Set(a,r,g,b);
	if (Viewer.BGImage.SourceImage.isValid())
	{
		Viewer.SetForceRedraw();
		Viewer.BGImage.DisplayImage.Terminalize();
	}
}

void __stdcall LyricsView_SetBGImageLimitFlag(bool display_x,bool display_y,
											  bool source_x,bool source_y,
											  bool maxsize_x,bool maxsize_y)
{
	Viewer.BGImage.LimitDisplayX = display_x;
	Viewer.BGImage.LimitDisplayY = display_y;
	Viewer.BGImage.LimitSourceX = source_x;
	Viewer.BGImage.LimitSourceY = source_y;
	Viewer.BGImage.LimitMaxX = maxsize_x;
	Viewer.BGImage.LimitMaxY = maxsize_y;
	if (Viewer.BGImage.SourceImage.isValid())
		Viewer.SetForceRedraw();
}


void __stdcall LyricsView_SetBGImageMaxSize(int width,int height)
{
	if (width < 0 || height < 0)
		return;

	Viewer.BGImage.MaxX = width;
	Viewer.BGImage.MaxY = height;
	if (Viewer.BGImage.SourceImage.isValid())
		Viewer.SetForceRedraw();
}



void __stdcall LyricsView_SetLineBackColor(unsigned char a,unsigned char r,unsigned char g,unsigned char b)
{
	Viewer.Parameter.CurrentLineBackColor.Set(a,r,g,b);
	Viewer.SetForceRedraw();
}


void __stdcall LyricsView_SetLineBackPlusUp(int pluswidth)
{
	Viewer.Parameter.CurrentLineBackPlusUp = pluswidth;
	Viewer.SetForceRedraw();
}
void __stdcall LyricsView_SetLineBackPlusDown(int pluswidth)
{
	Viewer.Parameter.CurrentLineBackPlusDown = pluswidth;
	Viewer.SetForceRedraw();
}


void __stdcall LyricsView_SetScrollTime(int scroll_time,int fade_time)
{
	Viewer.Parameter.ScrollTime = scroll_time;
	Viewer.Parameter.FadeTime = fade_time;
	Viewer.SetForceRedraw();
}


void __stdcall LyricsView_SetVAlignment(int alignment,int y_offset)
{
	if (alignment < 0)
		Viewer.Parameter.VAlignment = Juna::Lyrics::VA_Bottom;
	else if (alignment > 0)
		Viewer.Parameter.VAlignment = Juna::Lyrics::VA_Top;
	else
		Viewer.Parameter.VAlignment = Juna::Lyrics::VA_Center;
	Viewer.Parameter.CurrentLineYOffset = y_offset;
	Viewer.SetForceRedraw();
}

void __stdcall LyricsView_SetHAlignment(int alignment)
{
	if (alignment < 0)
		Viewer.Parameter.HAlignment = Juna::Lyrics::HA_Right;
	else if (alignment > 0)
		Viewer.Parameter.HAlignment = Juna::Lyrics::HA_Left;
	else
		Viewer.Parameter.HAlignment = Juna::Lyrics::HA_Center;
	Viewer.SetForceRedraw();
}

void __stdcall LyricsView_SetSpace(int linespace,int charspace,int norubylinetopspace,int rubybottomspace)
{
	Viewer.Parameter.LineSpace = linespace;
	Viewer.Parameter.CharSpace = charspace;
	Viewer.Parameter.NoRubyLineTopSpace = norubylinetopspace;
	Viewer.Parameter.RubyBottomSpace = rubybottomspace;
	Viewer.SetForceRedraw();
}

void __stdcall LyricsView_SetRubyAlignment(int ruby_alignment)
{
	Viewer.Parameter.RAlignment = (Juna::Lyrics::RubyAlignment)ruby_alignment;
	Viewer.SetForceRedraw();
}


void __stdcall LyricsView_SetMargin(int left,int top,int right,int bottom)
{
	Viewer.Parameter.LeftMargin = left;
	Viewer.Parameter.TopMargin = top;
	Viewer.Parameter.RightMargin = right;
	Viewer.Parameter.BottomMargin = bottom;
	Viewer.SetForceRedraw();
}


	


bool __stdcall LyricsView_SetInfoTextFont(unsigned int fontquality,HFONT hfont,bool outline,int slip)
{
	Juna::Picture::OutlineFont font;
	if (!font.Initialize(hfont,fontquality))
		return false;

	Viewer.SetForceRedraw();
	return Viewer.InfoText.SetFont(fontquality,font,slip);
}
void __stdcall LyricsView_SetInfoTextColor(unsigned char r,unsigned char g,unsigned char b,
									   unsigned char or,unsigned char og,unsigned char ob)
{
	Viewer.InfoText.Color.Set(r,g,b);
	Viewer.InfoText.OutlineColor.Set(or,og,ob);
	Viewer.SetForceRedraw();
}
void __stdcall LyricsView_SetInfoTextAlignment(int valignment,int halignment)
{
	if (valignment < 0)
		Viewer.InfoText.Param.VAlignment = Juna::Lyrics::VA_Bottom;
	else if (valignment > 0)
		Viewer.InfoText.Param.VAlignment = Juna::Lyrics::VA_Top;
	else
		Viewer.InfoText.Param.VAlignment = Juna::Lyrics::VA_Center;

	if (halignment < 0)
		Viewer.InfoText.Param.HAlignment = Juna::Lyrics::HA_Right;
	else if (halignment > 0)
		Viewer.InfoText.Param.HAlignment = Juna::Lyrics::HA_Left;
	else
		Viewer.InfoText.Param.HAlignment = Juna::Lyrics::HA_Center;

	Viewer.SetForceRedraw();
}

void __stdcall LyricsView_SetInfoTextLayout(int left,int top,int right,int bottom)
{
	Viewer.TextLayoutRect.Set(left,top,right,bottom);
	Viewer.SetForceRedraw();
}

void __stdcall LyricsView_SetInfoTextString(const wchar_t *string)
{
	Viewer.InfoText.SetText(string);
	Viewer.SetForceRedraw();
}

void __stdcall LyricsView_SetFrameImage(HBITMAP hbitmap)
{
	Viewer.SetForceRedraw();
	if (hbitmap == NULL)
	{
		Viewer.FrameImage.Terminalize();
		return;
	}
	BITMAP bitmap;
	if (::GetObject(hbitmap , sizeof (BITMAP) , &bitmap) == 0)
	{
		Viewer.FrameImage.Terminalize();
		return;
	}

	if (Viewer.FrameImage.SourceImage.width() != bitmap.bmWidth || Viewer.FrameImage.SourceImage.height() != bitmap.bmHeight)
	{
		Viewer.FrameImage.SourceImage.Initialize(bitmap.bmWidth,bitmap.bmHeight);
	}
	HDC hdc = ::CreateCompatibleDC(HDC(Viewer.FrameImage.SourceImage.GetDC()));
	HGDIOBJ hdefbm = ::SelectObject(hdc,hbitmap);
	::BitBlt(HDC(Viewer.FrameImage.SourceImage.GetDC()),0,0,bitmap.bmWidth,bitmap.bmHeight,hdc,0,0,SRCCOPY);
	::SelectObject(hdc,hdefbm);
	::DeleteDC(hdc);
}

