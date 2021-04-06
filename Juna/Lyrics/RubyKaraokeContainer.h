#pragma once


#include <string>
#include <vector>

#include "LyricsContainer.h"


namespace Juna {
namespace Lyrics {




class RubyKaraokeContainer
{
public:

	struct RubyKaraokeLine : public KaraokeContainer::KaraokeLine
	{
//		using LyricsLine::words;	 //RubyLineBreakWords words;
//		using LyricsLine::string;	 //RubyString string;
//		using LyricsLine::starttime; //int starttime;
//		using endtime;				 //int endtime;
//		using TimedChar;			 //std::vector<KaraokeCharTag> TimedChar;

		std::vector<std::vector<KaraokeContainer::KaraokeCharTag>> TimedRuby; 

//		using GetLyricsLine(void) const;	//const LyricsContainer::LyricsLine &GetLyricsLine(void) const;
	};
	typedef std::vector<RubyKaraokeLine> RubyKaraokeLines;

private:
	RubyKaraokeLines Lines;
	const Juna::Picture::OutlineFont *Font;
public:
	RubyKaraokeContainer(void) : Lines(), Font(0) {}
	~RubyKaraokeContainer() {Terminalize();}

	bool IsValid(void) const {return !Lines.empty() && Font;}

	void Terminalize(void) {Lines.clear();Font = 0;}

	const RubyKaraokeLines &GetLines(void) const {return Lines;}
	const Juna::Picture::OutlineFont *GetFont(void) const {return Font;}

	void SetRubyKaraoke(const std::wstring &lyrics,const Juna::Picture::OutlineFont *font = 0,bool keephead = false);
	void SetFontWidth(const Juna:: Picture::OutlineFont& font);
};




}//namespace Lyrics
}//namespace Juna
