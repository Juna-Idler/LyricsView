#pragma once


#include "RubyKaraokeContainer.h"

#include "LyricsDrawParameter.h"

namespace Juna {
namespace Lyrics {

namespace ScrollDraw {

void DrawRubyKaraoke(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
		  const RubyKaraokeContainer &karaoke,const LyricsDrawParameter &param,int playtime_ms);


void DrawCSRubyKaraoke(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
		  const RubyKaraokeContainer &karaoke,const LyricsDrawParameter &param,int playtime_ms);



}//namespace ScrollDraw	
}//namespace Lyrics
}//namespace Juna

