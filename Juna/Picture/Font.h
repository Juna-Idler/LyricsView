
#ifndef Juna_Picture_Font_Image_H
#define Juna_Picture_Font_Image_H

#include "Image.h"
#include "../2d_coordinate.h"

#include <map>


namespace Juna {
namespace Picture {

typedef D2::Rectangle<long> Rect;

static const short CellIncError = -0x8000;

struct FontImage
{
	GrayImage Image;
	int xoffset = 0;
	int yoffset = 0;
	int CellIncX = 0;
	//�x���o�邩��K���ɏ��������A�ŋ߂͂���Œʂ�̂�
};


//���t�����������������`��
class OutlineFont
{
public:
	enum class CharSet { CharSetANSI = 0, CharSetDefault = 1, CharSetSymbol = 2, CharSetShiftJis = 128, CharSetOEM = 255 };
	enum class Pitch { PitchDefault = 0, PitchFixed = 1, PitchVariable = 2 };
	enum class Family { FamilyDontcare = 0, FamilyRoman = 1, FamilySwiss = 2, FamilyModern = 3, FamilyScript = 4, FamilyDecorative = 5 };

	struct InitParam
	{
		const wchar_t* facename;
		long width;
		long weight;
		CharSet charset;
		Pitch pitch;
		Family family;

		InitParam() :
			facename(0), width(0), weight(0), charset(CharSet::CharSetShiftJis), pitch(Pitch::PitchDefault), family(Family::FamilyDontcare)
		{}
		explicit InitParam(const wchar_t* facename, long width = 0, long weight = 0, Pitch pitch = Pitch::PitchDefault, Family family = Family::FamilyDontcare, CharSet charset = CharSet::CharSetShiftJis)
			: facename(facename), width(width), weight(weight), charset(charset), pitch(pitch), family(family)
		{}

	};

public:
	OutlineFont(void) : hFont(0), Quality(0), tmAscent(), tmHeight() {}
	~OutlineFont() { Terminalize(); }

	bool Initialize(long height, const InitParam& param, unsigned quality = 0);

	bool Initialize(const wchar_t* facename, long height, long width = 0, long weight = 0, unsigned quality = 0)
	{
		return Initialize(height, InitParam(facename, width, weight), quality);
	}
	//Windows Font Handle  LOGFONT���擾���Ď��O�ŃR�s�[�����܂�
	bool Initialize(void* font_handle, unsigned quality = 0);


	//�T�C�Y��ς��ăR�s�[
	bool Initialize(const OutlineFont& source, long height, long width = 0);


	void Terminalize(void);

	bool IsValid(void) const { return hFont != 0; }

	//1�ȏ�Ŕ{���摜���k�������A���`�G�C���A�X����������B���{ 2�{ 4�{ 8�{
	unsigned GetQuality(void) const { return Quality; }
	void SetQuality(unsigned quality)
	{
		if (quality > 3) quality = 3;
		Quality = quality;
	}

	long GetHeight(void) const { return tmHeight >> Quality; }

	short GetCharCellIncX(wchar_t c) const;
	short GetFirstCharCellIncX(wchar_t c) const;


	bool GetFontImage(FontImage& image, wchar_t c) const;
	bool GetOutlineImage(FontImage& image, wchar_t c, int thickness) const;

	void Swap(OutlineFont& other);

private:
	void* hFont;
	int Quality;

	long  tmHeight;
	long  tmAscent;

private:
	OutlineFont(OutlineFont&);
	void operator=(OutlineFont&);

};


class BufferedOutlineFont
{
private:
	typedef std::map<wchar_t, FontImage*> map;
public:
	BufferedOutlineFont(void) : BaseFont(), Buffer(new map()), OutlineBuffer(new map()), Thickness(0) {}
	~BufferedOutlineFont() { Terminalize(); delete Buffer; }

//�n���ꂽOutlineFont�͂��̃C���X�^���X�̎x�z���ɓ���A��������font�͖����Ȃ��̂��Ԃ���܂�
//thickness��1�ȏ�̏ꍇ�A�����摜�����Ƃ��ɁA���摜�������ɐ������܂��B
	bool Initialize(OutlineFont& font, unsigned thickness = 0);

//�x�[�X�ƂȂ�OutlineFont���ꏏ�ɔj�����܂�
	void Terminalize(void);

	bool IsValid(void) const { return BaseFont.IsValid(); }

	const OutlineFont& GetFont(void) const { return BaseFont; }

	void SetQuality(unsigned quality);
	void SetThickness(unsigned thickness);

	unsigned GetQuality(void) const { return BaseFont.GetQuality(); }
	unsigned GetThickness(void) const { return Thickness; }

	long GetHeight(void) const { return BaseFont.GetHeight(); }

	short GetCharCellIncX(wchar_t c) const;
	short GetFirstCharCellIncX(wchar_t c) const;

	short DrawCharColor(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect = 0) const;
	short DrawCharAlpha(I_Image& target, int x, int y, wchar_t c, const Rect* clip_rect = 0) const;
	short DrawCharPreMultipliedAlpha(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect = 0) const;
	short DrawCharAlpha(I_GrayImage& target, int x, int y, wchar_t c, const Rect* clip_rect = 0) const;

	short DrawFirstCharColor(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect = 0) const;
	short DrawFirstCharAlpha(I_Image& target, int x, int y, wchar_t c, const Rect* clip_rect = 0) const;
	short DrawFirstCharPreMultipliedAlpha(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect = 0) const;
	short DrawFirstCharAlpha(I_GrayImage& target, int x, int y, wchar_t c, const Rect* clip_rect = 0) const;

	short DrawOutlineCharColor(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect = 0) const;
	short DrawOutlineCharAlpha(I_Image& target, int x, int y, wchar_t c, const Rect* clip_rect = 0) const;
	short DrawOutlineCharPreMultipliedAlpha(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect = 0) const;
	short DrawOutlineCharAlpha(I_GrayImage& target, int x, int y, wchar_t c, const Rect* clip_rect = 0) const;

	short DrawOutlineFirstCharColor(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect = 0) const;
	short DrawOutlineFirstCharAlpha(I_Image& target, int x, int y, wchar_t c, const Rect* clip_rect = 0) const;
	short DrawOutlineFirstCharPreMultipliedAlpha(I_Image& target, int x, int y, wchar_t c, Color color, const Rect* clip_rect = 0) const;
	short DrawOutlineFirstCharAlpha(I_GrayImage& target, int x, int y, wchar_t c, const Rect* clip_rect = 0) const;

	const FontImage* GetFontImage(wchar_t c) const;
	const FontImage* GetOutlineImage(wchar_t c) const;

private:
	bool SetBuffer(wchar_t c) const;

private:
	OutlineFont BaseFont;
	map* Buffer;
	map* OutlineBuffer;
	unsigned Thickness;
private:
	BufferedOutlineFont(BufferedOutlineFont&);
	void operator=(BufferedOutlineFont&);
};



class Font
{
public:
	enum class CharSet {CharSetANSI = 0,CharSetDefault = 1,CharSetSymbol = 2,CharSetShiftJis = 128,CharSetOEM = 255};
	enum class Pitch {PitchDefault = 0,PitchFixed = 1,PitchVariable = 2};
	enum class Family {FamilyDontcare = 0,FamilyRoman = 1,FamilySwiss = 2,FamilyModern = 3,FamilyScript = 4,FamilyDecorative = 5};

	struct InitParam
	{
		const wchar_t *facename;
		long width;
		long weight;
		CharSet charset;
		Pitch pitch;
		Family family;

		InitParam() :
			facename(0), width(0), weight(0), charset(CharSet::CharSetShiftJis), pitch(Pitch::PitchDefault), family(Family::FamilyDontcare)
		{}
		explicit InitParam(const wchar_t *facename,long width = 0,long weight = 0,Pitch pitch = Pitch::PitchDefault,Family family = Family::FamilyDontcare,CharSet charset = CharSet::CharSetShiftJis)
			: facename(facename), width(width), weight(weight), charset(charset), pitch(pitch), family(family)
		{}
	};

public:
	Font(void) : hFont(0) , Quality(0) , hDC(0) , hDCFont(0),tmAscent(),tmHeight() {}
	~Font() {Terminalize();}

	bool Initialize(long height,const InitParam &param,unsigned quality = 0);

	bool Initialize(const wchar_t *facename,long height,long width = 0,long weight = 0,unsigned quality = 0)
	{
		return Initialize(height,InitParam(facename,width,weight),quality);
	}
//Windows Font Handle  LOGFONT���擾���Ď��O�ŃR�s�[�����܂�
	bool Initialize(void *font_handle,unsigned quality = 0);

//�R�s�[
	bool Initialize(const Font &source)
	{
		if (this == &source)
			return true;
		return Initialize(source.hFont,source.Quality);
	}

//�T�C�Y��ς��ăR�s�[
	bool Initialize(const Font &source,long height,long width = 0);


	void Terminalize(void);

	bool IsValid(void) const {return hDC != 0;}

//1�ȏ�ŃA���`�G�C���A�X
	unsigned GetQuality(void) const {return Quality;}
	void SetQuality(unsigned quality)
	{
		if (quality > 3) quality = 3;
		Quality = quality;
	}

	long GetHeight(void) const {return tmHeight;}

	short GetCharCellIncX(wchar_t c) const;

	//return CellIncX  :  failed = CellIncError
	short DrawCharColor(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect = 0) const;
	short DrawCharAlpha(I_Image &target,int x,int y,wchar_t c,const Rect *clip_rect = 0) const;
	short DrawCharPreMultipliedAlpha(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect = 0) const;
	short DrawCharAlpha(I_GrayImage &target,int x,int y,wchar_t c,const Rect *clip_rect = 0) const;


	//�P�����ڌn�iGlyphOrigin.x ���}�C�i�X�̕����ł��A0�ɂ��Ă��炷�j
	short GetFirstCharCellIncX(wchar_t c) const;

	short DrawFirstCharColor(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect = 0) const;
	short DrawFirstCharAlpha(I_Image &target,int x,int y,wchar_t c,const Rect *clip_rect = 0) const;
	short DrawFirstPreMultipliedAlpha(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect = 0) const;
	short DrawFirstCharAlpha(I_GrayImage &target,int x,int y,wchar_t c,const Rect *clip_rect = 0) const;


	bool GetFontImage(FontImage &image,wchar_t c) const;

	void Swap(Font &other);

private:
	void *hFont;
	int Quality;

	void *hDC;
	void *hDCFont;

	long  tmHeight;
	long  tmAscent;

private:
	Font(Font &);
	void operator=(Font &);
};


class BufferedFont
{
private:
	typedef std::map<wchar_t,FontImage *> map;
public:
	BufferedFont(void) : BaseFont(), Buffer(new map()) {}
	~BufferedFont() {Terminalize(); delete Buffer;}

	bool Initialize(Font &font);

	void Terminalize(void);

	bool IsValid(void) const {return BaseFont.IsValid();}

	const Font &GetFont(void) const {return BaseFont;}

	void SetQuality(unsigned quality);

	long GetHeight(void) const {return BaseFont.GetHeight();}

	short GetCharCellIncX(wchar_t c) const;
	short GetFirstCharCellIncX(wchar_t c) const;

	short DrawCharColor(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect = 0) const;
	short DrawCharAlpha(I_Image &target,int x,int y,wchar_t c,const Rect *clip_rect = 0) const;
	short DrawCharPreMultipliedAlpha(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect = 0) const;
	short DrawCharAlpha(I_GrayImage &target,int x,int y,wchar_t c,const Rect *clip_rect = 0) const;

	short DrawFirstCharColor(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect = 0) const;
	short DrawFirstCharAlpha(I_Image &target,int x,int y,wchar_t c,const Rect *clip_rect = 0) const;
	short DrawFirstCharPreMultipliedAlpha(I_Image &target,int x,int y,wchar_t c,Color color,const Rect *clip_rect = 0) const;
	short DrawFirstCharAlpha(I_GrayImage &target,int x,int y,wchar_t c,const Rect *clip_rect = 0) const;

	const FontImage *GetFontImage(wchar_t c) const;

private:
	bool SetBuffer(wchar_t c) const;

private:
	Font BaseFont;
	map *Buffer;
private:
	BufferedFont(BufferedFont &);
	void operator=(BufferedFont &);
};



}//namespace Picture
}//namespace Juna

#endif
