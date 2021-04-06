#pragma once

#include <string>

class TextFileReader
{
public:
	TextFileReader(void) {}
	TextFileReader(const wchar_t *filename) {Open(filename);}
	~TextFileReader(void) {}

	bool Open(const wchar_t *filename);

	const std::wstring &GetFileName(void) const {return FileName;}

	const std::wstring &GetString(void) const {return TextData;}

private:
	std::wstring FileName;
	std::wstring TextData;
};

