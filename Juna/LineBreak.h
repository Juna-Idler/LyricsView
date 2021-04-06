#pragma once

#include <string>
#include <vector>

#include "Picture/Font.h"

namespace Juna {

class LineBreakWords
{
public:
	static const long ReturnMagicNumber = 0x3FFFFFFF;

	struct Word
	{
		unsigned short offset;   //äJénï∂éöà íu
		unsigned short length;   //ï∂éöêî

		long width;     //ï`âÊéûÇÃïù

		Word(void) : offset(0), length(0), width(0) {}
		Word(short offset,short length,long width = 0) : offset(offset),length(length),width(width) {}
	};
	typedef std::vector<Word> Words;

	Words words;

public:
	const Words &GetWords(void) const {return words;}

	void SetWords(const std::wstring &text);
	void SetWords(const std::wstring &text, const Juna::Picture::OutlineFont &font);
	void SetWidth(const std::wstring &text, const Juna::Picture::OutlineFont&font);


	int CountLine(int linewidth,int char_space) const;


	LineBreakWords(void) {}
	~LineBreakWords() {}

public:
	static std::wstring forcebreak;
	static std::wstring forcelink1;
	static std::wstring forcelink2;
	static std::wstring not_begin;
	static std::wstring nextbreak1;
	static std::wstring numlink_nextbreak1;
	static std::wstring prevbreak2;

	static bool isForceBreak(wchar_t c) {return forcebreak.find(c) != std::wstring::npos;}
	static bool isForceLink1(wchar_t c1) {return forcelink1.find(c1) != std::wstring::npos;}
	static bool isForceLink2(wchar_t c2) {return forcelink2.find(c2) != std::wstring::npos;}
	static bool isNotBegin(wchar_t c2) {return not_begin.find(c2) != std::wstring::npos;}
	static bool isNextBreak1(wchar_t c) {return nextbreak1.find(c) != std::wstring::npos;}
	static bool isNumLink_NextBreak1(wchar_t c2) {return numlink_nextbreak1.find(c2) != std::wstring::npos;}
	static bool isPrevBreak2(wchar_t c) {return prevbreak2.find(c) != std::wstring::npos;}

	static bool isLink(wchar_t c1,wchar_t c2);

};


}//namespace Juna

