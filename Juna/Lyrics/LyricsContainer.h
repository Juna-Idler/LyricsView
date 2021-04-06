#pragma once


#include <string>
#include <vector>

#include "RubyCommon.h"
#include "../Picture/Font.h"


namespace Juna {
namespace Lyrics {


class TextContainer
{
public:
	struct TextLine
	{
		RubyLineBreakWords words;
		RubyString string;
	};
	typedef std::vector<TextLine> TextLines;

private:
	TextLines Lines;
	const Juna::Picture::OutlineFont *Font;
public:
	TextContainer(void) : Lines(), Font(0) {}
	~TextContainer() {Terminalize();}

	bool IsValid(void) const {return !Lines.empty() && Font;}

	void Terminalize(void) {Lines.clear();Font = 0;}

	const TextLines &GetLines(void) const {return Lines;}
	const Juna::Picture::OutlineFont *GetFont(void) const {return Font;}

	void SetText(const std::wstring &lyrics,const Juna::Picture::OutlineFont *font = 0);
	void SetFontWidth(const Picture::OutlineFont &font);
};

class LyricsContainer
{
public:
	struct LyricsLine : public TextContainer::TextLine
	{
		using TextLine::words;	 //RubyLineBreakWords words;
		using TextLine::string;	 //RubyString string;
		int starttime;
	};
	typedef std::vector<LyricsLine> LyricsLines;

private:
	LyricsLines Lines;
	const Juna::Picture::OutlineFont *Font;
public:
	LyricsContainer(void) : Lines(), Font(0) {}
	~LyricsContainer() {Terminalize();}

	bool IsValid(void) const {return !Lines.empty() && Font;}

	void Terminalize(void) {Lines.clear();Font = 0;}

	const LyricsLines &GetLines(void) const {return Lines;}
	const Juna::Picture::OutlineFont *GetFont(void) const {return Font;}

	void SetLyrics(const std::wstring &lyrics,const Juna::Picture::OutlineFont *font = 0);
	void SetFontWidth(const Juna:: Picture::OutlineFont& font);
};

class KaraokeContainer
{
public:
	struct KaraokeCharTag
	{
		int start;
		int end;
	};
	struct KaraokeLine : public LyricsContainer::LyricsLine
	{
//		using LyricsLine::words;	 //RubyLineBreakWords words;
//		using LyricsLine::string;	 //RubyString string;
//		using LyricsLine::starttime; //int starttime;
		int endtime;
		std::vector<KaraokeCharTag> TimedChar;

		const LyricsContainer::LyricsLine &GetLyricsLine(void) const {return *this;}
	};
	typedef std::vector<KaraokeLine> KaraokeLines;

private:
	KaraokeLines Lines;
	const Juna::Picture::OutlineFont *Font;
public:
	KaraokeContainer(void) : Lines(), Font(0) {}
	~KaraokeContainer() {Terminalize();}

	bool IsValid(void) const {return !Lines.empty() && Font;}

	void Terminalize(void) {Lines.clear();Font = 0;}

	const KaraokeLines &GetLines(void) const {return Lines;}
	const Juna::Picture::OutlineFont *GetFont(void) const {return Font;}

	void SetKaraoke(const std::wstring &lyrics,const Juna::Picture::OutlineFont *font = 0,bool keephead = false);
	void SetFontWidth(const Juna::Picture::OutlineFont &font);
};




}//namespace Lyrics
}//namespace Juna
