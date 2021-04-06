
#include "../RubyKaraokeContainer.h"
#include "../TimeTag.h"
#include "../../StringLines.h"


namespace Juna {
namespace Lyrics {

/*

��s���Ƃ̃��r�w��
[00:00.00]��������������������[00:05.00]
@Rubyed=|��(����)|��(����)|�Y(��)|��(��)��������

���r�Ɛe�����ȊO�̕����񂪈�v����΁A���̍s�����r�^�O�s�Ƃ��ď���
����@Rubyed�͂��̎��̍s����T�������B

���r�^�C���^�O�̋߂��ɖ{����u�������ꍇ


�����s�w��
@Multiline_Rubyed=
|��(����)|��(����)|�Y(��)|��(��)��������
|�h(��)��|�|(��)|�w(��)��|��(��)��|��(��)|��(��)��
@

��s���炢�͖������Ė�薳���Ǝv�����A����ȊO�͑S�Ĕ�r�Ώ�

�^�C���^�O�Ƃ͗����ēZ�߂Ė{����u�������ꍇ

���̓�͓����g�p�\
�ォ�珇�ԂɘA�����ēZ�߂čs�P�ʂŏ��������


@Multiline_XXX=
�b��d�l
�ȉ��̊e�s��@XXX=�s�̒��g
�Ƃ����^�O�ɒu��������



*/
namespace {

bool lower_compare(const std::wstring &str,const wchar_t *lower_str,size_t length)
{
	if (str.length() != length)
		return false;
	for (size_t i = 0;i < length;i++)
	{
		if (::towlower(str[i]) != lower_str[i])
		{
			return false;
		}
	}
	return true;
}
}

void RubyKaraokeContainer::SetRubyKaraoke(const std::wstring &lyrics,const Juna::Picture::OutlineFont *font,bool keephead)
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
	RubyTag ruby;
	ruby.LoadAtRubyTag(atTags);


	RubyTag::RubyedContainer &rubyed = ruby.GetRubyed();
	if (rubyed.size() == 0)
	{
		for (size_t i = 0;i < lines.size();i++)
		{
			std::wstring rd;
			TimeTag::RemoveTimeTag(rd,lines[i].word.c_str());
			rubyed.push_back(RubyString(rd,ruby.parent(),ruby.begin(),ruby.end()));

			std::vector<RubyString::RubyInfo> ri;
			if (RubyString::GetRubyInfo(ri,lines[i].word,ruby.parent(),ruby.begin(),ruby.end()) > 0)
			{
				for (int j = ri.size() - 1;j >= 0;j--)
				{
					lines[i].word.erase(ri[j].end_mark,ri[j].endend - ri[j].end_mark);
					lines[i].word.erase(ri[j].parent_mark,ri[j].ruby - ri[j].parent_mark);
				}
			}
		}
	}

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
	std::wstring plain;
	RubyTag::RubyedContainer::const_iterator current_rubyed = rubyed.begin();
	const std::wstring *phonetic;
	if (current_rubyed != rubyed.end())
		phonetic = &current_rubyed->GetPhonetic();

	std::vector<KaraokeContainer::KaraokeCharTag> timed_plain;

	for (size_t i = 0;i < lines.size();i++)
	{
		int nexttime = (i == lines.size() - 1) ? TimeTag::MaxTime : lines[i + 1].milisec;
		RubyKaraokeLine &kl = Lines[i + not_zero_start];
		kl.starttime = lines[i].milisec;

		TimeTag::RemoveTimeTag(plain,lines[i].word.c_str());

//�^�C���^�O���ꕶ�����ɍו���
		timed_plain.resize(plain.length());
		TimeTag::SeparateKaraokeLine(pairs,lines[i].milisec,lines[i].word.c_str());
		size_t count = 0;
		for (size_t j = 0; j < pairs.size() - 1; j++)
		{
			int starttime = pairs[j].milisec;
			int endtime = pairs[j + 1].milisec;
			const std::wstring &word = pairs[j].word;
			for (size_t k = 0;k < word.length(); k++)
			{
				timed_plain[count].start = ((word.length() - k) * starttime + k * endtime) / word.length();
				timed_plain[count].end = ((word.length() - (k + 1)) * starttime + (k + 1) * endtime) / word.length();
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
				timed_plain[count].start = ((word.length() - k) * starttime + k * endtime) / word.length();
				timed_plain[count].end = ((word.length() - (k + 1)) * starttime + (k + 1) * endtime) / word.length();
				count++;
			}
			kl.endtime = endtime > nexttime ? endtime : nexttime;
		}
		else
		{
			if ((pairs.back().milisec > nexttime) || (pairs.size() > 1 && pairs[pairs.size()-2].word.empty()))
				kl.endtime = pairs.back().milisec;
			else
				kl.endtime = nexttime;
		}

//�Ή����r�����邩
		const RubyString *match_rubyed = 0;
		if (current_rubyed != rubyed.end() && plain == *phonetic)
		{
			kl.string = *current_rubyed;
			kl.TimedChar.resize(kl.string.GetParent().size());
			kl.TimedRuby.resize(kl.string.GetRubyPositions().size());
			for (size_t j = 0;j < kl.string.GetRubyPositions().size();j++)
			{
				kl.TimedRuby[j].resize(kl.string.GetRubyPosition(j).phonetic_length);
			}


			size_t offset = 0;
			size_t plain_offset = 0;
			const RubyString::RubyPositions &rubys = kl.string.GetRubyPositions();
			for (size_t j = 0;j < rubys.size();j++)
			{
				for (size_t k = offset;k < rubys[j].parent_offset;k++)
				{
					kl.TimedChar[k] = timed_plain[plain_offset++];
				}
				int parent_start = timed_plain[plain_offset].start;
				for (size_t k = 0;k < rubys[j].phonetic_length;k++)
				{
					kl.TimedRuby[j][k] = timed_plain[plain_offset++];
				}
				int parent_end = timed_plain[plain_offset - 1].end;
				size_t length = rubys[j].parent_length;
				for (size_t k = 0;k < length;k++)
				{
					kl.TimedChar[rubys[j].parent_offset + k].start = ((length - k) * parent_start + k * parent_end) / length;
					kl.TimedChar[rubys[j].parent_offset + k].end = ((length - (k + 1)) * parent_start + (k + 1) * parent_end) / length;
				}
				offset = rubys[j].parent_offset + rubys[j].parent_length;
			}
			for (size_t k = offset;k < kl.string.GetParent().length();k++)
			{
				kl.TimedChar[k] = timed_plain[plain_offset++];
			}



			++current_rubyed;
			if (current_rubyed != rubyed.end())
				phonetic = &current_rubyed->GetPhonetic();
		}
		else
		{
			kl.string = plain;
			kl.TimedChar = timed_plain;
		}

		if (font)
			kl.words.SetWords(kl.string,*font);
		else
			kl.words.SetWords(kl.string);
	}
	Font = font;
}

void RubyKaraokeContainer::SetFontWidth(const Juna::Picture::OutlineFont&font)
{
	for (RubyKaraokeLines::iterator it = Lines.begin();it  != Lines.end();++it)
	{
		it->words.SetWidth(it->string,font);
	}
	Font = &font;
}

}//namespace Lyrics
}//namespace Juna