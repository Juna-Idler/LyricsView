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



template<class TImage>
inline void ClipRect(Rect& dest, const TImage& dst, const Rect* clip_rect)
{
	Rect rect;
	if (clip_rect)
		rect = *clip_rect;
	else
		rect.Set(0, 0, dst.width(), dst.height());

	dest.x1 = max(dest.x1, rect.x1);
	dest.y1 = max(dest.y1, rect.y1);
	dest.x2 = min(dest.x2, rect.x2);
	dest.y2 = min(dest.y2, rect.y2);
}


class BinaryDIB
{
public:
	BinaryDIB(void) : hDC(0), hBitmap(0),hBrush(0),Buffer(0),Width(0),Height(0),Distance(0) {}
	~BinaryDIB() { Terminalize(); }

	struct BITMAPINFO2
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD          bmiColors[2];
	};

	HBITMAP CreateDIBitmap(int width, int height)
	{
		BITMAPINFO2 bi = {};

		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth = width;
		bi.bmiHeader.biHeight = -height;
		bi.bmiHeader.biBitCount = 1;
		bi.bmiHeader.biSizeImage = 0;
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biCompression = BI_RGB;

		bi.bmiColors[0] = { 0,0,0,0 };
		bi.bmiColors[1] = { 255,255,255,0 };

		void* mem;
		HBITMAP hbmp = ::CreateDIBSection(NULL, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, &mem, NULL, 0);
		if (hbmp == NULL)
			return NULL;
		Buffer = static_cast<BYTE*>(mem);
		return hbmp;
	}

	bool Initialize(int width, int height)
	{
		Terminalize();

		HBITMAP hbmp = CreateDIBitmap(width, height);
		if (!hbmp)
			return false;

		hDC = ::CreateCompatibleDC(NULL);
		if (hDC == NULL)
		{
			::DeleteObject(hbmp);
			Terminalize();
			return false;
		}
		hBitmap = static_cast<HBITMAP>(::SelectObject(hDC, hbmp));
		if (hBitmap == NULL)
		{
			::DeleteObject(hbmp);
			Terminalize();
			return false;
		}

		HBRUSH hbrush = (HBRUSH)::CreateSolidBrush(RGB(255, 255, 255));
		hBrush = (HBRUSH)::SelectObject(hDC, hbrush);

		Width = width;
		Height = height;
		Distance = (width + 31) / 32 * 4;

		return true;
	}
	void Terminalize(void)
	{
		if (hDC)
		{
			if (hBrush)
			{
				::DeleteObject(::SelectObject(hDC, hBrush));
				hBrush = NULL;
			}
			if (hBitmap)
			{
				::DeleteObject(::SelectObject(hDC, hBitmap));
				hBitmap = NULL;
			}
			::DeleteDC(hDC);
			hDC = NULL;
		}
		Buffer = 0;
		Width = Height = Distance = 0;
	}

	bool Resize(int width, int height)
	{
		if (!hDC)
			return Initialize(width, height);

		HBITMAP hbmp = CreateDIBitmap(width, height);
		if (!hbmp)
			return false;

		::DeleteObject(::SelectObject(hDC, hbmp));

		Width = width;
		Height = height;
		Distance = (width + 31) / 32 * 4;

		return true;

	}

	void Clear()
	{
		ZeroMemory(Buffer, Distance * Height);
	}


	void Shrink2GrayImage(GrayImage& image, int quality)
	{
		switch (quality)
		{
		case 0:
			for (int i = 0; i < image.height(); i++)
			{
				for (int j = 0; j < image.width(); j++)
				{
					image[i][j] = GetPixel(j, i) * 255;
				}
			}
			break;
		case 1:
			for (int i = 0; i < image.height(); i++)
			{
				for (int j = 0; j < image.width() / 4; j++)
				{
					BYTE p1 = (*this)[i * 2][j];
					BYTE p2 = (*this)[i * 2 + 1][j];
					image[i][j * 4] = (((p1 >> 7) & 1) + ((p2 >> 7) & 1) + ((p1 >> 6) & 1) + ((p2 >> 6) & 1)) * 255 / 4;
					image[i][j * 4 + 1] = (((p1 >> 5) & 1) + ((p2 >> 5) & 1) + ((p1 >> 4) & 1) + ((p2 >> 4) & 1)) * 255 / 4;
					image[i][j * 4 + 2] = (((p1 >> 3) & 1) + ((p2 >> 3) & 1) + ((p1 >> 2) & 1) + ((p2 >> 2) & 1)) * 255 / 4;
					image[i][j * 4 + 3] = (((p1 >> 1) & 1) + ((p2 >> 1) & 1) + (p1 & 1) + (p2 & 1)) * 255 / 4;
				}
				if (image.width() & 3)
				{
					int j = (image.width() / 4) * 4;
					DWORD p1 = (*this)[i * 2][j / 4];
					DWORD p2 = (*this)[i * 2 + 1][j / 4];

					image[i][j] = (((p1 >> 7) & 1) + ((p2 >> 7) & 1) + ((p1 >> 6) & 1) + ((p2 >> 6) & 1)) * 255 / 4;
					if (++j < image.width())
						image[i][j] = (((p1 >> 5) & 1) + ((p2 >> 5) & 1) + ((p1 >> 4) & 1) + ((p2 >> 4) & 1)) * 255 / 4;
					if (++j < image.width())
						image[i][j] = (((p1 >> 3) & 1) + ((p2 >> 3) & 1) + ((p1 >> 2) & 1) + ((p2 >> 2) & 1)) * 255 / 4;
				}
			}
			break;
		case 2:
			for (int i = 0; i < image.height(); i++)
			{
				for (int j = 0; j < image.width() / 2; j++)
				{
					BYTE p1 = (*this)[i * 4][j];
					BYTE p2 = (*this)[i * 4 + 1][j];
					BYTE p3 = (*this)[i * 4 + 2][j];
					BYTE p4 = (*this)[i * 4 + 3][j];
					image[i][j * 2] = (
						(((p1 >> 7) & 1) + ((p1 >> 6) & 1) + ((p1 >> 5) & 1) + ((p1 >> 4) & 1)) +
						(((p2 >> 7) & 1) + ((p2 >> 6) & 1) + ((p2 >> 5) & 1) + ((p2 >> 4) & 1)) +
						(((p3 >> 7) & 1) + ((p3 >> 6) & 1) + ((p3 >> 5) & 1) + ((p3 >> 4) & 1)) +
						(((p4 >> 7) & 1) + ((p4 >> 6) & 1) + ((p4 >> 5) & 1) + ((p4 >> 4) & 1))
						) * 255 / 16;

					image[i][j * 2 + 1] = (
						(((p1 >> 3) & 1) + ((p1 >> 2) & 1) + ((p1 >> 1) & 1) + (p1 & 1)) +
						(((p2 >> 3) & 1) + ((p2 >> 2) & 1) + ((p2 >> 1) & 1) + (p2 & 1)) +
						(((p3 >> 3) & 1) + ((p3 >> 2) & 1) + ((p3 >> 1) & 1) + (p3 & 1)) +
						(((p4 >> 3) & 1) + ((p4 >> 2) & 1) + ((p4 >> 1) & 1) + (p4 & 1))
						) * 255 / 16;
				}
				if (image.width() & 1)
				{
					const int j = (image.width() / 2);
					DWORD p1 = (*this)[i * 4][j];
					DWORD p2 = (*this)[i * 4 + 1][j];
					DWORD p3 = (*this)[i * 4 + 3][j];
					DWORD p4 = (*this)[i * 4 + 4][j];
					image[i][j * 2] = (
						(((p1 >> 7) & 1) + ((p1 >> 6) & 1) + ((p1 >> 5) & 1) + ((p1 >> 4) & 1)) +
						(((p2 >> 7) & 1) + ((p2 >> 6) & 1) + ((p2 >> 5) & 1) + ((p2 >> 4) & 1)) +
						(((p3 >> 7) & 1) + ((p3 >> 6) & 1) + ((p3 >> 5) & 1) + ((p3 >> 4) & 1)) +
						(((p4 >> 7) & 1) + ((p4 >> 6) & 1) + ((p4 >> 5) & 1) + ((p4 >> 4) & 1))
						) * 255 / 16;
				}
			}
			break;
		case 3:
			for (int i = 0; i < image.height(); i++)
			{
				for (int j = 0; j < image.width(); j++)
				{
					BYTE p1 = (*this)[i * 8][j];
					BYTE p2 = (*this)[i * 8 + 1][j];
					BYTE p3 = (*this)[i * 8 + 2][j];
					BYTE p4 = (*this)[i * 8 + 3][j];
					BYTE p5 = (*this)[i * 8 + 4][j];
					BYTE p6 = (*this)[i * 8 + 5][j];
					BYTE p7 = (*this)[i * 8 + 6][j];
					BYTE p8 = (*this)[i * 8 + 7][j];

					image[i][j] = (
						(((p1 >> 7) & 1) + ((p1 >> 6) & 1) + ((p1 >> 5) & 1) + ((p1 >> 4) & 1) + ((p1 >> 3) & 1) + ((p1 >> 2) & 1) + ((p1 >> 1) & 1) + (p1 & 1)) +
						(((p2 >> 7) & 1) + ((p2 >> 6) & 1) + ((p2 >> 5) & 1) + ((p2 >> 4) & 1) + ((p2 >> 3) & 1) + ((p2 >> 2) & 1) + ((p2 >> 1) & 1) + (p2 & 1)) +
						(((p3 >> 7) & 1) + ((p3 >> 6) & 1) + ((p3 >> 5) & 1) + ((p3 >> 4) & 1) + ((p3 >> 3) & 1) + ((p3 >> 2) & 1) + ((p3 >> 1) & 1) + (p3 & 1)) +
						(((p4 >> 7) & 1) + ((p4 >> 6) & 1) + ((p4 >> 5) & 1) + ((p4 >> 4) & 1) + ((p4 >> 3) & 1) + ((p4 >> 2) & 1) + ((p4 >> 1) & 1) + (p4 & 1)) +
						(((p5 >> 7) & 1) + ((p5 >> 6) & 1) + ((p5 >> 5) & 1) + ((p5 >> 4) & 1) + ((p5 >> 3) & 1) + ((p5 >> 2) & 1) + ((p5 >> 1) & 1) + (p5 & 1)) +
						(((p6 >> 7) & 1) + ((p6 >> 6) & 1) + ((p6 >> 5) & 1) + ((p6 >> 4) & 1) + ((p6 >> 3) & 1) + ((p6 >> 2) & 1) + ((p6 >> 1) & 1) + (p6 & 1)) +
						(((p7 >> 7) & 1) + ((p7 >> 6) & 1) + ((p7 >> 5) & 1) + ((p7 >> 4) & 1) + ((p7 >> 3) & 1) + ((p7 >> 2) & 1) + ((p7 >> 1) & 1) + (p7 & 1)) +
						(((p8 >> 7) & 1) + ((p8 >> 6) & 1) + ((p8 >> 5) & 1) + ((p8 >> 4) & 1) + ((p8 >> 3) & 1) + ((p8 >> 2) & 1) + ((p8 >> 1) & 1) + (p8 & 1))
						) * 255 / 64;
				}
			}
			break;
		}

	}


	BYTE* operator[](int y) { return Buffer + Distance * y; }
	const BYTE* operator[](int y) const { return Buffer + Distance * y; }
	bool GetPixel(int x, int y) const { return (Buffer[Distance * y + (x >> 3)] >> (7 - (x & 7))) & 1; }

	BYTE* Buffer;	//âÊëfîzóÒ
	int Width;		//ïù
	int Height;		//çÇÇ≥
	int Distance;	//éüÇÃÉâÉCÉìÇ÷ÇÃãóó£



	HDC hDC;
	HBITMAP hBitmap;

	HBRUSH hBrush;

};




class OutlineBuffer
{
public:
	OutlineBuffer(unsigned long size) : DIB(), gm(),tm()
	{
		DIB.Initialize(1, 1);
		::SetBkMode(DIB.hDC, TRANSPARENT);
	}
	~OutlineBuffer() { DIB.Terminalize(); }

	BinaryDIB DIB;

	GLYPHMETRICS gm;
	TEXTMETRIC tm;

	bool SetGlyphMetrics(HDC hdc, wchar_t c)
	{
		DWORD r = ::GetGlyphOutlineW(hdc, c, GGO_METRICS, &gm, 0, NULL, &UnitMat);
		if (r == GDI_ERROR)
			return false;
		return true;
	}
	bool SetGlyphMetrics(HFONT hfont, wchar_t c)
	{
		HFONT oldfont = static_cast<HFONT>(::SelectObject(DIB.hDC, hfont));
		DWORD r = ::GetGlyphOutlineW(DIB.hDC, c, GGO_METRICS, &gm, 0, NULL, &UnitMat);
		::SelectObject(DIB.hDC, oldfont);
		if (r == GDI_ERROR)
			return false;
		return true;
	}


	void SetTextMetrics(HFONT hfont)
	{
		HFONT oldfont = static_cast<HFONT>(::SelectObject(DIB.hDC, hfont));
		::GetTextMetrics(DIB.hDC, &tm);
		::SelectObject(DIB.hDC, oldfont);
	}

public:
	bool MakeGrayImage(GrayImage& image, HFONT hfont, int quality, wchar_t c)
	{
		HFONT oldfont = static_cast<HFONT>(::SelectObject(DIB.hDC, hfont));
		::GetTextMetrics(DIB.hDC, &tm);
		DWORD r = ::GetGlyphOutlineW(DIB.hDC, c, GGO_METRICS, &gm, 0, NULL, &UnitMat);
		if (r == GDI_ERROR)
		{
			::SelectObject(DIB.hDC,oldfont);
			return false;
		}
		int bbx = ((gm.gmBlackBoxX + (1 << quality) - 1) >> quality) << quality;
		int bby = ((gm.gmBlackBoxY + (1 << quality) - 1) >> quality) << quality;
		if (DIB.Width < bbx || DIB.Height < bby)
		{
			if (!DIB.Resize(max(DIB.Width, bbx), max(DIB.Height, bby)))
			{
				::SelectObject(DIB.hDC, oldfont);
				return false;
			}
		}
		DIB.Clear();

		::BeginPath(DIB.hDC);
		::TextOutW(DIB.hDC, -gm.gmptGlyphOrigin.x, -(tm.tmAscent - gm.gmptGlyphOrigin.y), &c, 1);
		::EndPath(DIB.hDC);
		::FillPath(DIB.hDC);
		::SelectObject(DIB.hDC, oldfont);

		image.Initialize(bbx >> quality, bby >> quality);

		DIB.Shrink2GrayImage(image, quality);

		return true;
	}
	bool MakeOutlineGrayImage(GrayImage& image, HFONT hfont, int quality, wchar_t c,int thickness)
	{
		HFONT oldfont = static_cast<HFONT>(::SelectObject(DIB.hDC, hfont));
		::GetTextMetrics(DIB.hDC, &tm);
		DWORD r = ::GetGlyphOutlineW(DIB.hDC, c, GGO_METRICS, &gm, 0, NULL, &UnitMat);
		if (r == GDI_ERROR)
		{
			::SelectObject(DIB.hDC, oldfont);
			return false;
		}
		int fatsize = ((thickness << quality) + 1);
		int bbx = ((gm.gmBlackBoxX + fatsize + (1 << quality) - 1) >> quality) << quality;
		int bby = ((gm.gmBlackBoxY + fatsize + (1 << quality) - 1) >> quality) << quality;

		if (DIB.Width < bbx || DIB.Height < bby)
		{
			if (!DIB.Resize(max(DIB.Width, bbx), max(DIB.Height, bby)))
			{
				::SelectObject(DIB.hDC, oldfont);
				return false;
			}
		}
		DIB.Clear();

		HPEN hpen = (HPEN)::CreatePen(PS_SOLID, thickness << quality, RGB(255, 255, 255));
		HPEN oldpen = (HPEN)::SelectObject(DIB.hDC, hpen);

		::BeginPath(DIB.hDC);
		::TextOutW(DIB.hDC, -gm.gmptGlyphOrigin.x + fatsize / 2, -(tm.tmAscent - gm.gmptGlyphOrigin.y) + fatsize / 2, &c, 1);
		::EndPath(DIB.hDC);
		::StrokePath(DIB.hDC);
		::SelectObject(DIB.hDC, oldfont);

		::SelectObject(DIB.hDC, oldpen);
		::DeleteObject(hpen);


		image.Initialize(bbx >> quality, bby >> quality);

		DIB.Shrink2GrayImage(image, quality);

		return true;
	}
};



OutlineBuffer outlineBuffer(0);


}//anonymous namespace


bool OutlineFont::Initialize(long height, const InitParam& param, unsigned quality)
{
	Terminalize();
	SetQuality(quality);
	HFONT hfont = ::CreateFontW(
		height << Quality, param.width << Quality, 0, 0, param.weight, FALSE, FALSE, FALSE,
		(DWORD)param.charset,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		(unsigned)param.pitch | ((unsigned)param.family >> 4), param.facename);
	if (hfont == 0)
		return false;

	hFont = hfont;
	outlineBuffer.SetTextMetrics(hfont);
	tmAscent = outlineBuffer.tm.tmAscent;
	tmHeight = outlineBuffer.tm.tmHeight;

	return true;
}

bool OutlineFont::Initialize(void* font_handle, unsigned quality)
{
	Terminalize();
	if (font_handle == 0)
		return false;

	::LOGFONTW logfont;
	if (!::GetObjectW(font_handle, sizeof(LOGFONT), &logfont))
		return false;

	SetQuality(quality);
	logfont.lfHeight <<= Quality;
	logfont.lfWidth <<= Quality;
	HFONT hfont = ::CreateFontIndirectW(&logfont);
	if (hfont == 0)
		return false;

	hFont = hfont;
	outlineBuffer.SetTextMetrics(hfont);
	tmAscent = outlineBuffer.tm.tmAscent;
	tmHeight = outlineBuffer.tm.tmHeight;

	return true;
}


bool OutlineFont::Initialize(const OutlineFont& source, long height, long width)
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

	SetQuality(source.Quality);
	logfont.lfHeight = height << Quality;
	logfont.lfWidth = width << Quality;

	HFONT hfont = ::CreateFontIndirectW(&logfont);
	if (hfont == 0)
		return false;

	hFont = hfont;
	outlineBuffer.SetTextMetrics(hfont);
	tmAscent = outlineBuffer.tm.tmAscent;
	tmHeight = outlineBuffer.tm.tmHeight;


	return true;
}

void OutlineFont::Terminalize(void)
{
	if (hFont)
	{
		::DeleteObject(hFont);
		hFont = 0;
	}
	//	Quality = 0;

	tmHeight = 0;
	tmAscent = 0;
}

short OutlineFont::GetCharCellIncX(wchar_t c) const
{
	if (!outlineBuffer.SetGlyphMetrics(static_cast<HFONT>(hFont),c))
		return CellIncError;
	return outlineBuffer.gm.gmCellIncX >> Quality;
}
short OutlineFont::GetFirstCharCellIncX(wchar_t c) const
{
	if (!outlineBuffer.SetGlyphMetrics(static_cast<HFONT>(hFont), c))
		return CellIncError;

	if (outlineBuffer.gm.gmptGlyphOrigin.x < 0)
		return (outlineBuffer.gm.gmCellIncX + short(-outlineBuffer.gm.gmptGlyphOrigin.x)) >> Quality;
	return outlineBuffer.gm.gmCellIncX >> Quality;
}

bool OutlineFont::GetFontImage(FontImage& image, wchar_t c) const
{
	if (!outlineBuffer.MakeGrayImage(image.Image, static_cast<HFONT>(hFont), Quality, c))
		return false;
	image.xoffset = outlineBuffer.gm.gmptGlyphOrigin.x >> Quality;
	image.yoffset = (tmAscent - outlineBuffer.gm.gmptGlyphOrigin.y) >> Quality;
	image.CellIncX = outlineBuffer.gm.gmCellIncX >> Quality;
	return true;
}
bool OutlineFont::GetOutlineImage(FontImage& image, wchar_t c,int thickness) const
{
	if (!outlineBuffer.MakeOutlineGrayImage(image.Image, static_cast<HFONT>(hFont), Quality, c,thickness))
		return false;
	int fatsize = ((thickness << Quality) + 1) / 2;

	image.xoffset = (outlineBuffer.gm.gmptGlyphOrigin.x - fatsize) >> Quality;
	image.yoffset = (tmAscent - outlineBuffer.gm.gmptGlyphOrigin.y - fatsize) >> Quality;
	image.CellIncX = outlineBuffer.gm.gmCellIncX >> Quality;
	return true;
}

void OutlineFont::Swap(OutlineFont& other)
{
	void* tmp = this->hFont;
	this->hFont = other.hFont;
	other.hFont = tmp;

	int q = this->Quality;
	this->Quality = other.Quality;
	other.Quality = q;

	long l = this->tmHeight;
	this->tmHeight = other.tmHeight;
	other.tmHeight = l;

	l = this->tmAscent;
	this->tmAscent = other.tmAscent;
	other.tmAscent = l;
}




bool BufferedOutlineFont::Initialize(OutlineFont& font, unsigned thickness)
{
	Terminalize();
	if (!font.IsValid())
		return false;

	BaseFont.Swap(font);
	Thickness = thickness;
	return true;
}

void BufferedOutlineFont::Terminalize(void)
{
	BaseFont.Terminalize();

	for (map::iterator it = Buffer->begin(); it != Buffer->end(); ++it)
	{
		delete it->second;
	}
	Buffer->clear();
	for (map::iterator it = OutlineBuffer->begin(); it != OutlineBuffer->end(); ++it)
	{
		delete it->second;
	}
	OutlineBuffer->clear();
	Thickness = 0;
}

void BufferedOutlineFont::SetQuality(unsigned quality)
{
	if (BaseFont.GetQuality() == quality)
		return;
	BaseFont.SetQuality(quality);
	for (map::iterator it = Buffer->begin(); it != Buffer->end(); ++it)
	{
		delete it->second;
	}
	Buffer->clear();
	for (map::iterator it = OutlineBuffer->begin(); it != OutlineBuffer->end(); ++it)
	{
		delete it->second;
	}
	OutlineBuffer->clear();
}

void BufferedOutlineFont::SetThickness(unsigned thickness)
{
	if (thickness == Thickness)
		return;
	Thickness = thickness;
	for (map::iterator it = OutlineBuffer->begin(); it != OutlineBuffer->end(); ++it)
	{
		delete it->second;
	}
	OutlineBuffer->clear();
}

bool BufferedOutlineFont::SetBuffer(wchar_t c) const
{
	FontImage* image = new FontImage();
	if (!BaseFont.GetFontImage(*image, c))
	{
		delete image;
		return false;
	}
	if (Thickness > 0)
	{
		FontImage* outlineimage = new FontImage();
		if (!BaseFont.GetOutlineImage(*outlineimage, c, Thickness))
		{
			delete outlineimage;
			delete image;
			return false;
		}
		(*OutlineBuffer)[c] = outlineimage;
	}
	(*Buffer)[c] = image;
	return true;
}

short BufferedOutlineFont::GetCharCellIncX(wchar_t c) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		return BaseFont.GetCharCellIncX(c);
	}
	return it->second->CellIncX;
}
short BufferedOutlineFont::GetFirstCharCellIncX(wchar_t c) const
{
	map::iterator it = Buffer->find(c);
	if (it == Buffer->end())
	{
		return BaseFont.GetFirstCharCellIncX(c);
	}
	FontImage& image = *it->second;
	if (image.xoffset < 0)
	{
		return image.CellIncX + -image.xoffset;
	}
	return image.CellIncX;
}

const FontImage* BufferedOutlineFont::GetFontImage(wchar_t c) const
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
const FontImage* BufferedOutlineFont::GetOutlineImage(wchar_t c) const
{
	if (Thickness == 0)
		return 0;
	map::iterator it = OutlineBuffer->find(c);
	if (it == OutlineBuffer->end())
	{
		if (!SetBuffer(c))
			return 0;
		it = OutlineBuffer->find(c);
	}
	return it->second;
}


short BufferedOutlineFont::DrawCharColor(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect) const
{
	const FontImage* image = GetFontImage(c);
	if (!image)
		return CellIncError;

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + image->xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2ImageColor(target, image->Image, color, dx, dy, &rect);
	return image->CellIncX;
}
short BufferedOutlineFont::DrawCharAlpha(I_Image& target, int x, int y, wchar_t c, const Rect* clip_rect) const
{
	const FontImage* image = GetFontImage(c);
	if (!image)
		return CellIncError;

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + image->xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2ImageAlpha(target, image->Image, dx, dy, &rect);
	return image->CellIncX;
}
short BufferedOutlineFont::DrawCharPreMultipliedAlpha(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect) const
{
	const FontImage* image = GetFontImage(c);
	if (!image)
		return CellIncError;

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + image->xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2ImagePreMultipliedAlpha(target, image->Image, color, dx, dy, &rect);
	return image->CellIncX;
}
short BufferedOutlineFont::DrawCharAlpha(I_GrayImage& target, int x, int y, wchar_t c, const Rect* clip_rect) const
{
	const FontImage* image = GetFontImage(c);
	if (!image)
		return CellIncError;

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + image->xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2GrayBlit(target, image->Image, dx, dy, &rect);
	return image->CellIncX;
}

short BufferedOutlineFont::DrawFirstCharColor(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect) const
{
	const FontImage* image = GetFontImage(c);
	if (!image)
		return CellIncError;

	int xoffset = image->xoffset;
	int cellincx = image->CellIncX;
	if (xoffset < 0)
	{
		cellincx += -xoffset;
		xoffset = 0;
	}

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2ImageColor(target, image->Image, color, dx, dy, &rect);
	return cellincx;
}
short BufferedOutlineFont::DrawFirstCharAlpha(I_Image& target, int x, int y, wchar_t c, const Rect* clip_rect) const
{
	const FontImage* image = GetFontImage(c);
	if (!image)
		return CellIncError;

	int xoffset = image->xoffset;
	int cellincx = image->CellIncX;
	if (xoffset < 0)
	{
		cellincx += -xoffset;
		xoffset = 0;
	}

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2ImageAlpha(target, image->Image, dx, dy, &rect);
	return cellincx;
}
short BufferedOutlineFont::DrawFirstCharPreMultipliedAlpha(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect) const
{
	const FontImage* image = GetFontImage(c);
	if (!image)
		return CellIncError;

	int xoffset = image->xoffset;
	int cellincx = image->CellIncX;
	if (xoffset < 0)
	{
		cellincx += -xoffset;
		xoffset = 0;
	}

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2ImagePreMultipliedAlpha(target, image->Image, color, dx, dy, &rect);
	return cellincx;
}

short BufferedOutlineFont::DrawFirstCharAlpha(I_GrayImage& target, int x, int y, wchar_t c, const Rect* clip_rect) const
{
	const FontImage* image = GetFontImage(c);
	if (!image)
		return CellIncError;

	int xoffset = image->xoffset;
	int cellincx = image->CellIncX;
	if (xoffset < 0)
	{
		cellincx += -xoffset;
		xoffset = 0;
	}

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2GrayBlit(target, image->Image, dx, dy, &rect);
	return cellincx;
}




short BufferedOutlineFont::DrawOutlineCharColor(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect) const
{
	const FontImage* image = GetOutlineImage(c);
	if (!image)
		return CellIncError;

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + image->xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2ImageColor(target, image->Image, color, dx, dy, &rect);
	return image->CellIncX;
}
short BufferedOutlineFont::DrawOutlineCharAlpha(I_Image& target, int x, int y, wchar_t c, const Rect* clip_rect) const
{
	const FontImage* image = GetOutlineImage(c);
	if (!image)
		return CellIncError;

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + image->xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2ImageAlpha(target, image->Image, dx, dy, &rect);
	return image->CellIncX;
}
short BufferedOutlineFont::DrawOutlineCharPreMultipliedAlpha(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect) const
{
	const FontImage* image = GetOutlineImage(c);
	if (!image)
		return CellIncError;

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + image->xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2ImagePreMultipliedAlpha(target, image->Image, color, dx, dy, &rect);
	return image->CellIncX;
}
short BufferedOutlineFont::DrawOutlineCharAlpha(I_GrayImage& target, int x, int y, wchar_t c, const Rect* clip_rect) const
{
	const FontImage* image = GetOutlineImage(c);
	if (!image)
		return CellIncError;

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + image->xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2GrayBlit(target, image->Image, dx, dy, &rect);
	return image->CellIncX;
}

short BufferedOutlineFont::DrawOutlineFirstCharColor(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect) const
{
	const FontImage* image = GetOutlineImage(c);
	if (!image)
		return CellIncError;

	int xoffset = image->xoffset;
	int cellincx = image->CellIncX;
	const FontImage* fontimage = GetFontImage(c);
	int font_xoffset = fontimage->xoffset;
	if (font_xoffset < 0)
	{
		cellincx += -font_xoffset;
		xoffset += -font_xoffset;;
	}

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2ImageColor(target, image->Image, color, dx, dy, &rect);
	return cellincx;
}
short BufferedOutlineFont::DrawOutlineFirstCharAlpha(I_Image& target, int x, int y, wchar_t c, const Rect* clip_rect) const
{
	const FontImage* image = GetOutlineImage(c);
	if (!image)
		return CellIncError;

	int xoffset = image->xoffset;
	int cellincx = image->CellIncX;
	const FontImage* fontimage = GetFontImage(c);
	int font_xoffset = fontimage->xoffset;
	if (font_xoffset < 0)
	{
		cellincx += -font_xoffset;
		xoffset += -font_xoffset;;
	}

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2ImageAlpha(target, image->Image, dx, dy, &rect);
	return cellincx;
}
short BufferedOutlineFont::DrawOutlineFirstCharPreMultipliedAlpha(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect) const
{
	const FontImage* outlineimage = GetOutlineImage(c);
	if (!outlineimage)
		return CellIncError;

	int xoffset = outlineimage->xoffset;
	int cellincx = outlineimage->CellIncX;

	const FontImage* fontimage = GetFontImage(c);
	int font_xoffset = fontimage->xoffset;
	if (font_xoffset < 0)
	{
		cellincx += -font_xoffset;
		xoffset += -font_xoffset;;
	}

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + xoffset, y + outlineimage->yoffset, outlineimage->Image, clip_rect);

	Gray2ImagePreMultipliedAlpha(target, outlineimage->Image, color, dx, dy, &rect);
	return cellincx;
}

short BufferedOutlineFont::DrawOutlineFirstCharAlpha(I_GrayImage& target, int x, int y, wchar_t c, const Rect* clip_rect) const
{
	const FontImage* image = GetOutlineImage(c);
	if (!image)
		return CellIncError;

	int xoffset = image->xoffset;
	int cellincx = image->CellIncX;
	const FontImage* fontimage = GetFontImage(c);
	int font_xoffset = fontimage->xoffset;
	if (font_xoffset < 0)
	{
		cellincx += -font_xoffset;
		xoffset += -font_xoffset;;
	}

	Rect rect;
	int dx, dy;
	ClipSourceRect(rect, dx, dy, x + xoffset, y + image->yoffset, image->Image, clip_rect);

	Gray2GrayBlit(target, image->Image, dx, dy, &rect);
	return cellincx;
}


}//namespace Picture
}//namespace Juna

