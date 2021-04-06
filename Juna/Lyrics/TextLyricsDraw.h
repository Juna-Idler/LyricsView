#pragma once


#include "LyricsContainer.h"

#include "LyricsDrawParameter.h"



namespace Juna {
namespace Lyrics {


long GetLineHeight(const TextContainer::TextLine &line,const TextDrawParameter &param);

void DrawAlignedLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,int drawwidth,
					 const TextContainer::TextLine &line,size_t word_start,size_t word_length,
					 Juna::Picture::Color c,Juna::Picture::Color outc,const TextDrawParameter &param);

void DrawLine(Juna::Picture::I_Image &target, const Juna::Picture::Rect &target_rect,
			  int y,int drawwidth,Juna::Picture::Color c,Juna::Picture::Color outc,
			  const TextContainer::TextLine &line,const TextDrawParameter &param);



void DrawText(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
			  Juna::Picture::Color c,Juna::Picture::Color outc,
			  const TextContainer &text,const TextDrawParameter &param);


}//namespace Lyrics
}//namespace Juna
