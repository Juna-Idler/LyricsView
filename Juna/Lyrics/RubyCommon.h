
#pragma once

#include <string>
#include <vector>
#include <list>

#include "../LineBreak.h"
#include "../RubyString.h"

namespace Juna {

class RubyLineBreakWords : private LineBreakWords
{
public:
	using LineBreakWords::GetWords;

	void Clear(void) {words.clear();}

	void SetWords(const RubyString &text);
	void SetWords(const RubyString &text, const Juna::Picture::OutlineFont &font);
	void SetWidth(const RubyString &text, const Juna::Picture::OutlineFont &font)
	{
		LineBreakWords::SetWidth(text.GetString(),font);
	}

	using LineBreakWords::CountLine;


	RubyLineBreakWords(void) {}
	~RubyLineBreakWords() {}

};


namespace Lyrics
{

class AtTagContainer
{
public:
	struct AtTag
	{
		std::wstring name;
		std::wstring value;
	};
	typedef std::list<AtTag> Container;

public:
	void LoadAtTags(const std::vector<std::wstring> &text);
//	void LoadAtTags(const std::wstring &text);
	const Container &GetTags(void) const {return tags;}

public:
	static bool SeparateTag(AtTag &dest,const std::wstring &line);
	static bool SeparateTag(AtTag &dest,const std::wstring &text,size_t offset,size_t length);

private:
	Container tags;
};


class RubyTag
{
public:
	typedef std::list<RubyingWord> RubyContainer;
	typedef std::list<RubyString> RubyedContainer;
private:
	RubyContainer rubys;
	RubyedContainer rubyed;

	std::wstring ruby_parent;
	std::wstring ruby_begin;
	std::wstring ruby_end;

public:
	const std::wstring &parent(void) const {return ruby_parent;}
	const std::wstring &begin(void) const {return ruby_begin;}
	const std::wstring &end(void) const {return ruby_end;}

	RubyTag(void) : ruby_parent(L""),ruby_begin(L""),ruby_end(L"") {}

	void LoadAtRubyTag(const AtTagContainer &AtTags);

	const RubyContainer &GetRubys(void) const {return rubys;}
	const RubyedContainer &GetRubyed(void) const {return rubyed;}

	RubyContainer &GetRubys(void) {return rubys;}
	RubyedContainer &GetRubyed(void) {return rubyed;}

public:
//@Ruby=[êeï∂éö]0,3[Ç®Ç‚Ç‡Ç∂]
	bool Parse(RubyingWord &dest,const std::wstring &line);

};


}//namesapce Lyrics

}//namespace Juna
