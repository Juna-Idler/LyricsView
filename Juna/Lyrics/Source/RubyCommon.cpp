#include "../RubyCommon.h"


namespace Juna {

void RubyLineBreakWords::SetWords(const RubyString &rubytext)
{
	words.clear();
	if (rubytext.GetString().size() == 0)
		return;

	const std::wstring &text = rubytext.GetString();

	RubyString::RubyPositions::const_iterator it =  rubytext.GetRubyPositions().begin();

	int current_offset = 0;
	int current_length = 1;
	for (size_t i = 1; i < text.size(); i++)
	{
		if (it != rubytext.GetRubyPositions().end())
		{
			if (i-1 == it->parent_offset)
			{
				if (it->parent_length >= 2)
				{
					current_length += it->parent_length - 1;
					i += it->parent_length - 2;
					++it;
					continue;
				}
				++it;
			}
		}
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

void RubyLineBreakWords::SetWords(const RubyString &rubytext, const Juna::Picture::OutlineFont &font)
{
	words.clear();
	if (rubytext.GetString().size() == 0)
		return;

	const std::wstring &text = rubytext.GetString();

	RubyString::RubyPositions::const_iterator it =  rubytext.GetRubyPositions().begin();

	int current_offset = 0;
	int current_length = 1;
	for (size_t i = 1; i < text.size(); i++)
	{
		if (it != rubytext.GetRubyPositions().end())
		{
			if (i-1 == it->parent_offset)
			{
				if (it->parent_length >= 2)
				{
					current_length += it->parent_length - 1;
					i += it->parent_length - 2;
					++it;
					continue;
				}
				++it;
			}
		}
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

	SetWidth(rubytext,font);
}


namespace Lyrics
{

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
bool lower_forward_match(const std::wstring &str,const wchar_t *lower_str,size_t length)
{
	if (str.length() < length)
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

/*
void AtTagConntainer::LoadAtTags(const std::wstring &text)
{
	tags.clear();
	size_t offset = 0;
	bool at_line = false;
	bool multiline = false;
	for (size_t i = 0;i < text.length();i++)
	{
		if (multiline)
		{
			if (text[i] == L'@' && (text[i-1] == L'\n' || text[i-1] == L'\r'))
			{
				tags.back().value.assign(text,offset,i - offset);
				multiline = false;
			}
			else
			{
				continue;
			}
		}
		if (text[i] == L'@')
		{
			if (i == offset)
			{
				at_line = true;
			}
			continue;
		}
		if (text[i] == L'\r' || text[i] == L'\n')
		{
			if (at_line)
			{
				tags.push_back(AtTag());
				if (!SeparateTag(tags.back(),text,offset,i - offset))
				{
					tags.pop_back();
				}
				if (tags.back().name.length() == 9) //"multiline"
				{
					multiline = true;
					wchar_t *multiline_begin = L"multiline";
					for (size_t i = 0;i < 9;i++)
					{
						if (::towlower(tags.back().name[i]) != multiline_begin[i])
						{
							multiline = false;
							break;
						}
					}
				}
				at_line = false;
			}
			if (text[i] == L'\r' && text[i+1] == L'\n')
				i++;
			offset = i + 1;
			continue;
		}
	}
	if (multiline)
	{
		tags.back().value.assign(text,offset,text.length() - offset);
	}
	if (at_line)
	{
		tags.push_back(AtTag());
		if (!SeparateTag(tags.back(),text,offset,text.length() - offset))
		{
			tags.pop_back();
		}
	}
}
*/
void AtTagContainer::LoadAtTags(const std::vector<std::wstring> &text)
{
	tags.clear();
	size_t startline = 0;
	bool at_line = false;
	bool multiline = false;
	std::wstring multiline_tag = L"";
	for (size_t i = 0;i < text.size();i++)
	{
		if (multiline)
		{
			if (text[i].length() != 0 && text[i][0] == L'@')
			{
				multiline = false;
			}
			else
			{
				tags.push_back(AtTag());
				tags.back().name = multiline_tag;
				tags.back().value = text[i];
				continue;
			}
		}
		if (text[i].length() != 0 && text[i][0] == L'@')
		{
			tags.push_back(AtTag());
			if (!SeparateTag(tags.back(),text[i]))
			{
				tags.pop_back();
			}
			else if (lower_forward_match(tags.back().name,L"multiline_",10))
			{
				multiline = true;
				multiline_tag = tags.back().name.substr(10);
				tags.pop_back();
			}
		}

	}
}



bool AtTagContainer::SeparateTag(AtTag &dest,const std::wstring &line)
{
	if (line.empty() || line[0] != '@')
		return false;
	size_t equal = line.find('=',1);
	if (equal == std::wstring::npos)
		return false;
	size_t end = equal;
	while (line[end - 1] == ' ' || line[end - 1] == '@'|| line[end - 1] == '\t')
		end--;
	dest.name.assign(line.c_str() + 1,end - 1);
	if (equal + 1 < line.size())
		dest.value.assign(line.c_str() + equal + 1);
	else
		dest.value = L"";
	return true;
}

bool AtTagContainer::SeparateTag(AtTag &dest,const std::wstring &text,size_t offset,size_t length)
{
	if (length == 0 || text[offset] != '@')
		return false;
	size_t equal = text.find('=',offset + 1);
	if (equal == std::wstring::npos)
		return false;
	size_t end = equal;
	while (text[end - 1] == ' ' || text[end - 1] == '@'|| text[end - 1] == '\t')
		end--;
	dest.name.assign(text,offset + 1,end - (offset + 1));
	if (equal + 1 < text.length())
		dest.value.assign(text,equal + 1,length - (equal + 1 - offset));
	else
		dest.value = L"";
	return true;
}


void RubyTag::LoadAtRubyTag(const AtTagContainer &AtTags)
{
	rubys.clear();
	rubyed.clear();
	std::wstring temp(16,L' ');
	for (AtTagContainer::Container::const_iterator it = AtTags.GetTags().begin();it != AtTags.GetTags().end();++it)
	{
		temp.resize(it->name.size());
		for (size_t i = 0;i < temp.size();i++)
		{
			temp[i] = ::towlower(it->name[i]);
		}
		if (temp.compare(L"ruby") == 0)
		{
			rubys.push_back(RubyingWord());
			if (!Parse(rubys.back(),it->value))
				rubys.pop_back();
		}
		else if (temp.compare(L"ruby_parent") == 0)
		{
			ruby_parent = it->value;
		}
		else if (temp.compare(L"ruby_begin") == 0)
		{
			ruby_begin = it->value;
		}
		else if (temp.compare(L"ruby_end") == 0)
		{
			ruby_end = it->value;
		}
		else if (temp.compare(L"rubyed") == 0)
		{
			rubyed.push_back(RubyString(it->value,ruby_parent,ruby_begin,ruby_end));
		}
	}

}

//@Ruby=[e•¶Žš]0,3[‚¨‚â‚à‚¶]
bool RubyTag::Parse(RubyingWord &dest,const std::wstring &line)
{
	size_t t_begin = line.find(L'[');
	if (t_begin == std::wstring::npos)
		return false;
	size_t t_end = line.find(L']',t_begin + 1);
	if (t_end == std::wstring::npos)
		return false;
	size_t r_begin = line.find(L'[',t_end + 1);
	if (r_begin == std::wstring::npos)
		return false;
	size_t r_end = line.find(L']',r_begin + 1);
	if (r_end == std::wstring::npos)
		return false;
	if (t_end - t_begin == 1 || r_end - r_begin == 1)
		return false;
	dest.target.assign(line.c_str() + t_begin + 1,t_end - (t_begin + 1));
	dest.ruby.assign(line.c_str() + r_begin + 1,r_end - (r_begin + 1));
	dest.parent_offset = 0;
	dest.parent_length = (unsigned short)dest.target.size();
	if (r_begin - t_end > 1)
	{
		size_t i = t_end + 1;
		while (line[i] == L' ')
			i++;
		if (::isdigit(line[i]))
		{
			unsigned offset = line[i] - '0';
			while (::isdigit(line[++i]))
			{
				offset = offset * 10 + line[i] - '0';
			}
			while (line[i] == L' ')
				i++;
			if (line[i] == L',' || line[i] == L'.')
			{
				i++;
				while (line[i] == L' ')
					i++;
				if (::isdigit(line[i]))
				{
					unsigned length = line[i] - '0';
					while (::isdigit(line[++i]))
					{
						length = length * 10 + line[i] - '0';
					}

					if (offset < dest.target.size())
						dest.parent_offset = offset;
					if (length != 0 && dest.parent_offset + length <= dest.target.size())
						dest.parent_length = length;
					else
						dest.parent_length = (unsigned short)dest.target.size() - dest.parent_offset;
				}
			}
		}
	}
	return true;
}

}//namesapce Lyrics
}//namespace Juna
