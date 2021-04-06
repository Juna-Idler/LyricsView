#include "../KaraokeDraw.h"

#include <algorithm>

#include "../TextLyricsDraw.h"

namespace Juna {
namespace Lyrics {

int GetLineWidth(const LineBreakWords::Words &words,const LyricsDrawParameter &param)
{
	if (words.empty())
		return 0;
	int linewidth = words[0].width + param.CharSpace * (words[0].length - 1);

	for (size_t i = 1;i < words.size();i++)
	{
		const LineBreakWords::Word &word = words[i];
		linewidth += word.width + param.CharSpace * (word.length);
	}
	return linewidth;
}


void KaraokeDrawer::Draw(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int bottom_margin,
			  const KaraokeContainer &karaoke,const LyricsDrawParameter &param,int playtime_ms)
{
	if ( karaoke.GetFont() == 0 || karaoke.GetFont() != &param.GetFont().GetFont() )
		return;

	const KaraokeContainer::KaraokeLines &lines = karaoke.GetLines();
	if (lines.empty())
		return;

	size_t index;
	for ( index = 0 ; index < lines.size() - 1 ; index++ )
	{
		if ( playtime_ms < lines[index + 1].starttime )
			break;
	}
	if ( index == lines.size() - 1 )
		return;


	if (Container != &karaoke)
	{
		ActiveLines.clear();
		CurrentLine = index;
		Container = &karaoke;
	}
	else if (index < CurrentLine)
	{
		ActiveLines.clear();
		CurrentLine = index;
	}
	else if (index > CurrentLine && playtime_ms > LastTime + 1 * 1000)
	{
		ActiveLines.clear();
		CurrentLine = index;
	}
	LastTime = playtime_ms;


	{//çÌèúèàóù
		for (size_t i = 0;i < ActiveLines.size();i++)
		{
			const KaraokeContainer::KaraokeLine &line = lines[ActiveLines[i].index];
			if (playtime_ms > line.endtime)
			{
				ActiveLines[i].line = size_t(-1);
			}
		}
		std::sort(ActiveLines.begin(),ActiveLines.end());
		ActiveLineContainer::iterator end_it = std::remove(ActiveLines.begin(),ActiveLines.end(),ActiveLine(size_t(-1),0));
		ActiveLines.erase(end_it,ActiveLines.end());
	}

	if (index > CurrentLine)
	{
		for (size_t i = CurrentLine;i < index;i++)
		{
			if (playtime_ms < lines[i].endtime)
			{
				size_t line = 1;
				for (size_t j = 0;j < ActiveLines.size();j++)
				{
					if (ActiveLines[j].line == line)
					{
						line = ++ActiveLines[j].line;
					}
				}
				ActiveLines.insert(ActiveLines.begin(),ActiveLine(1,i));
			}
		}
	}
	CurrentLine = index;



	int norubylineheight = param.GetFont().GetHeight() + param.LineSpace + param.NoRubyLineTopSpace;
	int rubyedlineheight = param.GetFont().GetHeight() + param.LineSpace + param.GetRubyFont()->GetHeight() + param.RubyBottomSpace;
	int lineheight = norubylineheight > rubyedlineheight ? norubylineheight : rubyedlineheight;


	int scroll_y = 0;
	size_t scroll_lines = 0;
	if (lines[index + 1].starttime < lines[index].endtime)
	{
		int start = lines[index + 1].starttime - param.ScrollTime;
		if (playtime_ms > start)
		{
			int scrolltime = lines[index].endtime - lines[index].starttime;
			scrolltime = scrolltime < param.ScrollTime ? scrolltime : param.ScrollTime;
			float rate = float(playtime_ms - start) / scrolltime;

			scroll_lines = 1;
			for (size_t i = index + 2;lines[i].starttime == lines[index + 1].starttime;i++)
				scroll_lines++;
			scroll_y = int(lineheight * rate);
		}
	}



	int base_y = target_rect.y2 - param.BottomMargin - bottom_margin;

	int draw_width = GetLineWidth(lines[index].GetLyricsLine().words.GetWords(),param);

	DrawActiveLine(target,target_rect,
					base_y - GetLineHeight(lines[index].GetLyricsLine(),param) - scroll_y * scroll_lines,draw_width,
					lines[index],
					playtime_ms,param);

	size_t line = 1;
	for (size_t i = 0;i < ActiveLines.size();i++)
	{
		int y = base_y - lineheight * ActiveLines[i].line;
		if (ActiveLines[i].line <= line + scroll_lines)
		{
			int scroll = line + scroll_lines - ActiveLines[i].line;
			y -= scroll_y * scroll;
		}
		index = ActiveLines[i].index;
		draw_width = GetLineWidth(lines[index].GetLyricsLine().words.GetWords(),param);
		DrawActiveLine(target,target_rect,
						y - GetLineHeight(lines[index].GetLyricsLine(),param),draw_width,
						lines[index],
						playtime_ms,param);
		line++;
	}

}




void DrawActiveAlignedLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,
						   int y,int drawwidth,
						   const KaraokeContainer::KaraokeLine &line,size_t word_start,size_t word_length,
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
	
//ÉãÉr
	if (!param.GetRubyFont())
		return;
	const Juna::RubyString::RubyPositions &rubys = line.string.GetRubyPositions();
	if (rubys.empty())
		return;

	size_t word_offset = word_start;
	size_t char_offset = 0;
	size_t offset = words[word_start].offset;
	int offset_x = x;

	Juna::RubyString::RubyPositions::const_iterator it = rubys.begin();
	for (; it != rubys.end(); ++it)
	{
		if (it->parent_offset >= words[word_start].offset)
			break;
	}

	for (; it != rubys.end(); ++it)
	{
		for (;word_offset < word_start + word_length; word_offset++)
		{
			const LineBreakWords::Word &word = words[word_offset];
			if (char_offset == 0)
			{
				if (offset == it->parent_offset)
					goto RubySeachEnd;
				offset_x += font.GetFirstCharCellIncX(text[word.offset]) + param.CharSpace;
				offset++;
				char_offset = 1;
			}
			for (; char_offset < word.length; char_offset++)
			{
				if (offset == it->parent_offset)
					goto RubySeachEnd;
				offset_x += font.GetCharCellIncX(text[word.offset + char_offset]) + param.CharSpace;
				offset++;
			}
			char_offset = 0;
		}
		continue;

RubySeachEnd:;
//ÉãÉrï`âÊ
		const Juna::Picture::BufferedOutlineFont &rubyfont = *param.GetRubyFont();

		int start_x = offset_x;
		int width = 0;
		for (size_t i = 0;i < it->parent_length; i++)
		{
			width += font.GetCharCellIncX(text[it->parent_offset + i]) + param.CharSpace;
		}
		width -= param.CharSpace;

		int rubywidth = 0;
		for (size_t i = 0; i < it->phonetic_length; i++)
		{
			rubywidth += rubyfont.GetCharCellIncX(line.string.GetPhonetic()[it->phonetic_offset + i]);
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
			space = (width - rubywidth) / it->phonetic_length;
			x = start_x + ((width - rubywidth) - space * (it->phonetic_length - 1)) / 2;
			break;
		case RA_010:
			if (it->phonetic_length > 1)
			{
				space = (width - rubywidth) / (it->phonetic_length - 1);
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


		if (rubyfont.GetThickness() > 0)
		{
			for (size_t i = 0; i < it->phonetic_length; i++)
			{
				wchar_t c = line.string.GetPhonetic()[it->phonetic_offset + i];
				rubyfont.DrawOutlineCharPreMultipliedAlpha(target,x, y, c, coc,&current_rect);
				rubyfont.DrawOutlineCharPreMultipliedAlpha(target,x, y, c, soc,&standby_rect);
				rubyfont.DrawCharColor(target,x,y,c,cc,&current_rect);
				x += rubyfont.DrawCharColor(target,x,y,c,sc,&standby_rect) + space;
			}
		}
		else
		{
			for (size_t i = 0; i < it->phonetic_length; i++)
			{
				rubyfont.DrawCharPreMultipliedAlpha(target,x,y,line.string.GetPhonetic()[it->phonetic_offset + i],cc,&current_rect);
				x += rubyfont.DrawCharPreMultipliedAlpha(target,x,y,line.string.GetPhonetic()[it->phonetic_offset + i],sc,&standby_rect) + space;
			}
		}
	}

}

void DrawActiveLine(Juna::Picture::I_Image &target,const Juna::Picture::Rect &target_rect,int y,int drawwidth,
					const KaraokeContainer::KaraokeLine &line,int playtime_ms,const LyricsDrawParameter &param)
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
					DrawActiveAlignedLine(target,target_rect,y + yoffset,xoffset,line,start,length,playtime_ms,param);
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
			DrawActiveAlignedLine(target,target_rect,y + yoffset,xoffset,line,start,length,playtime_ms,param);
		}
	}
}

}//namespace Lyrics
}//namespace Juna

