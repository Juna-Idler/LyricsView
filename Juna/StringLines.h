#pragma once

#include <string>
#include <vector>

namespace Juna {

	
class StringLines
{
public:
	typedef std::vector<std::wstring> LineContainer;

private:
	LineContainer lines;

public:
	const LineContainer &Lines(void) const {return lines;}

	const std::wstring &operator[](size_t i) const {return lines[i];}
	size_t size(void) const {return lines.size();}

	StringLines(void) : lines() {}
	StringLines(const std::wstring &text) : lines() {SetString(text);}
	~StringLines() {}

	void SetString(const std::wstring &text);
};


}//namespace Juna
