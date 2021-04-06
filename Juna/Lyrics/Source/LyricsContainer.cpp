
#include "../LyricsContainer.h"
#include "../TimeTag.h"
#include "../../StringLines.h"

namespace Juna {
namespace Lyrics {

void LyricsContainer::SetLyrics(const std::wstring &lyrics,const Juna::Picture::OutlineFont *font)
{
	Lines.clear();
	Font = 0;

	StringLines lyricslines(lyrics);

	TimeTag::PairContainer pairs;
	TimeTag::PickupHeadTimeTagLine(pairs,lyricslines.Lines());
	if (pairs.empty())
		return;

	AtTagContainer atTags;
	atTags.LoadAtTags(lyricslines.Lines());
	RubyTag rubying;
	rubying.LoadAtRubyTag(atTags);

	bool not_zero_start = pairs[0].milisec != 0;


	Lines.resize(pairs.size() + not_zero_start + 1);
	if (not_zero_start)
	{
		Lines.front().starttime = 0;
		Lines.front().string.AssignString(L"");
	}
	Lines.back().starttime = TimeTag::MaxTime;
	Lines.back().string.AssignString(L"");

	for (size_t i = 0;i < pairs.size();i++)
	{
		LyricsLine &ll = Lines[i + not_zero_start];
		ll.starttime = pairs[i].milisec;
		TimeTag::RemoveTimeTag(pairs[i].word);
		ll.string.AssignString(pairs[i].word,rubying.parent(),rubying.begin(),rubying.end());

		for (RubyTag::RubyContainer::const_iterator it = rubying.GetRubys().begin(); it != rubying.GetRubys().end(); ++it)
			ll.string.AddRuby(*it);

		if (font)
			ll.words.SetWords(ll.string,*font);
		else
			ll.words.SetWords(ll.string);
	}
	Font = font;
}

void LyricsContainer::SetFontWidth(const Juna::Picture::OutlineFont &font)
{
	for (LyricsLines::iterator it = Lines.begin();it  != Lines.end();++it)
	{
		it->words.SetWidth(it->string,font);
	}
	Font = &font;
}
	
void KaraokeContainer::SetKaraoke(const std::wstring &lyrics,const Juna::Picture::OutlineFont *font,bool keephead)
{
	Lines.clear();
	Font = 0;

	StringLines lyricslines(lyrics);

	std::vector<TimeTag::Pair> lines;
	TimeTag::PickupHeadTimeTagLine(lines,lyricslines.Lines());
	if (lines.empty())
		return;

	AtTagContainer atTags;
	atTags.LoadAtTags(lyricslines.Lines());
	RubyTag rubying;
	rubying.LoadAtRubyTag(atTags);

	bool not_zero_start = lines[0].milisec != 0;

	Lines.resize(lines.size() + not_zero_start + 1);
	if (not_zero_start)
	{
		Lines.front().starttime = 0;
		Lines.front().endtime = lines[0].milisec;
		Lines.front().string.AssignString(L"");
	}
	Lines.back().starttime = TimeTag::MaxTime;
	Lines.back().string.AssignString(L"");

	std::vector<TimeTag::Pair> pairs;
	std::vector<RubyString::RubyInfo> rubyinfo;
	for (size_t i = 0;i < lines.size();i++)
	{
		int nexttime = (i == lines.size() - 1) ? TimeTag::MaxTime : lines[i + 1].milisec;
		KaraokeLine &kl = Lines[i + not_zero_start];
		kl.starttime = lines[i].milisec;

		std::wstring plain;
		TimeTag::RemoveTimeTag(plain,lines[i].word.c_str());
		kl.string.AssignString(plain,rubying.parent(),rubying.begin(),rubying.end());
		if (RubyString::GetRubyInfo(rubyinfo,lines[i].word,rubying.parent(),rubying.begin(),rubying.end()) > 0)
		{
			for (std::vector<RubyString::RubyInfo>::const_reverse_iterator it = rubyinfo.rbegin();
				it != rubyinfo.rend(); ++it)
			{
				const RubyString::RubyInfo &ri = *it;
				lines[i].word.replace(ri.begin_mark,ri.endend - ri.begin_mark,L"");
				lines[i].word.replace(ri.parent_mark,ri.parent - ri.parent_mark,L"");
			}
		}
		for (RubyTag::RubyContainer::const_iterator it = rubying.GetRubys().begin(); it != rubying.GetRubys().end(); ++it)
			kl.string.AddRuby(*it);

		TimeTag::SeparateKaraokeLine(pairs,lines[i].milisec,lines[i].word.c_str());

		kl.TimedChar.resize(kl.string.GetString().size());
		size_t count = 0;
		for (size_t j = 0; j < pairs.size() - 1; j++)
		{
			int starttime = pairs[j].milisec;
			int endtime = pairs[j + 1].milisec;
			const std::wstring &word = pairs[j].word;

			for (size_t k = 0;k < word.size(); k++)
			{
				kl.TimedChar[count].start = ((word.size() - k) * starttime + k * endtime) / word.size();
				kl.TimedChar[count].end = ((word.size() - (k + 1)) * starttime + (k + 1) * endtime) / word.size();
				count++;
			}
		}
		if (!pairs.back().word.empty())
		{
			TimeTag::Pair &p = pairs.back();
			int starttime = p.milisec;
			int endtime = (keephead || p.milisec > nexttime) ? p.milisec : nexttime;
			const std::wstring &word = p.word;

			for (size_t k = 0;k < word.size(); k++)
			{
				kl.TimedChar[count].start = ((word.size() - k) * starttime + k * endtime) / word.size();
				kl.TimedChar[count].end = ((word.size() - (k + 1)) * starttime + (k + 1) * endtime) / word.size();
				count++;
			}
			kl.endtime = endtime > nexttime ? endtime : nexttime;
		}
		else
		{
			if ((pairs.back().milisec > nexttime) ||
				(pairs.size() > 1 && pairs[pairs.size()-1].word.empty() && pairs[pairs.size()-2].word.empty()))
				kl.endtime = pairs[pairs.size()-1].milisec;
			else
				kl.endtime = nexttime;
		}
		if (font)
			kl.words.SetWords(kl.string,*font);
		else
			kl.words.SetWords(kl.string);
	}
	Font = font;
}


void KaraokeContainer::SetFontWidth(const Juna::Picture::OutlineFont &font)
{
	for (KaraokeLines::iterator it = Lines.begin();it  != Lines.end();++it)
	{
		it->words.SetWidth(it->string,font);
	}
	Font = &font;
}


void TextContainer::SetText(const std::wstring &text,const Juna::Picture::OutlineFont *font)
{
	Lines.clear();
	Font = 0;

	StringLines textlines(text);

	AtTagContainer atTags;
	atTags.LoadAtTags(textlines.Lines());
	RubyTag rubying;
	rubying.LoadAtRubyTag(atTags);


	std::list<std::wstring> lines;
	int offset = 0;
	for (size_t i = 0;i < text.length();i++)
	{
		if (text[i] == L'\r')
		{
			std::wstring line = text.substr(offset,i - offset);
			lines.push_back(line);
			if (text[i+1] == L'\n')
				i++;
			offset = i + 1;
			continue;
		}
		if (text[i] == L'\n')
		{
			std::wstring line = text.substr(offset,i - offset);
			lines.push_back(line);
			offset = i + 1;
			continue;
		}
	}

	Lines.resize(lines.size());
	std::list<std::wstring>::iterator it = lines.begin();
	for (size_t i = 0 ; i < Lines.size() ; i++ )
	{
		TextLine &tl = Lines[i];
		tl.string.AssignString(*it,rubying.parent(),rubying.begin(),rubying.end());
		++it;

		for (RubyTag::RubyContainer::const_iterator it = rubying.GetRubys().begin(); it != rubying.GetRubys().end(); ++it)
			tl.string.AddRuby(*it);
		if (font)
			tl.words.SetWords(tl.string,*font);
		else
			tl.words.SetWords(tl.string);
	}
	Font = font;
}

void TextContainer::SetFontWidth(const Juna::Picture::OutlineFont &font)
{
	for (TextLines::iterator it = Lines.begin();it  != Lines.end();++it)
	{
		it->words.SetWidth(it->string,font);
	}
	Font = &font;
}

}//namespace Lyrics
}//namespace Juna

