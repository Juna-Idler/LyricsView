#pragma once

#include <vector>

#include "LyricsContainer.h"
#include "LyricsDrawParameter.h"


namespace Juna {
namespace Lyrics {


class KaraokeDrawer
{
private:
	struct ActiveLine
	{
		size_t line;
		size_t index;

		bool operator<(const ActiveLine &rhs) const {return line < rhs.line;}
		bool operator==(const ActiveLine &rhs) const {return line == rhs.line;}

		ActiveLine(void) : line(0),index(0) {}
		ActiveLine(size_t l,size_t i) : line(l),index(i) {}
	};
	typedef std::vector<ActiveLine> ActiveLineContainer;

	const KaraokeContainer *Container;
	ActiveLineContainer ActiveLines;
	size_t CurrentLine;
	int LastTime;
public:
	void Draw(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int bottom_margin,
			  const KaraokeContainer &karaoke,const LyricsDrawParameter &param,int playtime_ms);


	KaraokeDrawer(void) : Container(0),ActiveLines(),CurrentLine(0),LastTime(0)
	{}
};


void DrawActiveLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,int drawwidth,
					const KaraokeContainer::KaraokeLine &line,int playtime_ms,const LyricsDrawParameter &param);

void DrawActiveAlignedLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
						   int y,int drawwidth,
						   const KaraokeContainer::KaraokeLine &line,size_t word_start,size_t word_length,
						   int playtime_ms,const LyricsDrawParameter &param);


}//namespace Lyrics
}//namespace Juna

