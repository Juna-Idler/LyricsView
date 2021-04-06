#include "TextFile.h"

#include <Windows.h>

#include <vector>

bool TextFileReader::Open(const wchar_t *filename)
{
	HANDLE hfile;
	hfile = ::CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
						 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	if (hfile == INVALID_HANDLE_VALUE )
	{
		return false;
	}
	FileName = filename;

	DWORD size = ::GetFileSize(hfile,NULL);

	DWORD read;
	unsigned char bom[3] = {0,0,0};
	::ReadFile(hfile,bom,3,&read,NULL);

    if (bom[0] == 0xFF && bom[1] == 0xFE)		//utf-16 LE
    {
		::SetFilePointer(hfile,2,NULL,FILE_BEGIN);
		TextData.resize((size - 2) / 2);
		::ReadFile(hfile,&TextData[0],TextData.size(),&read,NULL);
    }
    else if (bom[0] == 0xFE && bom[1] == 0xFF)	//utf-16 BE
    {
		::SetFilePointer(hfile,2,NULL,FILE_BEGIN);
		TextData.resize((size - 2) / 2);
		::ReadFile(hfile,&TextData[0],TextData.size(),&read,NULL);
		for (size_t i = 0; i < TextData.size(); i++)
		{
			unsigned short c = TextData[i];
			TextData[i] = ((c >> 8) & 0xFF) | ((c << 8) & 0xFF00);
		}
	}
	else if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF ) // utf-8
	{
		std::vector<char> buffer(size - 3);
		::ReadFile(hfile,&buffer[0],buffer.size(),&read,NULL);
		int wsize = ::MultiByteToWideChar(CP_UTF8,0,&buffer[0],buffer.size(),NULL,0);
		if (wsize == 0)
		{
			::CloseHandle(hfile);
			return false;
		}
		TextData.resize(wsize);
		::MultiByteToWideChar(CP_UTF8,0,&buffer[0],buffer.size(),&TextData[0],wsize);
	}
	else	//shift-jis‚ÆŒ¾‚¤‚±‚Æ‚É‚·‚é
	{
		::SetFilePointer(hfile,0,NULL,FILE_BEGIN);
		std::vector<char> buffer(size);
		::ReadFile(hfile,&buffer[0],buffer.size(),&read,NULL);
		int wsize = ::MultiByteToWideChar( 932 ,0,&buffer[0],buffer.size(),NULL,0);
		if (wsize == 0)
		{
			::CloseHandle(hfile);
			return false;
		}
		TextData.resize(wsize);
		::MultiByteToWideChar( 932 ,0,&buffer[0],buffer.size(),&TextData[0],wsize);

	}

	::CloseHandle(hfile);
	return true;
}




