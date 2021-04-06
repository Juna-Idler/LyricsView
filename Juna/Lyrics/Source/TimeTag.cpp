
#include "../TimeTag.h"
#include <wchar.h>
#include <list>

namespace Juna {
namespace Lyrics {


namespace TimeTag
{

static std::wstring min_timetag = L"[00:00.00]";
static std::wstring max_timetag = L"[99:59.99]";


const std::wstring &MinTimeTag = min_timetag;

const std::wstring &MaxTimeTag = max_timetag;


static wchar_t decimal_point = '.';
const wchar_t &DecimalPoint = decimal_point;

void SetDecimalPointPeriod() { decimal_point = L'.'; }
void SetDecimalPointColon() { decimal_point = L':'; }


/*
static size_t taglength( const wchar_t *tag)
{
	if (tag[0] != L'[')
		return 0;
	size_t i;
	for (i = 1;tag[i] != L'\0';i++)
	{
		if (tag[i] == L']')
			return i + 1;
	}
	return i;
}
*/

static inline bool isdigit(int c) {return ('0' <= c) & (c <= '9');}

static const wchar_t *find_timetag(int &milisec,size_t &tag_length, const wchar_t *str)
{
	const wchar_t *f = ::wcschr(str,L'[');
	while (f != NULL)
	{
		milisec = timetag2milisec_length(tag_length,f);
		if (milisec >= 0)
		{
			return f;
		}
		f = ::wcschr(f + 1,L'[');
	}
	return NULL;
}

static size_t find_timetag(int &milisec,size_t &taglength, const std::wstring &str,size_t offset)
{
	offset = str.find(L'[',offset);
	while (offset != std::wstring::npos)
	{
		milisec = timetag2milisec_length(taglength,str.c_str() + offset);
		if (milisec >= 0)
		{
			return offset;
		}
		offset = str.find(L'[',offset + 1);
	}
	return std::wstring::npos;
}


int timetag2milisec( const std::wstring &timetag)
{
	if (timetag[0] == L'[' && isdigit(timetag[1]) && isdigit(timetag[2]) &&
		timetag[3] == L':' && isdigit(timetag[4]) && isdigit(timetag[5]))
	{
		int minute = (timetag[1] - '0') * 10 + timetag[2] - '0';
		int second = (timetag[4] - '0') * 10 + timetag[5] - '0';
		if ((timetag[6] == L'.' || timetag[6] == L':') &&
			isdigit(timetag[7]) && isdigit(timetag[8]) && timetag[9] == L']')
		{
			int milisec = (timetag[7] - '0') * 100 + (timetag[8] - '0') * 10;
			return (minute * 60 + second) * 1000 + milisec;
		}
		if (timetag[6] == L']')
		{
			return (minute * 60 + second) * 1000;
		}
	}
	return -1;
}

int timetag2milisec( const wchar_t *timetag)
{
	if (timetag[0] == L'[' && isdigit(timetag[1]) && isdigit(timetag[2]) &&
		timetag[3] == L':' && isdigit(timetag[4]) && isdigit(timetag[5]))
	{
		int minute = (timetag[1] - '0') * 10 + timetag[2] - '0';
		int second = (timetag[4] - '0') * 10 + timetag[5] - '0';
		if ((timetag[6] == L'.' || timetag[6] == L':') &&
			isdigit(timetag[7]) && isdigit(timetag[8]) && timetag[9] == L']')
		{
			int milisec = (timetag[7] - '0') * 100 + (timetag[8] - '0') * 10;
			return (minute * 60 + second) * 1000 + milisec;
		}
		if (timetag[6] == L']')
		{
			return (minute * 60 + second) * 1000;
		}
	}
	return -1;
}


int timetag2milisec_length(size_t &taglength, const wchar_t *timetag)
{
	if (timetag[0] == L'[' && isdigit(timetag[1]) && isdigit(timetag[2]) &&
		timetag[3] == L':' && isdigit(timetag[4]) && isdigit(timetag[5]))
	{
		int minute = (timetag[1] - '0') * 10 + timetag[2] - '0';
		int second = (timetag[4] - '0') * 10 + timetag[5] - '0';
		if ((timetag[6] == L'.' || timetag[6] == L':') &&
			isdigit(timetag[7]) && isdigit(timetag[8]) && timetag[9] == L']')
		{
			int milisec = (timetag[7] - '0') * 100 + (timetag[8] - '0') * 10;
			taglength = 10;
			return (minute * 60 + second) * 1000 + milisec;
		}
		if (timetag[6] == L']')
		{
			taglength = 7;
			return (minute * 60 + second) * 1000;
		}
	}
	taglength = 0;
	return -1;
}

int timetag2milisec_length(size_t &taglength, const std::wstring &timetag)
{
	if (timetag[0] == L'[' && isdigit(timetag[1]) && isdigit(timetag[2]) &&
		timetag[3] == L':' && isdigit(timetag[4]) && isdigit(timetag[5]))
	{
		int minute = (timetag[1] - '0') * 10 + timetag[2] - '0';
		int second = (timetag[4] - '0') * 10 + timetag[5] - '0';
		if ((timetag[6] == L'.' || timetag[6] == L':') &&
			isdigit(timetag[7]) && isdigit(timetag[8]) && timetag[9] == L']')
		{
			int milisec = (timetag[7] - '0') * 100 + (timetag[8] - '0') * 10;
			taglength = 10;
			return (minute * 60 + second) * 1000 + milisec;
		}
		if (timetag[6] == L']')
		{
			taglength = 7;
			return (minute * 60 + second) * 1000;
		}
	}
	taglength = 0;
	return -1;
}



static wchar_t m2t_timetag_tmp[11] = L"[00:00.00]";

const wchar_t *milisec2timetag(int milisec)
{
	if ( milisec < 0 )
		return L"";

	int minute = milisec / 1000 / 60;
	int second = milisec / 1000 % 60;
	int mili10 = milisec % 1000 / 10;
	m2t_timetag_tmp[1] = L'0' + minute / 10;
	m2t_timetag_tmp[2] = L'0' + minute % 10;
	m2t_timetag_tmp[4] = L'0' + second / 10;
	m2t_timetag_tmp[5] = L'0' + second % 10;
	m2t_timetag_tmp[6] = DecimalPoint;
	m2t_timetag_tmp[7] = L'0' + mili10 / 10;
	m2t_timetag_tmp[8] = L'0' + mili10 % 10;

	return m2t_timetag_tmp;
}

void milisec2timetag(std::wstring &timetag, int milisec)
{
	if ( milisec < 0 )
	{
		timetag = L"";
		return;
	}
	timetag.resize(10);

	int minute = milisec / 1000 / 60;
	int second = milisec / 1000 % 60;
	int mili10 = milisec % 1000 / 10;
	timetag[0] = L'[';
	timetag[1] = L'0' + minute / 10;
	timetag[2] = L'0' + minute % 10;
	timetag[3] = L':';
	timetag[4] = L'0' + second / 10;
	timetag[5] = L'0' + second % 10;
	timetag[6] = DecimalPoint;
	timetag[7] = L'0' + mili10 / 10;
	timetag[8] = L'0' + mili10 % 10;
	timetag[9] = L']';
}

void SeparateHeadTimeTagLine(Pair &dest, const wchar_t *line ,size_t length)
{
	size_t taglength;
	int time = timetag2milisec_length(taglength,line);
	if ( time < 0 )
	{
		dest.milisec = -1;
		dest.word.assign(line,length);
		return;
	}
	dest.milisec = time;
	dest.word.assign(line + taglength,length - taglength);
}

void SeparateHeadTimeTagLine(Pair &dest, const std::wstring &line )
{
	size_t length;
	int time = timetag2milisec_length(length,line);
	if ( time < 0 )
	{
		dest.milisec = -1;
		dest.word.assign(line);
		return;
	}
	dest.milisec = time;
	dest.word.assign( line.c_str() + length );
}

void SeparateHeadTimeTag(PairContainer &dest, const std::wstring &text )
{
	std::list<std::wstring> lines;
	int offset = 0;
	for (size_t i = 0;i < text.length();i++)
	{
		if (text[i] == L'\r')
		{
			lines.push_back(text.substr(offset,i - offset));
			if (text[i+1] == L'\n')
				i++;
			offset = i + 1;
			continue;
		}
		if (text[i] == L'\n')
		{
			lines.push_back(text.substr(offset,i - offset));
			offset = i + 1;
			continue;
		}
	}
	lines.push_back(text.substr(offset));
	dest.resize(lines.size());

	for (size_t i = 0;i < dest.size();i++)
	{
		const std::wstring &line = lines.front();
		SeparateHeadTimeTagLine(dest[i],line);
		lines.pop_front();
	}
}

void PickupHeadTimeTagLine(PairContainer &dest, const std::wstring &text )
{
	std::list<std::wstring> lines;
	int offset = 0;
	for (size_t i = 0;i < text.length();i++)
	{
		if (text[i] == L'\r')
		{
			std::wstring line = text.substr(offset,i - offset);
			if (timetag2milisec(line) >= 0)
			{
				lines.push_back(line);
			}
			if (text[i+1] == L'\n')
				i++;
			offset = i + 1;
			continue;
		}
		if (text[i] == L'\n')
		{
			std::wstring line = text.substr(offset,i - offset);
			if (timetag2milisec(line) >= 0)
			{
				lines.push_back(line);
			}
			offset = i + 1;
			continue;
		}
	}
	std::wstring line = text.substr(offset);
	if (timetag2milisec(line) >= 0)
	{
		lines.push_back(line);
	}

	dest.resize(lines.size());
	for (size_t i = 0;i < dest.size();i++)
	{
		const std::wstring &line = lines.front();
		SeparateHeadTimeTagLine(dest[i],line);
		lines.pop_front();
	}
}

void PickupHeadTimeTagLine(PairContainer &dest, const std::vector<std::wstring> &lines )
{
	std::vector<size_t> pickuplines;
	for (size_t i = 0;i < lines.size();i++)
	{
		if (timetag2milisec(lines[i]) >= 0)
		{
			pickuplines.push_back(i);
		}
	}
	dest.resize(pickuplines.size());
	for (size_t i = 0;i < dest.size();i++)
	{
		SeparateHeadTimeTagLine(dest[i],lines[pickuplines[i]]);
	}
}


void SeparateKaraokeLine(PairContainer &dest, const wchar_t *line )
{
	size_t length;
	int time = timetag2milisec_length(length,line);
	SeparateKaraokeLine(dest,time,line + length);
}

void SeparateKaraokeLine(PairContainer &dest, int head_milisec, const wchar_t *line )
{
	size_t count = 1;
	const wchar_t *find = line;
	int time;
	size_t length;
	while ((find = find_timetag(time,length,find)) != NULL)
	{
		count++;
		find += length;
	}
	dest.resize(count);
	if (count == 1)
	{
		dest[0].milisec = head_milisec;
		dest[0].word = line;
		return;
	}
	find = line;
	time = head_milisec;
	size_t i;
	for (i = 0; i < count - 1; i++)
	{
		dest[i].milisec = time;
		const wchar_t * next = find_timetag(time,length,find);
		dest[i].word.assign( find , next);
		find = next + length;
	}
	dest[i].milisec = time;
	dest[i].word.assign(find);

	return;
}


void SeparateKaraokeText(std::vector<PairContainer> &dest, const std::wstring &text )
{
	std::list<std::wstring> lines;
	int offset = 0;
	for (size_t i = 0;i < text.length();i++)
	{
		if (text[i] == L'\r')
		{
			lines.push_back(text.substr(offset,i - offset));
			if (text[i+1] == L'\n')
				i++;
			offset = i + 1;
			continue;
		}
		if (text[i] == L'\n')
		{
			lines.push_back(text.substr(offset,i - offset));
			offset = i + 1;
			continue;
		}
	}
	lines.push_back(text.substr(offset));
	dest.resize(lines.size());

	for (size_t i = 0;i < dest.size();i++)
	{
		const std::wstring &line = lines.front();
		SeparateKaraokeLine(dest[i],line.c_str());
		lines.pop_front();
	}
}


size_t RemoveTimeTag(std::wstring &dest,const wchar_t *text)
{
	dest.clear();
	const wchar_t *t = text;
	int milisec;
	size_t length;
	size_t count = 0;
	const wchar_t *f = find_timetag(milisec,length,t);
	while (f != NULL)
	{
		count++;
		dest.append(t,f - t);
		t = f + length;
		f = find_timetag(milisec,length,t);
	}
	dest.append(t);
	return count;
}
size_t RemoveTimeTag(std::wstring &text)
{
	int milisec;
	size_t length;
	size_t count = 0;
	size_t offset = find_timetag(milisec,length,text,0);
	while (offset != std::wstring::npos)
	{
		count++;
		text.erase(offset,length);
		offset = find_timetag(milisec,length,text,offset);
	}
	return count;
}

void RemoveKaraokeTag( std::wstring &dest,const std::wstring &text )
{
	std::vector<Pair> lines;
	SeparateHeadTimeTag(lines,text);

	dest.clear();
	dest.reserve(text.size());
	std::wstring temp;
	for (size_t i = 0; i < lines.size(); i++)
	{
		dest += lines[i].GetTimeTag();
		RemoveTimeTag(temp,lines[i].word.c_str());
		dest += temp;
		dest += L"\r\n";
	}
}

}//namespace TimeTag

}//namespace Lyrics
}//namespace Juna
