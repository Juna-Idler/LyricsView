#include "../LineBreak.h"


namespace Juna {


//Unicode Line Breaking Algorithm とやらを実装できれば良いんだろうけどめんどくさそう

//文字強制分割文字
std::wstring LineBreakWords::forcebreak = 
	L" \t\r\n　"
	L"@*|/";

//次の文字強制結合文字
std::wstring LineBreakWords::forcelink1 =
	L"([{\"'<"
	L"‘“（〔［｛〈《「『【";

//前の文字強制接合文字
std::wstring LineBreakWords::forcelink2 =
	L",.;:)]}\"'>"
	L"、。，．’”）〕］｝〉》」』】";

//ASCII以外の前の文字結合文字
std::wstring LineBreakWords::not_begin =
	L"・：；？！ヽヾゝゞ〃々ー―～…‥"
	L"っゃゅょッャュョぁぃぅぇぉァィゥェォ";

//次の文字分割文字
std::wstring LineBreakWords::nextbreak1 = 
	L")]};>"
	L"!%&?";

//数値結合記号（その他は分割）
std::wstring LineBreakWords::numlink_nextbreak1 =
	L".,-:$\\";

std::wstring LineBreakWords::prevbreak2 = 
	L"([{<";


bool LineBreakWords::isLink(wchar_t c1,wchar_t c2)
{
	//２文字改行
	if (c1 == L'\r' && c2 == '\n')
		return true;
	if (isForceBreak(c1) || isForceBreak(c2))
		return false;

	if (isForceLink1(c1) || isForceLink2(c2))
		return true;

//ASCII文字
	if (c1 <= 0x7F)
	{
		if (isNextBreak1(c1))
			return false;
		if (isPrevBreak2(c2))
			return false;
		if (isNumLink_NextBreak1(c1))
		{
			if (L'0' <= c2 && c2 <= L'9')
				return true;
			else
				return false;
		}

		//その他のASCIIの連続は全部くっつける
		if (c2 <= 0x7F)
		{
			return true;
		}
		return false;
	}

//その他の文字
	if (isNotBegin(c2))
		return true;

	return false;
}


void LineBreakWords::SetWords(const std::wstring &text)
{
	words.clear();
	if (text.size() == 0)
		return;

	int current_offset = 0;
	int current_length = 1;
	for (size_t i = 1; i < text.size(); i++)
	{
		wchar_t c1 = text[i-1];
		wchar_t c2 = text[i];
		if (isLink(c1,c2))
		{
			current_length++;
		}
		else
		{
			words.push_back(LineBreakWords::Word(current_offset,current_length));
			current_offset += current_length;
			current_length = 1;
		}
	}
	words.push_back(LineBreakWords::Word(current_offset,current_length));
}

void LineBreakWords::SetWords(const std::wstring &text, const Juna::Picture::OutlineFont&font)
{
	words.clear();
	if (text.size() == 0)
		return;

	int current_offset = 0;
	int current_length = 1;
	for (size_t i = 1; i < text.size(); i++)
	{
		wchar_t c1 = text[i-1];
		wchar_t c2 = text[i];
		if (isLink(c1,c2))
		{
			current_length++;
		}
		else
		{
			words.push_back(LineBreakWords::Word(current_offset,current_length));
			current_offset += current_length;
			current_length = 1;
		}
	}
	words.push_back(LineBreakWords::Word(current_offset,current_length));

	SetWidth(text,font);
}
void LineBreakWords::SetWidth(const std::wstring &text, const Juna::Picture::OutlineFont&font)
{
	for (size_t i = 0; i < words.size(); i++)
	{
		if (words[i].length > 0)
		{
			if (text[words[i].offset] == L'\n' || text[words[i].offset] == L'\r')
			{
				words[i].width = ReturnMagicNumber;
				continue;
			}

			words[i].width = font.GetFirstCharCellIncX(text[words[i].offset]);
			for (int j = 1; j < words[i].length; j++)
			{
				words[i].width += font.GetCharCellIncX(text[words[i].offset + j]);
			}
		}
	}
}



int LineBreakWords::CountLine(int linewidth,int char_space) const
{
	if (words.size() == 0)
		return 1;
	int count = 1;

	long size = words[0].width;
	size_t i = 0;
	while (size == ReturnMagicNumber)
	{
		count++;
		i++;
		if (i == words.size())
			return count;
		size = words[i].width;
	}
	size += char_space * (words[i].length - 1);
	i++;
	for (; i < words.size(); i++)
	{
		long width = words[i].width;
		while (width == ReturnMagicNumber)
		{
			count++;
			i++;
			if (i == words.size())
				return count;
			width = words[i].width;
		}
		width += char_space * (words[i].length - 1);
		if (size + char_space + width > linewidth)
		{
			count++;
			size = width;
		}
		else
			size += char_space + width;
	}
	return count;
}


}//namespace Juna
