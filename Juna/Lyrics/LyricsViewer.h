#pragma once

#include "LyricsContainer.h"
#include "LyricsDrawParameter.h"

#include "TextLyricsDraw.h"
#include "LyricsScrollDraw.h"
#include "KaraokeDraw.h"

#include "RubyKaraokeContainer.h"

#include "../Picture/DIBImage.h"



namespace Juna {
namespace Lyrics {



	
class LyricsViewer
{
	class C_BackGroundImage
	{
	public:
		C_BackGroundImage(void) :
			SourceImage(), DisplayImage(), Filter(0,0,0,0),TranslucentAlpha(256),
			LimitDisplayX(true), LimitDisplayY(true),
			LimitSourceX(true), LimitSourceY(true),
			LimitMaxX(true), LimitMaxY(true), MaxX(300), MaxY(300),
			Position(5)
		{}


		Juna::Picture::DIBImage SourceImage;
		Juna::Picture::Image DisplayImage;

		Juna::Picture::ARGB Filter;
		int TranslucentAlpha;

		bool LimitDisplayX;
		bool LimitDisplayY;
		bool LimitSourceX;
		bool LimitSourceY;
		bool LimitMaxX;
		bool LimitMaxY;

		int MaxX;
		int MaxY;

		int Position;


		void Update(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect);

		void Terminalize(void)
		{
			SourceImage.Terminalize();
			DisplayImage.Terminalize();
		}
	};

	class C_InformationText
	{
		TextContainer Text;

		Juna::Picture::BufferedOutlineFont Font;

	public:
		const TextContainer &GetText(void) const {return Text;}

		TextDrawParameter Param;
		Juna::Picture::Color Color;
		Juna::Picture::Color OutlineColor;


		bool SetFont(unsigned int fontquality,Juna::Picture::OutlineFont &font, unsigned outlinethickness);
		void SetText(const std::wstring &text);

		void Terminalize(void)
		{
			Text.Terminalize();
			Font.Terminalize();
		}
	};
	
	class C_FrameImage
	{
	public:
		C_FrameImage(void) : SourceImage() {}

		Juna::Picture::DIBImage SourceImage;

		void Draw(Juna::Picture::I_Image &target);

		void Terminalize(void)
		{
			SourceImage.Terminalize();
		}
	};

public:
	LyricsViewer(void) :
		Canvas(),
		BGColor(0,0,0),
		NTTTextColor(255,255,255),
		NTTTextOutlineColor(0,0,0),

		LyricsMode(LM_Null),
		Lyrics(),
		Karaoke(),
		Text(),
		RubyKaraoke(),

		Parameter(),
		Font(), RubyFont(),

		BGImage(),
		InfoText(),

		LyricsLayoutRect(0,0,0,0),
		BGImageLayoutRect(0,0,0,0),
		TextLayoutRect(0,0,0,0),

		LastDrawPoint(),
		ForceRedraw(true)
	{
		LastDrawPoint.index = 0;
		LastDrawPoint.scroll_y = 0;
		LastDrawPoint.scroll_rate = 0;
		LastDrawPoint.playtime_ms = 0;
	}

	enum enumLyricsMode {LM_Null = 0,LM_Lyrics = 1,LM_Karaoke = 2,LM_Text = 3,LM_RubyKaraoke = 4};
private:
	Juna::Picture::DIBImage Canvas;

	enumLyricsMode LyricsMode;

	LyricsContainer Lyrics;
	KaraokeContainer Karaoke;
	TextContainer Text;
	RubyKaraokeContainer RubyKaraoke;

	Juna::Picture::BufferedOutlineFont Font;
	Juna::Picture::BufferedOutlineFont RubyFont;


	Juna::Lyrics::ScrollDraw::DrawPoint LastDrawPoint;
	bool ForceRedraw;

	KaraokeDrawer KDrawer;

	void FillBGColor(void);

public:
	LyricsDrawParameter Parameter;
	Juna::Picture::ARGB BGColor;
	Juna::Picture::Color NTTTextColor;
	Juna::Picture::Color NTTTextOutlineColor;

	C_BackGroundImage BGImage;
	C_InformationText InfoText;
	C_FrameImage FrameImage;

	Juna::Picture::Rect LyricsLayoutRect;
	Juna::Picture::Rect BGImageLayoutRect;
	Juna::Picture::Rect TextLayoutRect;


	bool IsValid(void) const {return (Lyrics.IsValid() || Karaoke.IsValid() || Text.IsValid() || RubyKaraoke.IsValid()) && Parameter.IsValid();}
	enumLyricsMode GetLyricsMode(void) const {return LyricsMode;}

	void SetLyrics(const std::wstring &text);
	void SetKaraoke(const std::wstring &text,bool keephead = false);
	void SetText(const std::wstring &text);
	void SetRubyKaraoke(const std::wstring &text,bool keephead = false);

	bool SetFont(unsigned int fontquality,Juna::Picture::OutlineFont &font,int outlinethickness);

	void Terminalize(void)
	{
		Lyrics.Terminalize();
		Karaoke.Terminalize();
		Text.Terminalize();
		RubyKaraoke.Terminalize();
		RubyFont.Terminalize();
		Font.Terminalize();
		Canvas.Terminalize();

		BGImage.Terminalize();
		InfoText.Terminalize();
		FrameImage.Terminalize();
	}

	void Resize(int width,int height);


	bool Update(int playtime_ms);
	bool UpdateContinuousScroll(int playtime_ms);

	const Juna::Picture::DIBImage &GetCanvas(void) const {return Canvas;}


	void SetForceRedraw(void) {ForceRedraw = true;}


	bool UpdateKaraoke(int playtime_ms,int bottom_margin);

};

}//namespace Lyrics
}//namespace Juna
