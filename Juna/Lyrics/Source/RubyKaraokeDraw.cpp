
#include "../RubyKaraokeDraw.h"


#include "../TimeTag.h"
#include "../../Picture/Blit.h"

#include "../TextLyricsDraw.h"


namespace Juna {
namespace Lyrics {

void DrawRubyActiveLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,int drawwidth,
					const RubyKaraokeContainer::RubyKaraokeLine &line,int playtime_ms,const LyricsDrawParameter &param);


namespace ScrollDraw {

namespace {
	inline Juna::Picture::Color BlendColor(Juna::Picture::Color a,Juna::Picture::Color b,float a_rate)
	{
		return Juna::Picture::Color(
			unsigned char(b.r - (b.r - a.r) * a_rate) ,
			unsigned char(b.g - (b.g - a.g) * a_rate) ,
			unsigned char(b.b - (b.b - a.b) * a_rate) );
	}
}


void DrawRubyKaraoke(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
		  const RubyKaraokeContainer &karaoke,const LyricsDrawParameter &param,int playtime_ms)
{
	const RubyKaraokeContainer::RubyKaraokeLines &Lines = karaoke.GetLines();
	if ( Lines.empty() || karaoke.GetFont() == 0 || karaoke.GetFont() != &param.GetFont().GetFont() )
		return;

	size_t index;
	for ( index = 0 ; index < Lines.size() - 1 ; index++ )
	{
		if ( playtime_ms < Lines[index + 1].starttime )
			break;
	}
	if ( index == Lines.size() - 1 )
		return;

	size_t nextindex = index + 1;

	size_t currentlines = 1;
	if (index > 0)
	{
		for (size_t i = index - 1;Lines[i].starttime == Lines[index].starttime;i--)
		{
			currentlines++;
			if (i == 0)
				break;
		}
		index -= (currentlines - 1);
	}
	size_t nextlines = 1;
	if (nextindex + 1 < Lines.size())
	{
		for (size_t i = nextindex + 1;Lines[i].starttime == Lines[nextindex].starttime;i++)
		{
			nextlines++;
		}
	}

	int scroll_time = param.ScrollTime;
	if ( scroll_time < 0 )
		scroll_time = TimeTag::MaxTime;

	//スクロール判定
	float scroll_rate = 0;
	float fade_rate = 0;

	int next = Lines[nextindex].starttime;
	if ( playtime_ms + scroll_time > next )
	{
		int scroll = next - Lines[index].starttime;
		if ( scroll_time < scroll )
		{
			scroll = scroll_time;
		}
		int sub = next - playtime_ms;
		scroll_rate = 1.0f - float(sub) / scroll;

		if ( param.FadeTime >= 0 && scroll > param.FadeTime )
		{
			if ( sub > param.FadeTime )
				fade_rate = 0;
			else
				fade_rate = 1.0f - sub / (float)param.FadeTime;
		}
		else
			fade_rate = 1.0f - (float)sub / scroll;
	}


	//ライン幅規制
	int drawwidth = target_rect.width() - param.LeftMargin - param.RightMargin;

	long currentheight = Lines[index].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[index].GetLyricsLine(),param);
	for (size_t i = index + 1;i < index + currentlines;i++)
		currentheight += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);

	long nextheight = Lines[nextindex].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[nextindex].GetLyricsLine(),param);
	for (size_t i = nextindex + 1;i < nextindex + nextlines;i++)
		nextheight += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);


	int current_y = 0;
	long scrollheight = 0;
	switch (param.VAlignment)
	{
	case VA_Top:
		current_y = target_rect.y1 + param.TopMargin;
		scrollheight = currentheight;
		break;
	case VA_Center:
		current_y = target_rect.y1 + param.TopMargin + (target_rect.height() - (param.TopMargin + param.BottomMargin) - currentheight) / 2;
		scrollheight = (nextheight + currentheight) / 2;
		break;
	case VA_Bottom:
		current_y = target_rect.y2 - param.BottomMargin - currentheight;
		scrollheight = nextheight;
		break;
	}
	current_y += param.CurrentLineYOffset;

	long scroll_y = long(scrollheight *  scroll_rate);
	if (param.CurrentLineBackColor.a > 0)
	{
		long cy = current_y - scroll_y - param.LineSpace;
		long ny = cy + currentheight;
		long cy2 = ny + param.LineSpace;
		long ny2 = ny + nextheight + param.LineSpace;
		
		Juna::Picture::Rect rect(target_rect.x1,
								 long(cy - (cy - ny) * fade_rate) - param.CurrentLineBackPlusUp,
								 target_rect.x2,
								 long(cy2 - (cy2 - ny2) * fade_rate) + param.CurrentLineBackPlusDown);
		if (rect.SetIntersectRect(rect,target_rect))
			Juna::Picture::PreMultipliedAlpha::TranslucentFillColor(target,param.CurrentLineBackColor,param.CurrentLineBackColor.a + 1,&rect);
	}


//現在行
	int y = current_y - scroll_y;
	for (size_t i = index;i < index + currentlines;i++)
	{
		if (playtime_ms >= Lines[i].endtime)
		{
			if (playtime_ms < Lines[i].endtime + param.FadeTime)
			{
				float rate = float(playtime_ms - Lines[i].endtime) / param.FadeTime;
				Juna::Picture::Color c = BlendColor(param.OtherTextColor,param.CurrentTextColor,rate);
				Juna::Picture::Color oc = BlendColor(param.OtherTextOutlineColor,param.CurrentTextOutlineColor,rate);
				DrawLine(target, target_rect,y,drawwidth,c, oc, Lines[i].GetLyricsLine(),param);
			}
			else
			{
				DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i].GetLyricsLine(),param);
			}
		}
		else
			DrawRubyActiveLine(target, target_rect, y, drawwidth, Lines[i], playtime_ms,param);
		y += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);
	}
//過去行
	y = current_y - scroll_y;
	for ( int i = index - 1 ; i >= 0 && y > param.TopMargin ; i-- )
	{
		y -= Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);
		if (playtime_ms >= Lines[i].endtime)
		{
			if (playtime_ms < Lines[i].endtime + param.FadeTime)
			{
				float rate = float(playtime_ms - Lines[i].endtime) / param.FadeTime;
				Juna::Picture::Color c = BlendColor(param.OtherTextColor,param.CurrentTextColor,rate);
				Juna::Picture::Color oc = BlendColor(param.OtherTextOutlineColor,param.CurrentTextOutlineColor,rate);
				DrawLine(target, target_rect,y,drawwidth,c, oc, Lines[i].GetLyricsLine(),param);
			}
			else
			{
				DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i].GetLyricsLine(),param);
			}
		}
		else
			DrawRubyActiveLine(target, target_rect, y, drawwidth, Lines[i], playtime_ms,param);

	}

//未来行
	y = current_y - scroll_y + currentheight;
	for ( size_t i = nextindex ; i < Lines.size() && y < target.height() - param.BottomMargin ; i++ )
	{
		if (playtime_ms >= Lines[i].starttime - param.FadeTime)
		{
			float rate = float(playtime_ms + param.FadeTime - Lines[i].starttime) / param.FadeTime;
			Juna::Picture::Color c = BlendColor(param.StandbyTextColor,param.OtherTextColor,rate);
			Juna::Picture::Color oc = BlendColor(param.StandbyTextOutlineColor,param.OtherTextOutlineColor,rate);
			DrawLine(target, target_rect,y,drawwidth,c, oc, Lines[i].GetLyricsLine(),param);
		}
		else
		{
			DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i].GetLyricsLine(),param);
		}
		y += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);
	}
}

void DrawCSRubyKaraoke(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
		  const RubyKaraokeContainer &karaoke,const LyricsDrawParameter &param,int playtime_ms)
{
	const RubyKaraokeContainer::RubyKaraokeLines &Lines = karaoke.GetLines();
	if (Lines.empty())
		return;

	size_t index;
	for ( index = 0 ; index < Lines.size() - 1 ; index++ )
	{
		if ( playtime_ms < Lines[index + 1].starttime )
			break;
	}
	if ( index == Lines.size() - 1 )
		return;

	size_t nextindex = index + 1;

	size_t currentlines = 1;
	if (index > 0)
	{
		for (size_t i = index - 1;Lines[i].starttime == Lines[index].starttime;i--)
		{
			currentlines++;
			if (i == 0)
				break;
		}
		index -= (currentlines - 1);
	}
	size_t nextlines = 1;
	if (nextindex + 1 < Lines.size())
	{
		for (size_t i = nextindex + 1;Lines[i].starttime == Lines[nextindex].starttime;i++)
		{
			nextlines++;
		}
	}


	float fscroll_y = 0;
	float scroll_rate = 0;

	int next = Lines[nextindex].starttime;

	int scroll = next - Lines[index].starttime;
	int sub = next - playtime_ms;
	fscroll_y = 1.0f - float(sub) / scroll;

	if ( scroll > param.FadeTime )
	{
		if ( sub > param.FadeTime )
			scroll_rate = 0;
		else
			scroll_rate = 1.0f - sub / (float)param.FadeTime;
	}
	else
		scroll_rate = 1.0f - (float)sub / scroll;

	//ライン幅規制
	int drawwidth = target_rect.width() - param.LeftMargin - param.RightMargin;

	long currentheight = Lines[index].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[index].GetLyricsLine(),param);
	for (size_t i = index + 1;i < index + currentlines;i++)
		currentheight += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);

	long nextheight = Lines[nextindex].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[nextindex].GetLyricsLine(),param);
	for (size_t i = nextindex + 1;i < nextindex + nextlines;i++)
		nextheight += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);

	int current_y = 0;
	switch (param.VAlignment)
	{
	case VA_Top:
		current_y = target_rect.y1 + param.TopMargin;
		break;
	case VA_Center:
		current_y = target_rect.y1 + param.TopMargin + (target_rect.height() - (param.TopMargin + param.BottomMargin)) / 2;
		break;
	case VA_Bottom:
		current_y = target_rect.y2 - param.BottomMargin;
		break;
	}
	current_y += param.CurrentLineYOffset;

	long scroll_y = long(currentheight *  fscroll_y);

	if (param.CurrentLineBackColor.a > 0)
	{
		long cy = current_y - scroll_y - param.LineSpace;
		long ny = cy + currentheight;
		long cy2 = ny + param.LineSpace;
		long ny2 = ny + nextheight + param.LineSpace;
		
		Juna::Picture::Rect rect(target_rect.x1,
								 long(cy - (cy - ny) * scroll_rate) - param.CurrentLineBackPlusUp,
								 target_rect.x2,
								 long(cy2 - (cy2 - ny2) * scroll_rate) + param.CurrentLineBackPlusDown);
		if (rect.SetIntersectRect(rect,target_rect))
			Juna::Picture::PreMultipliedAlpha::TranslucentFillColor(target,param.CurrentLineBackColor,param.CurrentLineBackColor.a + 1,&rect);
	}



//現在行
	int y = current_y - scroll_y;
	for (size_t i = index;i < index + currentlines;i++)
	{
		if (playtime_ms >= Lines[i].endtime)
		{
			if (playtime_ms < Lines[i].endtime + param.FadeTime)
			{
				float rate = float(playtime_ms - Lines[i].endtime) / param.FadeTime;
				Juna::Picture::Color c = BlendColor(param.OtherTextColor,param.CurrentTextColor,rate);
				Juna::Picture::Color oc = BlendColor(param.OtherTextOutlineColor,param.CurrentTextOutlineColor,rate);
				DrawLine(target, target_rect,y,drawwidth,c, oc, Lines[i].GetLyricsLine(),param);
			}
			else
			{
				DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i].GetLyricsLine(),param);
			}
		}
		else
			DrawRubyActiveLine(target, target_rect, y, drawwidth, Lines[i], playtime_ms,param);
		y += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);
	}
//過去行
	y = current_y - scroll_y;
	for ( int i = index - 1 ; i >= 0 && y > param.TopMargin ; i-- )
	{
		y -= Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);
		if (playtime_ms >= Lines[i].endtime)
		{
			if (playtime_ms < Lines[i].endtime + param.FadeTime)
			{
				float rate = float(playtime_ms - Lines[i].endtime) / param.FadeTime;
				Juna::Picture::Color c = BlendColor(param.OtherTextColor,param.CurrentTextColor,rate);
				Juna::Picture::Color oc = BlendColor(param.OtherTextOutlineColor,param.CurrentTextOutlineColor,rate);
				DrawLine(target, target_rect,y,drawwidth,c, oc, Lines[i].GetLyricsLine(),param);
			}
			else
			{
				DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i].GetLyricsLine(),param);
			}
		}
		else
			DrawRubyActiveLine(target, target_rect, y, drawwidth, Lines[i], playtime_ms,param);

	}

//未来行
	y = current_y - scroll_y + currentheight;
	for ( size_t i = nextindex ; i < Lines.size() && y < target.height() - param.BottomMargin ; i++ )
	{
		if (playtime_ms >= Lines[i].starttime - param.FadeTime)
		{
			float rate = float(playtime_ms + param.FadeTime - Lines[i].starttime) / param.FadeTime;
			Juna::Picture::Color c = BlendColor(param.StandbyTextColor,param.OtherTextColor,rate);
			Juna::Picture::Color oc = BlendColor(param.StandbyTextOutlineColor,param.OtherTextOutlineColor,rate);
			DrawLine(target, target_rect,y,drawwidth,c, oc, Lines[i].GetLyricsLine(),param);
		}
		else
		{
			DrawLine(target, target_rect,y,drawwidth,param.OtherTextColor, param.OtherTextOutlineColor, Lines[i].GetLyricsLine(),param);
		}
		y += Lines[i].words.CountLine(drawwidth,param.CharSpace) * GetLineHeight(Lines[i].GetLyricsLine(),param);
	}
}

}//namespace ScrollDraw




void DrawRubyActiveAlignedLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
						   int y,int drawwidth,
						   const RubyKaraokeContainer::RubyKaraokeLine &line,size_t word_start,size_t word_length,
						   int playtime_ms,const LyricsDrawParameter &param)
{
	Juna::Picture::Rect clip_rect(target_rect.x1,// + LeftMargin,
								  target_rect.y1 + param.TopMargin,
								  target_rect.x2,// - RightMargin,
								  target_rect.y2 - param.BottomMargin);
	int x = 0;
	switch (param.HAlignment)
	{
	case HA_Left:
		x = target_rect.x1 + param.LeftMargin;
		break;
	case HA_Right:
		x = target_rect.x2 - param.RightMargin - drawwidth;
		break;
	case HA_Center:
		x = target_rect.x1 + (target_rect.width() - param.LeftMargin - param.RightMargin - drawwidth) / 2 + param.LeftMargin;
		break;
	}

	int base_y;
	if (param.GetRubyFont() && !line.string.GetRubyPositions().empty())
		base_y = y + param.GetRubyFont()->GetHeight() + param.RubyBottomSpace;
	else
		base_y = y + param.NoRubyLineTopSpace;

	const Juna::Picture::BufferedOutlineFont &font = param.GetFont();
	const std::wstring &text = line.string.GetParent();

	

	const std::vector<KaraokeContainer::KaraokeCharTag> &chars = line.TimedChar;
	const LineBreakWords::Words &words = line.words.GetWords();
	int separate_point = x;
	for (size_t i = 0;i < word_length;i++)
	{
		const LineBreakWords::Word &word = words[word_start + i];
		wchar_t character = text[word.offset];

		if (playtime_ms <= chars[word.offset].start)
		{
			goto SeparateSearchEnd;
		}
		if (playtime_ms <= chars[word.offset].end)
		{
			const Juna::Picture::FontImage *image = font.GetFontImage(character);
			if (image)
			{
				const KaraokeContainer::KaraokeCharTag &kct = chars[word.offset];
				float rate = float(playtime_ms - kct.start) / float(kct.end - kct.start);
				int char_width = image->CellIncX;
//				char_width += -image->xoffset;
				separate_point += int(char_width * rate);// + image->xoffset
			}
			goto SeparateSearchEnd;
		}
		int width = font.GetFirstCharCellIncX(character);
		separate_point += width + param.CharSpace;
		for (int j = 1;j < word.length; j++)
		{
			character = text[word.offset + j];
			if (playtime_ms <= chars[word.offset + j].end)
			{
				const Juna::Picture::FontImage *image = font.GetFontImage(character);
				if (image)
				{
					const KaraokeContainer::KaraokeCharTag &kct = chars[word.offset + j];
					float rate = float(playtime_ms - kct.start) / float(kct.end - kct.start);
					int char_width = image->CellIncX;
//					char_width += -image->xoffset;
					separate_point += int(char_width * rate);// + image->xoffset
				}
				goto SeparateSearchEnd;
			}
			width = font.GetCharCellIncX(character);
			separate_point += width + param.CharSpace;
		}
	}
SeparateSearchEnd:;
	Juna::Picture::Rect current_rect = clip_rect; current_rect.x2 = separate_point;
	Juna::Picture::Rect standby_rect = clip_rect; standby_rect.x1 = separate_point;

	Juna::Picture::Color cc = param.CurrentTextColor;
	Juna::Picture::Color coc = param.CurrentTextOutlineColor;
	Juna::Picture::Color sc = param.StandbyTextColor;
	Juna::Picture::Color soc = param.StandbyTextOutlineColor;


	if (font.GetThickness() > 0)
	{
		int xoffset = x;
		for (size_t i = word_start; i < word_start + word_length; i++)
		{
			const LineBreakWords::Word &word = words[i];
			if (word.length == 0)
				continue;
			font.DrawOutlineFirstCharPreMultipliedAlpha(target,xoffset, base_y, text[word.offset], coc,&current_rect);
			font.DrawOutlineFirstCharPreMultipliedAlpha(target,xoffset, base_y, text[word.offset], soc,&standby_rect);
			font.DrawFirstCharColor(target,xoffset, base_y,  text[word.offset], cc,&current_rect);
			int width = font.DrawFirstCharColor(target,xoffset, base_y,  text[word.offset], sc,&standby_rect);
			xoffset += width + param.CharSpace;
			for (int j = 1; j < word.length; j++)
			{
				font.DrawOutlineCharPreMultipliedAlpha(target,xoffset, base_y, text[word.offset + j], coc,&current_rect);
				font.DrawOutlineCharPreMultipliedAlpha(target,xoffset, base_y, text[word.offset + j], soc,&standby_rect);
				font.DrawCharColor(target,xoffset, base_y,  text[word.offset + j], cc,&current_rect);
				int width = font.DrawCharColor(target,xoffset, base_y,  text[word.offset + j], sc,&standby_rect);
				xoffset += width + param.CharSpace;
			}
		}
	}
	else
	{
		int xoffset = x;
		for (size_t i = word_start; i < word_start + word_length; i++)
		{
			const LineBreakWords::Word &word = words[i];
			if (word.length == 0)
				continue;
			font.DrawFirstCharPreMultipliedAlpha(target,xoffset, base_y,  text[word.offset], cc,&current_rect);
			int width = font.DrawFirstCharPreMultipliedAlpha(target,xoffset, base_y,  text[word.offset], sc,&standby_rect);
			xoffset += width + param.CharSpace;
			for (int j = 1; j < word.length; j++)
			{
				font.DrawCharPreMultipliedAlpha(target,xoffset, base_y,  text[word.offset + j], cc,&current_rect);
				int width = font.DrawCharPreMultipliedAlpha(target,xoffset, base_y,  text[word.offset + j], sc,&standby_rect);
				xoffset += width + param.CharSpace;
			}
		}

	}
	
//ルビ
	if (!param.GetRubyFont())
		return;
	const Juna::RubyString::RubyPositions &rubys = line.string.GetRubyPositions();
	if (rubys.empty())
		return;

	size_t word_offset = word_start;
	size_t char_offset = 0;
	size_t offset = words[word_start].offset;
	int offset_x = x;

	size_t ruby_index = 0;
	for (; ruby_index < rubys.size(); ruby_index++ )
	{
		if (rubys[ruby_index].parent_offset >= words[word_start].offset)
			break;
	}

	for (; ruby_index < rubys.size(); ruby_index++ )
	{
		for (;word_offset < word_start + word_length; word_offset++)
		{
			const LineBreakWords::Word &word = words[word_offset];
			if (char_offset == 0)
			{
				if (offset == rubys[ruby_index].parent_offset)
					goto RubySeachEnd;
				offset_x += font.GetFirstCharCellIncX(text[word.offset]) + param.CharSpace;
				offset++;
				char_offset = 1;
			}
			for (; char_offset < word.length; char_offset++)
			{
				if (offset == rubys[ruby_index].parent_offset)
					goto RubySeachEnd;
				offset_x += font.GetCharCellIncX(text[word.offset + char_offset]) + param.CharSpace;
				offset++;
			}
			char_offset = 0;
		}
		continue;

RubySeachEnd:;
//ルビ描画
		const Juna::Picture::BufferedOutlineFont &rubyfont = *param.GetRubyFont();

		int start_x = offset_x;
		int width = 0;
		for (size_t i = 0;i < rubys[ruby_index].parent_length; i++)
		{
			width += font.GetCharCellIncX(text[rubys[ruby_index].parent_offset + i]) + param.CharSpace;
		}
		width -= param.CharSpace;

		int rubywidth = 0;
		for (size_t i = 0; i < rubys[ruby_index].phonetic_length; i++)
		{
			rubywidth += rubyfont.GetCharCellIncX(line.string.GetPhonetic()[rubys[ruby_index].phonetic_offset + i]);
		}

		int x;
		int space;
		RubyAlignment ra = param.RAlignment;
		if (rubywidth >= width && (ra == RA_121 || ra == RA_010))
		{
			ra = RA_Center;
		}
		switch (ra)
		{
		default:
		case RA_Center:
			x = start_x + (width - rubywidth) / 2;
			space = 0;
			break;
		case RA_121:
			space = (width - rubywidth) / rubys[ruby_index].phonetic_length;
			x = start_x + ((width - rubywidth) - space * (rubys[ruby_index].phonetic_length - 1)) / 2;
			break;
		case RA_010:
			if (rubys[ruby_index].phonetic_length > 1)
			{
				space = (width - rubywidth) / (rubys[ruby_index].phonetic_length - 1);
				x = start_x;
			}
			else
			{
				x = start_x + (width - rubywidth) / 2;
				space = 0;
			}
			break;
		case RA_Left:
			x = start_x;
			space = 0;
			break;
		case RA_Right:
			x = start_x + (width - rubywidth);
			space = 0;
			break;
		}


		KaraokeContainer::KaraokeCharTag first = line.TimedRuby[ruby_index].front();
		KaraokeContainer::KaraokeCharTag last = line.TimedRuby[ruby_index].back();

		if (playtime_ms <= first.start)
		{
			if (rubyfont.GetThickness() > 0)
			{
				for (size_t i = 0; i < rubys[ruby_index].phonetic_length; i++)
				{
					wchar_t c = line.string.GetPhonetic()[rubys[ruby_index].phonetic_offset + i];
					rubyfont.DrawOutlineCharPreMultipliedAlpha(target,x, y, c, soc);
					x += rubyfont.DrawCharColor(target,x,y,c,sc) + space;
				}
			}
			else
			{
				for (size_t i = 0; i < rubys[ruby_index].phonetic_length; i++)
				{
					x += rubyfont.DrawCharPreMultipliedAlpha(target,x,y,line.string.GetPhonetic()[rubys[ruby_index].phonetic_offset + i],sc) + space;
				}
			}
		}
		else if (playtime_ms >= last.end)
		{
			if (rubyfont.GetThickness() > 0)
			{
				for (size_t i = 0; i < rubys[ruby_index].phonetic_length; i++)
				{
					wchar_t c = line.string.GetPhonetic()[rubys[ruby_index].phonetic_offset + i];
					rubyfont.DrawOutlineCharPreMultipliedAlpha(target,x, y, c, coc);
					x += rubyfont.DrawCharColor(target,x,y,c,cc) + space;
				}
			}
			else
			{
				for (size_t i = 0; i < rubys[ruby_index].phonetic_length; i++)
				{
					x += rubyfont.DrawCharPreMultipliedAlpha(target,x,y,line.string.GetPhonetic()[rubys[ruby_index].phonetic_offset + i],cc) + space;
				}
			}
		}
		else
		{
			if (rubyfont.GetThickness() > 0)
			{
				for (size_t i = 0; i < rubys[ruby_index].phonetic_length; i++)
				{
					wchar_t c = line.string.GetPhonetic()[rubys[ruby_index].phonetic_offset + i];
					KaraokeContainer::KaraokeCharTag kct = line.TimedRuby[ruby_index][i];
					if (playtime_ms <= kct.start)
					{
						rubyfont.DrawOutlineCharPreMultipliedAlpha(target,x, y, c, soc);
						x += rubyfont.DrawCharColor(target,x,y,c,sc) + space;
					}
					else if (playtime_ms >= kct.end)
					{
						rubyfont.DrawOutlineCharPreMultipliedAlpha(target,x, y, c, coc);
						x += rubyfont.DrawCharColor(target,x,y,c,cc) + space;
					}
					else
					{
						float rate = float(playtime_ms - kct.start) / float(kct.end - kct.start);
						int char_width = rubyfont.GetCharCellIncX(c);
//						char_width += -image->xoffset;
						int wipe_point = x + int(char_width * rate);// + image->xoffset
						current_rect.x2 = standby_rect.x1 = wipe_point;

						rubyfont.DrawOutlineCharPreMultipliedAlpha(target,x, y, c, coc,&current_rect);
						rubyfont.DrawOutlineCharPreMultipliedAlpha(target,x, y, c, soc,&standby_rect);
						rubyfont.DrawCharColor(target,x,y,c,cc,&current_rect);
						x += rubyfont.DrawCharColor(target,x,y,c,sc,&standby_rect) + space;
					}
				}
			}
			else
			{
				for (size_t i = 0; i < rubys[ruby_index].phonetic_length; i++)
				{
					wchar_t c = line.string.GetPhonetic()[rubys[ruby_index].phonetic_offset + i];
					KaraokeContainer::KaraokeCharTag kct = line.TimedRuby[ruby_index][i];
					if (playtime_ms <= kct.start)
					{
						x += rubyfont.DrawCharColor(target, x, y, c, sc) + space;
					}
					else if (playtime_ms >= kct.end)
					{
						x += rubyfont.DrawCharColor(target, x, y, c, cc) + space;
					}
					else
					{
						float rate = float(playtime_ms - kct.start) / float(kct.end - kct.start);
						int char_width = rubyfont.GetCharCellIncX(c);
						int wipe_point = x + int(char_width * rate);// + image->xoffset
						current_rect.x2 = standby_rect.x1 = wipe_point;

						rubyfont.DrawCharColor(target, x, y, c, cc, &current_rect);
						x += rubyfont.DrawCharColor(target, x, y, c, sc, &standby_rect) + space;
					}
				}
			}
		}
	}

}

void DrawRubyActiveLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,int drawwidth,
					const RubyKaraokeContainer::RubyKaraokeLine &line,int playtime_ms,const LyricsDrawParameter &param)
{
	if (line.string.GetParent().empty())
		return;

	int lineheight = GetLineHeight(line.GetLyricsLine(),param);

	const LineBreakWords::Words &words = line.words.GetWords();
	size_t start = 0;
	size_t length = 1;
	int xoffset = words[0].width + param.CharSpace * (words[0].length - 1);
	int yoffset = 0;
	for (size_t j = 1; j < words.size(); j++)
	{
		const LineBreakWords::Word &word = words[j];
		int width = word.width + param.CharSpace * (word.length - 1);
		if (xoffset + param.CharSpace + width > drawwidth)
		{
			if (playtime_ms < line.TimedChar[words[start].offset].start)
			{
				DrawAlignedLine(target,target_rect,y + yoffset,xoffset,line.GetLyricsLine(),start,length,
								param.StandbyTextColor,param.StandbyTextOutlineColor,param);
			}
			else
			{
				const Juna::LineBreakWords::Word &endword = words[start + length - 1];

				if (playtime_ms >= line.TimedChar[endword.offset + endword.length - 1].end)
				{
					DrawAlignedLine(target,target_rect,y + yoffset,xoffset,line.GetLyricsLine(),start,length,
									param.CurrentTextColor,param.CurrentTextOutlineColor,param);
				}
				else
				{
					DrawRubyActiveAlignedLine(target,target_rect,y + yoffset,xoffset,line,start,length,playtime_ms,param);
				}
			}
			start += length;
			length = 1;
			yoffset += lineheight;
			xoffset = width;
		}
		else
		{
			xoffset += param.CharSpace + width;
			length++;
		}
	}
	if (playtime_ms < line.TimedChar[words[start].offset].start)
	{
		DrawAlignedLine(target,target_rect,y + yoffset,xoffset,line.GetLyricsLine(),start,length,
						param.StandbyTextColor,param.StandbyTextOutlineColor,param);
	}
	else
	{
		const Juna::LineBreakWords::Word &endword = words[start + length - 1];

		if (playtime_ms >= line.TimedChar[endword.offset + endword.length - 1].end)
		{
			DrawAlignedLine(target,target_rect,y + yoffset,xoffset,line.GetLyricsLine(),start,length,
							param.CurrentTextColor,param.CurrentTextOutlineColor,param);
		}
		else
		{
			DrawRubyActiveAlignedLine(target,target_rect,y + yoffset,xoffset,line,start,length,playtime_ms,param);
		}
	}
}


}//namespace Lyrics
}//namespace Juna

