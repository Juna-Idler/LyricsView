
#include "../RubyString.h"

#include<algorithm>


namespace Juna {

size_t RubyString::AssignString(const std::wstring &source,
						const std::wstring &parent_mark,const std::wstring &begin_mark,const std::wstring &end_mark)
{
	positions.clear();
	if (parent_mark.empty() || begin_mark.empty() || end_mark.empty())
	{
		parent = phonetic = source;
		return 0;
	}

	size_t offset = 0;
	for (size_t i = 0; ; i++)
	{
		size_t e = source.find(end_mark,offset);
		if (e != std::wstring::npos)
		{
			size_t b = source.rfind(begin_mark,e - begin_mark.size());
			if (b != std::wstring::npos && b >= offset)
			{
				size_t p = source.rfind(parent_mark,b - parent_mark.size());
				if (p != std::wstring::npos && p >= offset)
				{
					parent.append(source.c_str() + offset, p - offset);
					phonetic.append(source.c_str() + offset, p - offset);

					size_t parent_offset = parent.size();
					size_t source_parent_offset = p + parent_mark.size();
					parent.append(source.c_str() + source_parent_offset,b - source_parent_offset);
					size_t parent_length = parent.size() - parent_offset;

					size_t phonetic_offset = phonetic.size();
					size_t souce_phonetic_offset = b + begin_mark.size();
					phonetic.append(source.c_str() + souce_phonetic_offset,e - souce_phonetic_offset);
					size_t phonetic_length = phonetic.size() - phonetic_offset;

					positions.push_back(RubyPosition());
					RubyPosition &rp = positions.back();
					rp.parent_offset = (unsigned short)parent_offset;
					rp.parent_length = (unsigned short)parent_length;
					rp.phonetic_offset = (unsigned short)phonetic_offset;
					rp.phonetic_length = (unsigned short)phonetic_length;

					offset = e + end_mark.size();
					continue;
				}
			}
		}
		parent += source.c_str() + offset;
		phonetic += source.c_str() + offset;
		return i;
	}
}

size_t RubyString::GetRubyInfo(std::vector<RubyInfo> &dest,
						const std::wstring &source,
						const std::wstring &parent_mark,const std::wstring &begin_mark,const std::wstring &end_mark)
{
	dest.clear();
	if (parent_mark.empty() || begin_mark.empty() || end_mark.empty())
		return 0;

	size_t offset = 0;
	for (size_t i = 0; ; i++)
	{
		size_t b = source.find(begin_mark,offset);
		if (b != std::wstring::npos)
		{
			size_t p = source.rfind(parent_mark,b - parent_mark.size());
			if (p != std::wstring::npos && p >= offset)
			{
				size_t e = source.find(end_mark,b + begin_mark.size());
				if (e != std::wstring::npos)
				{
					dest.push_back(RubyInfo(p,p+parent_mark.size(),b,b + begin_mark.size(),e,e + end_mark.size()));
					
					offset = e + end_mark.size();
					continue;
				}
			}
		}
		return i;
	}
}



namespace {

bool isCollision(size_t a_offset,size_t a_length,RubyString::RubyPositions::iterator b)
{
	size_t a_point = a_offset + a_offset + a_length;
	size_t b_point = b->parent_offset + b->parent_offset + b->parent_length;

	size_t distance = (a_point > b_point) ? a_point - b_point : b_point - a_point;
	size_t range = a_length + b->parent_length;
	return distance < range;
}

struct lower_bound_compare
{
	bool operator()(const RubyString::RubyPosition& lhs, size_t rhs) const { return lhs.parent_offset < rhs; }

	bool operator()(size_t rhs, const RubyString::RubyPosition& lhs) const { return rhs < lhs.parent_offset; }

	bool operator()(const RubyString::RubyPosition& lhs,const RubyString::RubyPosition& rhs) const
	{ return lhs.parent_offset < rhs.parent_offset; }
};

}



size_t RubyString::AddRuby(const RubyingWord &word)
{
	size_t count = 0;
	size_t find_offset = parent.find(word.target,0);
	while (find_offset != std::wstring::npos)
	{
		size_t offset = find_offset + word.parent_offset;
		size_t length = word.parent_length;

		RubyPositions::iterator it = std::lower_bound(positions.begin(),positions.end(),offset,lower_bound_compare());
		if (it == positions.end() || !isCollision(offset,length,it))
		{
			RubyPositions::iterator prev = it;
			if (it == positions.begin() || !isCollision(offset,length,--prev))
			{
				int sub = word.ruby.length() - word.parent_length;
				unsigned short phonetic_offset = (unsigned short)((it == positions.begin()) ? offset : prev->phonetic_offset + prev->phonetic_length + (offset - (prev->parent_offset + prev->parent_length)));
				for (RubyPositions::iterator i = it;i != positions.end();++i)
				{
					i->phonetic_offset += sub;
				}
				phonetic.replace(phonetic_offset,length,word.ruby);
				RubyPositions::iterator ins = positions.insert(it,RubyString::RubyPosition());
				ins->Set((unsigned short)offset, (unsigned short)length, (unsigned short)phonetic_offset, (unsigned short)word.ruby.length());
			}
		}
		find_offset = parent.find(word.target,find_offset + 1);
	}

	return count;
}



}//namespace Juna
