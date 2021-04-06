#pragma once


#include "LyricsContainer.h"

#include "LyricsDrawParameter.h"

namespace Juna {
namespace Lyrics {

namespace ScrollDraw {

struct DrawPoint
{
	size_t index;
	float scroll_y;
	float scroll_rate;
	int playtime_ms;
};
bool GetDrawPointLyrics(DrawPoint &dest, const LyricsContainer &lyrics,const LyricsDrawParameter &param,int playtime_ms);

void DrawLyrics(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
		  const LyricsContainer &lyrics,const LyricsDrawParameter &param,const DrawPoint &drawpoint);


void DrawKaraoke(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
		  const KaraokeContainer &karaoke,const LyricsDrawParameter &param,int playtime_ms);



void DrawCSLyrics(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
		  const LyricsContainer &lyrics,const LyricsDrawParameter &param,int playtime_ms);
void DrawCSKaraoke(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
		  const KaraokeContainer &karaoke,const LyricsDrawParameter &param,int playtime_ms);



}//namespace ScrollDraw	
}//namespace Lyrics
}//namespace Juna

