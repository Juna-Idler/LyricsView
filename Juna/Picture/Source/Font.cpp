
#include "../Font.h"
#include "../Blit.h"

#include <windows.h>
#include <assert.h>


namespace Juna {
namespace Picture {

namespace {

struct Mat2 : public MAT2
{
	Mat2(void)
	{
		eM11.fract = eM12.fract = eM21.fract = eM22.fract = 0;
		eM12.value = eM21.value = 0;
		eM11.value = eM22.value = 1;
	}
};
const Mat2 UnitMat;

const UINT Format[4] = {GGO_BITMAP,GGO_GRAY2_BITMAP,GGO_GRAY4_BITMAP,GGO_GRAY8_BITMAP};


template<class TImage>
inline void ClipRect(Rect &dest,const TImage &dst,const Rect *clip_rect)
{

	Rect rect;
	if (clip_rect)
		rect = *clip_rect;
	else
		rect.Set(0,0,dst.width(),dst.height());

	dest.x1 = max(dest.x1,rect.x1);
	dest.y1 = max(dest.y1,rect.y1);
	dest.x2 = min(dest.x2,rect.x2);
	dest.y2 = min(dest.y2,rect.y2);
}


class OutlineBuffer
{
public:
	OutlineBuffer(unsigned long size) : Buffer(0),Size(0),Quality(-1)
		{Buffer = operator new(size);Size = size;}
	~OutlineBuffer() {operator delete(Buffer);}

	void resize(unsigned long size)
	{
		operator delete(Buffer);
		Buffer = 0;Size = 0;
		Buffer = operator new(size);Size = size;
	}

	void *Buffer;
	unsigned long Size;
	int Quality;
	GLYPHMETRICS gm;

	bool SetGlyphMetrics(HDC hdc,wchar_t c)
	{
		DWORD r = ::GetGlyphOutlineW(static_cast<HDC>(hdc),c,GGO_METRICS,&gm,0,NULL,&UnitMat);
		if (r == GDI_ERROR)
			return false;
		return true;
	}
	bool SetCharBitmap(HDC hdc,int quality,wchar_t c)
	{
		DWORD size = ::GetGlyphOutlineW(hdc,c,Format[quality],&gm,0,NULL,&UnitMat);
		if (size == GDI_ERROR)
			return false;
		if (Size < size)
			resize(size);
		if (::GetGlyphOutlineW(hdc,c,Format[quality],&gm,size,Buffer,&UnitMat) == GDI_ERROR)
			return false;
		if (size == 0)
		{
			gm.gmBlackBoxX = 0;
			gm.gmBlackBoxY = 0;
		}
		Quality = quality;
		return true;
	}
	bool MakeGrayImage(GrayImage &image,HDC hdc,int quality,wchar_t c)
	{
		if (!SetCharBitmap(hdc,quality,c))
			return false;
		image.Initialize(gm.gmBlackBoxX,gm.gmBlackBoxY);
		if (Quality == 0)
		{
			unsigned pitch = ((gm.gmBlackBoxX + 31) & ~31) >> 3;
			const BYTE *bp = static_cast<BYTE *>(Buffer);
			for (int i = 0;i < image.height();i++)
			{
				for (int j = 0;j < image.width();j++)
				{
					BYTE b = bp[j >> 3];
					int draw = ((b >> (7 - (j & 7))) & 1) * 255;
					image[i][j] = draw;
				}
				bp += pitch;
			}
		}
		else
		{
			unsigned pitch = (gm.gmBlackBoxX + 3) & ~3;
			const BYTE *bp = static_cast<BYTE *>(Buffer);
			for (int i = 0;i < image.height();i++)
			{
				for (int j = 0;j < image.width();j++)
				{
					BYTE p = bp[j];
					image[i][j] =
						(p << (8 - (Quality << 1))) - (p >> (Quality << 1));
				}
				bp += pitch;
			}
		}

		return true;
	}


	void DrawCharColor(I_Image &image,int dx,int dy,Color color,const Rect *clip_rect)
	{
		dx += gm.gmptGlyphOrigin.x;
		dy -= gm.gmptGlyphOrigin.y;

		Rect rect(dx,dy,dx + gm.gmBlackBoxX,dy + gm.gmBlackBoxY);
		ClipRect(rect,image,clip_rect);

		if (Quality == 0)
		{
			unsigned pitch = ((gm.gmBlackBoxX + 31) & ~31) >> 3;
			const BYTE *bp = static_cast<BYTE *>(Buffer) + pitch * (rect.y1 - dy);
			for (int i = rect.y1;i < rect.y2;i++)
			{
				for (int j = rect.x1;j < rect.x2;j++)
				{
					ARGB &dargb = image[i][j];
					BYTE b = bp[(j-dx) >> 3];
					int draw = ((b >> (7 - ((j-dx) & 7))) & 1);
					dargb.r = (dargb.r * !draw) + (color.r * draw);
					dargb.g = (dargb.g * !draw) + (color.g * draw);
					dargb.b = (dargb.b * !draw) + (color.b * draw);
				}
				bp += pitch;
			}
		}
		else
		{
			unsigned pitch = (gm.gmBlackBoxX + 3) & ~3;
			const BYTE *bp = static_cast<BYTE *>(Buffer) + pitch * (rect.y1 - dy);
			unsigned shift = (8 - (Quality << 1));
			for (int i = rect.y1;i < rect.y2;i++)
			{
				for (int j = rect.x1;j < rect.x2;j++)
				{
					ARGB &dargb = image[i][j];
					BYTE p = bp[j-dx];
					unsigned rate = (p << shift);
					dargb.r = (dargb.r * (256 - rate) >> 8) + (color.r * rate >> 8);
					dargb.g = (dargb.g * (256 - rate) >> 8) + (color.g * rate >> 8);
					dargb.b = (dargb.b * (256 - rate) >> 8) + (color.b * rate >> 8);
				}
				bp += pitch;
			}
		}
	}

	void DrawCharPreMultipliedAlpha(I_Image &image,int dx,int dy,Color color,const Rect *clip_rect)
	{
		dx += gm.gmptGlyphOrigin.x;
		dy -= gm.gmptGlyphOrigin.y;

		Rect rect(dx,dy,dx + gm.gmBlackBoxX,dy + gm.gmBlackBoxY);
		ClipRect(rect,image,clip_rect);

		if (Quality == 0)
		{
			unsigned pitch = ((gm.gmBlackBoxX + 31) & ~31) >> 3;
			const BYTE *bp = static_cast<BYTE *>(Buffer) + pitch * (rect.y1 - dy);
			for (int i = rect.y1;i < rect.y2;i++)
			{
				for (int j = rect.x1;j < rect.x2;j++)
				{
					ARGB &dargb = image[i][j];
					BYTE b = bp[(j-dx) >> 3];
					int draw = ((b >> (7 - ((j-dx) & 7))) & 1);
					dargb.r = (dargb.r * !draw) + (color.r * draw);
					dargb.g = (dargb.g * !draw) + (color.g * draw);
					dargb.b = (dargb.b * !draw) + (color.b * draw);
					dargb.a = (dargb.a * !draw) + (draw);
				}
				bp += pitch;
			}
		}
		else
		{
			unsigned pitch = (gm.gmBlackBoxX + 3) & ~3;
			const BYTE *bp = static_cast<BYTE *>(Buffer) + pitch * (rect.y1 - dy);
			unsigned shift = (8 - (Quality << 1));
			for (int i = rect.y1;i < rect.y2;i++)
			{
				for (int j = rect.x1;j < rect.x2;j++)
				{
					ARGB &dargb = image[i][j];
					BYTE p = bp[j-dx];
					unsigned rate = (p << shift);
					dargb.r = (dargb.r * (256 - rate) >> 8) + (color.r * rate >> 8);
					dargb.g = (dargb.g * (256 - rate) >> 8) + (color.g * rate >> 8);
					dargb.b = (dargb.b * (256 - rate) >> 8) + (color.b * rate >> 8);
					dargb.a = (dargb.a * (256 - rate) >> 8) + (255 * rate >> 8);
				}
				bp += pitch;
			}
		}
	}


	void DrawCharAlpha(I_Image &image,int dx,int dy,const Rect *clip_rect)
	{
		dx += gm.gmptGlyphOrigin.x;
		dy -= gm.gmptGlyphOrigin.y;

		Rect rect(dx,dy,dx + gm.gmBlackBoxX,dy + gm.gmBlackBoxY);
		ClipRect(rect,image,clip_rect);

		if (Quality == 0)
		{
			unsigned pitch = ((gm.gmBlackBoxX + 31) & ~31) >> 3;
			const BYTE *bp = static_cast<BYTE *>(Buffer) + pitch * (rect.y1 - dy);
			for (int i = rect.y1;i < rect.y2;i++)
			{
				for (int j = rect.x1;j < rect.x2;j++)
				{
					BYTE b = bp[(j-dx) >> 3];
					int draw = ((b >> (7 - ((j-dx) & 7))) & 1) * 255;
					ARGB &dargb = image[i][j];
					dargb.a = (dargb.a * !draw) + (draw);
				}
				bp += pitch;
			}
		}
		else
		{
			unsigned pitch = (gm.gmBlackBoxX + 3) & ~3;
			const BYTE *bp = static_cast<BYTE *>(Buffer) + pitch * (rect.y1 - dy);
			unsigned shift = (8 - (Quality << 1));
			for (int i = rect.y1;i < rect.y2;i++)
			{
				for (int j = rect.x1;j < rect.x2;j++)
				{
					BYTE p = bp[j-dx];
					unsigned rate = (p << shift);
					ARGB &dargb = image[i][j];
					dargb.a = (dargb.a * (256 - rate) >> 8) + (255 * rate >> 8);
				}
				bp += pitch;
			}
		}
	}
/*
	void DrawCharStraightAlpha(I_Image &image,int dx,int dy,Color color,const Rect *clip_rect)
	{
		ARGB argb(color);
		dx += gm.gmptGlyphOrigin.x;
		dy -= gm.gmptGlyphOrigin.y;

		Rect rect(dx,dy,dx + gm.gmBlackBoxX,dy + gm.gmBlackBoxY);
		ClipRect(rect,image,clip_rect);

		if (Quality == 0)
		{
			unsigned pitch = ((gm.gmBlackBoxX + 31) & ~31) >> 3;
			const BYTE *bp = static_cast<BYTE *>(Buffer) + pitch * (rect.y1 - dy);
			for (int i = rect.y1;i < rect.y2;i++)
			{
				for (int j = rect.x1;j < rect.x2;j++)
				{
					BYTE b = bp[(j-dx) >> 3];
					int draw = ((b >> (7 - ((j-dx) & 7))) & 1) * 255;
					ARGB &dargb = image[i][j];

					dargb.r = (dargb.r * !draw) + (color.r * draw);
					dargb.g = (dargb.g * !draw) + (color.g * draw);
					dargb.b = (dargb.b * !draw) + (color.b * draw);
					dargb.a = (dargb.a * !draw) + (draw);
					image[i][j] = dargb;
				}
				bp += pitch;
			}
		}
		else
		{
			unsigned pitch = (gm.gmBlackBoxX + 3) & ~3;
			const BYTE *bp = static_cast<BYTE *>(Buffer) + pitch * (rect.y1 - dy);
			unsigned shift = (8 - (Quality << 1));
			for (int i = rect.y1;i < rect.y2;i++)
			{
				for (int j = rect.x1;j < rect.x2;j++)
				{
					BYTE p = bp[j-dx];
					ARGB &dargb = image[i][j];
					unsigned rate = (p << shift);
					dargb.a = (dargb.a * (256 - rate) >> 8) + (255 * rate >> 8);
					if (dargb.a > 0)
					{
						byte da = dargb.a + 1;
						dargb.r = ((dargb.r * da * (256 - rate) >> 8) + (color.r * rate)) / da;
						dargb.g = ((dargb.g * da * (256 - rate) >> 8) + (color.g * rate)) / da;
						dargb.b = ((dargb.b * da * (256 - rate) >> 8) + (color.b * rate)) / da;
					}
				}
				bp += pitch;
			}
		}
	}
*/
	void DrawCharAlpha(I_GrayImage &image,int dx,int dy,const Rect *clip_rect)
	{
		dx += gm.gmptGlyphOrigin.x;
		dy -= gm.gmptGlyphOrigin.y;
		Rect rect(dx,dy,dx + gm.gmBlackBoxX,dy + gm.gmBlackBoxY);
		ClipRect(rect,image,clip_rect);

		if (Quality == 0)
		{
			unsigned pitch = ((gm.gmBlackBoxX + 31) & ~31) >> 3;
			const BYTE *bp = static_cast<BYTE *>(Buffer) + pitch * (rect.y1 - dy);
			for (int i = rect.y1;i < rect.y2;i++)
			{
				for (int j = rect.x1;j < rect.x2;j++)
				{
					BYTE b = bp[(j-dx) >> 3];
					int draw = ((b >> (7 - ((j-dx) & 7))) & 1) * 255;
					image[i][j] = draw;
				}
				bp += pitch;
			}
		}
		else
		{
			unsigned pitch = (gm.gmBlackBoxX + 3) & ~3;
			const BYTE *bp = static_cast<BYTE *>(Buffer) + pitch * (rect.y1 - dy);
			for (int i = rect.y1;i < rect.y2;i++)
			{
				for (int j = rect.x1;j < rect.x2;j++)
				{
					BYTE p = bp[j-dx];
					image[i][j] =
						(p << (8 - (Quality << 1))) - (p >> (Quality << 1));
				}
				bp += pitch;
			}
		}
	}

};



OutlineBuffer outlineBuffer(0);


}//anonymous namespace


bool Font::Initialize(long height,const InitParam &param,unsigned quality)
{
	Terminalize();
	hFont = ::CreateFontW(
		height,param.width,0,0,param.weight,FALSE,FALSE,FALSE,
		(DWORD)param.charset,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		(unsigned)param.pitch | ((unsigned)param.family >> 4), param.facename);
	if (hFont == 0)
		return false;

	hDC = ::CreateCompatibleDC(NULL);
	hDCFont = ::SelectObject(static_cast<HDC>(hDC),hFont);

	TEXTMETRIC tm;
	::GetTextMetrics(static_cast<HDC>(hDC),&tm);
	tmAscent = tm.tmAscent;
	tmHeight = tm.tmHeight;

	SetQuality(quality);
	return true;
}

bool Font::Initialize(void *font_handle,unsigned quality)
{
	Terminalize();
	if (font_handle == 0)
		return false;

	::LOGFONTW logfont;
	if (!::GetObjectW(font_handle, sizeof(LOGFONT), &logfont))
		return false;

	hFont = ::CreateFontIndirectW(&logfont);
	if (hFont == 0)
		return false;

	hDC = ::CreateCompatibleDC(NULL);
	hDCFont = ::SelectObject(static_cast<HDC>(hDC),hFont);

	TEXTMETRIC tm;
	::GetTextMetrics(static_cast<HDC>(hDC),&tm);
	tmAscent = tm.tmAscent;
	tmHeight = tm.tmHeight;

	SetQuality(quality);

	return true;
}


bool Font::Initialize(const Font &source,long height,long width)
{
	if (!source.IsValid())
	{
		Terminalize();
		return false;
	}

	::LOGFONTW logfont;
	if (!::GetObjectW(source.hFont, sizeof(LOGFONT), &logfont))
	{
		Terminalize();
		return false;
	}
	Terminalize();

	logfont.lfHeight = height;
	logfont.lfWidth = width;

	hFont = ::CreateFontIndirectW(&logfont);
	if (hFont == 0)
		return false;

	hDC = ::CreateCompatibleDC(NULL);
	hDCFont = ::SelectObject(static_cast<HDC>(hDC),hFont);

	TEXTMETRIC tm;
	::GetTextMetrics(static_cast<HDC>(hDC),&tm);
	tmAscent = tm.tmAscent;
	tmHeight = tm.tmHeight;

	SetQuality(source.Quality);

	return true;
}



void Font::Terminalize(void)
{
	if (hFont)
	{
		if (hDC)
		{
			::SelectObject(static_cast<HDC>(hDC),hDCFont);
		}
		::DeleteObject(hFont);
		hFont = 0;
	}
	if (hDC)
	{
		hDCFont = 0;
		::DeleteDC(static_cast<HDC>(hDC));
		hDC = 0;
	}
//	Quality = 0;

	tmHeight = 0;
	tmAscent = 0;
}

short Font::GetCharCellIncX(wchar_t c) const
{
	if (!outlineBuffer.SetGlyphMetrics(static_cast<HDC>(hDC),c))
		return CellIncError;
	return outlineBuffer.gm.gmCellIncX;
}
short Font::GetFirstCharCellIncX(wchar_t c) const
{
	if (!outlineBuffer.SetGlyphMetrics(static_cast<HDC>(hDC),c))
		return CellIncError;
	if (outlineBuffer.gm.gmptGlyphOrigin.x < 0)
		return outlineBuffer.gm.gmCellIncX + short(-outlineBuffer.gm.gmptGlyphOrigin.x);
	return outlineBuffer.gm.gmCellIncX;
}


short Font::DrawCharColor(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect) const
{
	if (!outlineBuffer.SetCharBitmap(static_cast<HDC>(hDC),Quality,c))
		return CellIncError;
	outlineBuffer.DrawCharColor( target, x, y + tmAscent, color,clip_rect);
	return outlineBuffer.gm.gmCellIncX;
}

short Font::DrawCharAlpha(I_Image &target,int x,int y,wchar_t c,const Rect *clip_rect) const
{
	if (!outlineBuffer.SetCharBitmap(static_cast<HDC>(hDC),Quality,c))
		return CellIncError;
	outlineBuffer.DrawCharAlpha(target,x,y + tmAscent,clip_rect);
	return outlineBuffer.gm.gmCellIncX;
}

short Font::DrawCharPreMultipliedAlpha(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect) const
{
	if (!outlineBuffer.SetCharBitmap(static_cast<HDC>(hDC),Quality,c))
		return CellIncError;
	outlineBuffer.DrawCharPreMultipliedAlpha(target,x,y + tmAscent,color,clip_rect);
	return outlineBuffer.gm.gmCellIncX;
}


short Font::DrawCharAlpha(I_GrayImage &target,int x,int y,wchar_t c,const Rect *clip_rect) const
{
	if (!outlineBuffer.SetCharBitmap(static_cast<HDC>(hDC),Quality,c))
		return CellIncError;
	outlineBuffer.DrawCharAlpha(target,x,y + tmAscent,clip_rect);
	return outlineBuffer.gm.gmCellIncX;
}


short Font::DrawFirstCharColor(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect) const
{
	if (!outlineBuffer.SetCharBitmap(static_cast<HDC>(hDC),Quality,c))
		return CellIncError;
	if (outlineBuffer.gm.gmptGlyphOrigin.x)
	{
		outlineBuffer.gm.gmCellIncX += short(-outlineBuffer.gm.gmptGlyphOrigin.x);
		outlineBuffer.gm.gmptGlyphOrigin.x = 0;
	}
	outlineBuffer.DrawCharColor( target, x, y + tmAscent, color,clip_rect);
	return outlineBuffer.gm.gmCellIncX;
}

short Font::DrawFirstCharAlpha(I_Image &target,int x,int y,wchar_t c,const Rect *clip_rect) const
{
	if (!outlineBuffer.SetCharBitmap(static_cast<HDC>(hDC),Quality,c))
		return CellIncError;
	if (outlineBuffer.gm.gmptGlyphOrigin.x)
	{
		outlineBuffer.gm.gmCellIncX += short(-outlineBuffer.gm.gmptGlyphOrigin.x);
		outlineBuffer.gm.gmptGlyphOrigin.x = 0;
	}
	outlineBuffer.DrawCharAlpha(target,x,y + tmAscent,clip_rect);
	return outlineBuffer.gm.gmCellIncX;
}

short Font::DrawFirstPreMultipliedAlpha(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect) const
{
	if (!outlineBuffer.SetCharBitmap(static_cast<HDC>(hDC),Quality,c))
		return CellIncError;
	if (outlineBuffer.gm.gmptGlyphOrigin.x)
	{
		outlineBuffer.gm.gmCellIncX += short(-outlineBuffer.gm.gmptGlyphOrigin.x);
		outlineBuffer.gm.gmptGlyphOrigin.x = 0;
	}
	outlineBuffer.DrawCharPreMultipliedAlpha(target,x,y + tmAscent,color,clip_rect);
	return outlineBuffer.gm.gmCellIncX;
}

short Font::DrawFirstCharAlpha(I_GrayImage &target,int x,int y,wchar_t c,const Rect *clip_rect) const
{
	if (!outlineBuffer.SetCharBitmap(static_cast<HDC>(hDC),Quality,c))
		return CellIncError;
	if (outlineBuffer.gm.gmptGlyphOrigin.x)
	{
		outlineBuffer.gm.gmCellIncX += short(-outlineBuffer.gm.gmptGlyphOrigin.x);
		outlineBuffer.gm.gmptGlyphOrigin.x = 0;
	}
	outlineBuffer.DrawCharAlpha(target,x,y + tmAscent,clip_rect);
	return outlineBuffer.gm.gmCellIncX;
}



bool Font::GetFontImage(FontImage &image,wchar_t c) const
{
	if (!outlineBuffer.MakeGrayImage(image.Image,static_cast<HDC>(hDC),Quality,c))
		return false;
	image.xoffset = outlineBuffer.gm.gmptGlyphOrigin.x;
	image.yoffset = tmAscent - outlineBuffer.gm.gmptGlyphOrigin.y;
	image.CellIncX = outlineBuffer.gm.gmCellIncX;
	return true;
}


void Font::Swap(Font &other)
{
	void *tmp = this->hFont;
	this->hFont = other.hFont;
	other.hFont = tmp;
		
	int q = this->Quality;
	this->Quality = other.Quality;
	other.Quality = q;

	tmp = this->hDC;
	this->hDC = other.hDC;
	other.hDC = tmp;

	tmp = this->hDCFont;
	this->hDCFont = other.hDCFont;
	other.hDCFont = tmp;

	long l = this->tmHeight;
	this->tmHeight = other.tmHeight;
	other.tmHeight = l;

	l = this->tmAscent;
	this->tmAscent = other.tmAscent;
	other.tmAscent = l;
}






bool BufferedFont::Initialize(Font &font)
{
	Terminalize();
	if (!font.IsValid())
		return false;

	BaseFont.Swap(font);
	return true;
}

void BufferedFont::Terminalize(void)
{
	BaseFont.Terminalize();

	for (map::iterator it = Buffer->begin();it != Buffer->end();++it)
	{
		delete it->second;
	}
	Buffer->clear();
}

void BufferedFont::SetQuality(unsigned quality)
{
	if (BaseFont.GetQuality() == quality)
		return;
	BaseFont.SetQuality(quality);
	Buffer->clear();
}

short BufferedFont::GetCharCellIncX(wchar_t c) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		return BaseFont.GetCharCellIncX(c);
	}
	return it->second->CellIncX;
}
short BufferedFont::GetFirstCharCellIncX(wchar_t c) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		return BaseFont.GetFirstCharCellIncX(c);
	}
	FontImage &image = *it->second;
	if (image.xoffset < 0)
	{
		return image.CellIncX + -image.xoffset;
	}
	return image.CellIncX;
}


short BufferedFont::DrawCharColor(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
			return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + image.xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2ImageColor(target,image.Image,color,dx,dy,&rect);
	return image.CellIncX;
}
short BufferedFont::DrawCharAlpha(I_Image &target,int x,int y,wchar_t c,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
			return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + image.xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2ImageAlpha(target,image.Image,dx,dy,&rect);
	return image.CellIncX;
}
short BufferedFont::DrawCharPreMultipliedAlpha(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
			return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + image.xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2ImagePreMultipliedAlpha(target,image.Image,color,dx,dy,&rect);
	return image.CellIncX;
}

short BufferedFont::DrawCharAlpha(I_GrayImage &target,int x,int y,wchar_t c,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
			return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + image.xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2GrayBlit(target,image.Image,dx,dy,&rect);
	return image.CellIncX;
}

bool BufferedFont::SetBuffer(wchar_t c) const
{
	FontImage *image = new FontImage();
	if (!BaseFont.GetFontImage(*image,c))
	{
		delete image;
		return false;
	}
	(*Buffer)[c] = image;

	return true;
}

short BufferedFont::DrawFirstCharColor(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
			return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;
	int xoffset = image.xoffset;
	int cellincx = image.CellIncX;
	if (xoffset < 0)
	{
		cellincx += -xoffset;
		xoffset = 0;
	}

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2ImageColor(target,image.Image,color,dx,dy,&rect);
	return cellincx;
}
short BufferedFont::DrawFirstCharAlpha(I_Image &target,int x,int y,wchar_t c,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
			return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;
	int xoffset = image.xoffset;
	int cellincx = image.CellIncX;
	if (xoffset < 0)
	{
		cellincx += -xoffset;
		xoffset = 0;
	}

	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2ImageAlpha(target,image.Image,dx,dy,&rect);
	return cellincx;
}

short BufferedFont::DrawFirstCharPreMultipliedAlpha(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
			return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;
	int xoffset = image.xoffset;
	int cellincx = image.CellIncX;
	if (xoffset < 0)
	{
		cellincx += -xoffset;
		xoffset = 0;
	}
	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2ImagePreMultipliedAlpha(target,image.Image,color,dx,dy,&rect);
	return cellincx;
}

short BufferedFont::DrawFirstCharAlpha(I_GrayImage &target,int x,int y,wchar_t c,const Rect *clip_rect) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
			return CellIncError;
		it = Buffer->find(c);
	}
	FontImage &image = *it->second;
	int xoffset = image.xoffset;
	int cellincx = image.CellIncX;
	if (xoffset < 0)
	{
		cellincx += -xoffset;
		xoffset = 0;
	}
	Rect rect;
	int dx,dy;
	ClipSourceRect(rect,dx,dy,x + xoffset,y + image.yoffset,image.Image,clip_rect);

	Gray2GrayBlit(target,image.Image,dx,dy,&rect);
	return cellincx;
}




const FontImage *BufferedFont::GetFontImage(wchar_t c) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		if (!SetBuffer(c))
			return 0;
		it = Buffer->find(c);
	}
	return it->second;
}


}//namespace Picture
}//namespace Juna

