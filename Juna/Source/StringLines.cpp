
#include "../StringLines.h"



namespace Juna {

void StringLines::SetString(const std::wstring &text)
{
	struct pair
	{
		size_t start;
		size_t end;
		pair(size_t s,size_t e) : start(s),end(e) {}
		size_t length(void) {return end - start;}
	};
	std::vector<pair> line_char_pos;

	size_t linehead = 0;
	for (size_t i = 0;i < text.length();i++)
	{
		if (text[i] == L'\r' || text[i] == L'\n')
		{
			line_char_pos.push_back(pair(linehead,i));
			if (text[i] == L'\r' && text[i+1] == L'\n')
				i++;
			linehead = i + 1;
		}
	}
	line_char_pos.push_back(pair(linehead,text.length()));

	lines.clear();
	lines.resize(line_char_pos.size());
	for (size_t i = 0;i < line_char_pos.size();i++)
	{
		if (line_char_pos[i].length() == 0)
			lines[i].assign(L"");
		else
			lines[i].assign(text,line_char_pos[i].start,line_char_pos[i].length());
	}

}


}//namespace Juna
