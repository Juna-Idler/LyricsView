
#include "JunaLyricsMessage.h"

#pragma comment(lib,"winmm.lib")


const wchar_t * const JunaLyricsMessage::RegisterMessageString = L"Juna Lyrics Message Broadcast";
const wchar_t * const JunaLyricsMessage::DataFileMappingName = L"Juna Lyrics Message Data";
const wchar_t * const JunaLyricsMessage::DataMutexName = L"Juna Lyrics Message Data Mutex";

const wchar_t * const JunaLyricsMessage::UserFileMappingName = L"Juna Lyrics Message User";
const wchar_t * const JunaLyricsMessage::UserMutexName = L"Juna Lyrics Message User Mutex";

namespace {
class MutexLock
{
public:
	HANDLE hMutex;
	DWORD ret;

	MutexLock(HANDLE hmutex,DWORD milisec)
	{
		hMutex = hmutex;
		ret = ::WaitForSingleObject(hmutex,milisec);
	}
	MutexLock(HANDLE hmutex)
	{
		hMutex = hmutex;
		ret = ::WaitForSingleObject(hmutex,INFINITE);
	}
	~MutexLock()
	{
		::ReleaseMutex(hMutex);
	}
};
}//anonymous namespace


JunaLyricsMessage::JunaLyricsMessage(void) : RegisterMessage(WM_NULL),
		hDataMutex(NULL), hDataFileMapping(NULL),
		hUserMutex(NULL), hUserFileMapping(NULL),
		EnterMode(EM_NoEnter),
		Listener(NULL)
{
	RegisterMessage = ::RegisterWindowMessageW(RegisterMessageString);
	::timeBeginPeriod(1);
}


JunaLyricsMessage::~JunaLyricsMessage(void)
{
	Exit();
	::timeEndPeriod(1);
}

void JunaLyricsMessage::Exit(void)
{
	switch (EnterMode)
	{
	case EM_NoEnter:
		break;
	case EM_Sender:
		ExitSender();
		break;
	case EM_Listener:
		ExitListener();
		break;
	}
}

bool JunaLyricsMessage::EnterSender()
{
	Exit();

	hUserMutex = ::CreateMutexW(NULL,FALSE,UserMutexName);
	if (hUserMutex == NULL)
		return false;
	{
		MutexLock ml(hUserMutex);
		hUserFileMapping = ::CreateFileMappingW(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,UserFileMappingMaxSize,UserFileMappingName);
		if (hUserFileMapping == NULL)
		{
			ExitSender();
			return false;
		}
		DWORD *address = static_cast<DWORD *>(::MapViewOfFile(hUserFileMapping,FILE_MAP_ALL_ACCESS,0,0,0));
		if (address == NULL)
		{
			ExitSender();
			return false;
		}
		if (::GetLastError() == ERROR_ALREADY_EXISTS)
		{
			BYTE &user = *reinterpret_cast<BYTE *>(address);
			if (user != 0)
			{
				::UnmapViewOfFile(address);
				::CloseHandle(hUserFileMapping);
				hUserFileMapping = NULL;
				ExitSender();
				return false;
			}
			user = 1;
		}
		else
		{
			BYTE &user = *reinterpret_cast<BYTE *>(address);
			DWORD &size = *(address + 1);
			user = 1;
			size = 0;
		}
		::UnmapViewOfFile(address);
	}

	hDataMutex = ::CreateMutexW(NULL,FALSE,DataMutexName);
	if (hDataMutex == NULL)
	{
		ExitSender();
		return false;
	}

	{
		MutexLock ml(hDataMutex);
		hDataFileMapping = ::CreateFileMappingW(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,DataFileMappingMaxSize,DataFileMappingName);
		if (hDataFileMapping == NULL)
		{
			ExitSender();
			return false;
		}
	}
	EnterMode = EM_Sender;
	return true;
}

void JunaLyricsMessage::ExitSender(void)
{
	if (hDataFileMapping != NULL)
	{
		::CloseHandle(hDataFileMapping);
		hDataFileMapping = NULL;
	}
	if (hDataMutex != NULL)
	{
		::CloseHandle(hDataMutex);
		hDataMutex = NULL;
	}
	if (hUserFileMapping != NULL)
	{
		MutexLock ml(hUserMutex);
		DWORD* address = static_cast<DWORD*>(::MapViewOfFile(hUserFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0));
		if (address != NULL)
		{
			DWORD& user = *(address);
			user = 0;
			::UnmapViewOfFile(address);
		}
		::CloseHandle(hUserFileMapping);
		hUserFileMapping = NULL;
	}
	if (hUserMutex != NULL)
	{
		::CloseHandle(hUserMutex);
		hUserMutex = NULL;
	}
	EnterMode = EM_NoEnter;
}


bool JunaLyricsMessage::EnterListener(HWND listener)
{
	Exit();

	hUserMutex = ::CreateMutexW(NULL,FALSE,UserMutexName);
	if (hUserMutex == NULL)
		return false;
	{
		MutexLock ml(hUserMutex);
		hUserFileMapping = ::CreateFileMappingW(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,UserFileMappingMaxSize,UserFileMappingName);
		if (hUserFileMapping == NULL)
		{
			ExitListener();
			return false;
		}
		bool exists = ::GetLastError() == ERROR_ALREADY_EXISTS;
		DWORD *address = static_cast<DWORD *>(::MapViewOfFile(hUserFileMapping,FILE_MAP_ALL_ACCESS,0,0,0));
		if (address == NULL)
		{
			ExitListener();
			return false;
		}
		if (exists)
		{
			DWORD &size = *(address + 1);
			if (size > MaxListenerCount)
			{
				::UnmapViewOfFile(address);
				ExitListener();
				return false;
			}
			HWND *hwndarray = reinterpret_cast<HWND *>(address + 2);
			hwndarray[size] = listener;
			size++;
		}
		else
		{
			DWORD &user = *(address);
			DWORD &size = *(address + 1);
			HWND *hwndarray = reinterpret_cast<HWND *>(address + 2);
			user = 0;
			size = 1;
			hwndarray[0] = listener;
		}
		::UnmapViewOfFile(address);
		Listener = listener;
	}

	hDataMutex = ::CreateMutexW(NULL,FALSE,DataMutexName);
	if (hDataMutex == NULL)
	{
		ExitListener();
		return false;
	}
	hDataFileMapping = ::OpenFileMappingW(FILE_MAP_READ,FALSE,DataFileMappingName);

	EnterMode = EM_Listener;
	return true;
}
void JunaLyricsMessage::ExitListener(void)
{
	if (hDataFileMapping != NULL)
	{
		::CloseHandle(hDataFileMapping);
		hDataFileMapping = NULL;
	}
	if (hDataMutex != NULL)
	{
		::CloseHandle(hDataMutex);
		hDataMutex = NULL;
	}
	if (hUserFileMapping != NULL)
	{
		MutexLock ml(hUserMutex);
		DWORD *address = static_cast<DWORD *>(::MapViewOfFile(hUserFileMapping,FILE_MAP_ALL_ACCESS,0,0,0));
		if (address != NULL)
		{
			DWORD &size = *(address + 1);
			HWND *hwndarray = reinterpret_cast<HWND *>(address + 2);
			for (unsigned i = 0;i < size ;i++)
			{
				if (Listener == hwndarray[i])
				{
					::memmove(hwndarray + i,hwndarray + i + 1,size - i - 1);
					size--;
					break;
				}
			}
			::UnmapViewOfFile(address);
			Listener = NULL;
		}
		::CloseHandle(hUserFileMapping);
		hUserFileMapping = NULL;
	}
	if (hUserMutex != NULL)
	{
		::CloseHandle(hUserMutex);
		hUserMutex = NULL;
	}

	EnterMode = EM_NoEnter;
}



bool JunaLyricsMessage::PostMessage(enumPlaybackEvent  pb_event,unsigned int milisec,DWORD wait_ms)
{
	MutexLock ml(hUserMutex,wait_ms);
	if (ml.ret == WAIT_OBJECT_0)
	{
		DWORD *address = static_cast<DWORD *>(::MapViewOfFile(hUserFileMapping,FILE_MAP_READ,0,0,0));
		if (address)
		{
			DWORD& size = *(address + 1);
			HWND* hwnd = reinterpret_cast<HWND*>(address + 2);
			for (unsigned i = 0; i < size; i++)
			{
				::PostMessageW(hwnd[i], RegisterMessage, pb_event, milisec);
			}
			::UnmapViewOfFile(address);
		}
		return true;
	}
	return false;
}

void JunaLyricsMessage::PostMessageBroadCast(enumPlaybackEvent pb_event,unsigned int milisec)
{
	::PostMessageW(HWND_BROADCAST,RegisterMessage,pb_event,milisec);
}




bool JunaLyricsMessage::SetData(DWORD wait_ms,
		const wchar_t *path,size_t path_length,
		const wchar_t * title,size_t title_length,
		const wchar_t * artist,size_t artist_length,
		const wchar_t * album,size_t album_length,
		const wchar_t * genre,size_t genre_length,
		const wchar_t * date,size_t date_length,
		const wchar_t * comment,size_t comment_length)
{
	const size_t char_size = sizeof(wchar_t);
	DWORD size = 4 + (2 + path_length * char_size) +
				(2 + title_length * char_size) + (2 + artist_length * char_size) + (2 + album_length * char_size) +
				(2 + genre_length * char_size) + (2 + date_length * char_size) + (2 + comment_length * char_size);

	MutexLock ml(hDataMutex,wait_ms);
	if (ml.ret != WAIT_OBJECT_0)
		return false;

	BYTE *address = static_cast<BYTE *>(::MapViewOfFile(hDataFileMapping,FILE_MAP_WRITE,0,0,size));
	if (address == NULL)
		return false;

	size_t offset = 0;
	*(DWORD *)(address + offset) = size; offset += 4;
	*(WORD *)(address + offset) = path_length * char_size; offset += 2;
	::memcpy(address + offset,path,path_length * 2); offset += path_length * char_size;

	*(WORD *)(address + offset) = title_length * char_size; offset += 2;
	::memcpy(address + offset,title,title_length * char_size); offset += title_length * char_size;

	*(WORD *)(address + offset) = artist_length * char_size; offset += 2;
	::memcpy(address + offset,artist,artist_length * char_size); offset += artist_length * char_size;

	*(WORD *)(address + offset) = album_length * char_size; offset += 2;
	::memcpy(address + offset,album,album_length * char_size); offset += album_length * char_size;

	*(WORD *)(address + offset) = genre_length * char_size; offset += 2;
	::memcpy(address + offset,genre,genre_length * char_size); offset += genre_length * char_size;

	*(WORD *)(address + offset) = date_length * char_size; offset += 2;
	::memcpy(address + offset,date,date_length * char_size); offset += date_length * char_size;

	*(WORD *)(address + offset) = comment_length * char_size; offset += 2;
	::memcpy(address + offset,comment,comment_length * char_size); offset += comment_length * char_size;

	::UnmapViewOfFile(address);

	return true;
}


bool JunaLyricsMessage::GetData(std::wstring &path,
								std::wstring &title,std::wstring &artist,std::wstring &album,
								std::wstring &genre,std::wstring &date,std::wstring &comment,
								DWORD wait_ms)
{
	MutexLock ml(hDataMutex,wait_ms);
	if (ml.ret != WAIT_OBJECT_0)
		return false;

	if (hDataFileMapping == NULL)
	{
		hDataFileMapping = ::OpenFileMappingW(FILE_MAP_READ,FALSE,DataFileMappingName);
		if (hDataFileMapping == NULL)
			return false;
	}

	BYTE *address = static_cast<BYTE *>(::MapViewOfFile(hDataFileMapping,FILE_MAP_READ,0,0,0));
	if (address == NULL)
		return false;

	const size_t char_size = sizeof(wchar_t);
	size_t offset = 0;
	DWORD size = *(DWORD *)(address + offset); offset += 4;
	WORD string_size;


	string_size = *(WORD *)(address + offset); offset += 2;
	path.resize(string_size / char_size);
	if (string_size)
		::memcpy(&path[0],address + offset,string_size); offset += string_size;

	string_size = *(WORD *)(address + offset); offset += 2;
	title.resize(string_size / char_size);
	if (string_size)
		::memcpy(&title[0],address + offset,string_size); offset += string_size;

	string_size = *(WORD *)(address + offset); offset += 2;
	artist.resize(string_size / char_size);
	if (string_size)
		::memcpy(&artist[0],address + offset,string_size); offset += string_size;

	string_size = *(WORD *)(address + offset); offset += 2;
	album.resize(string_size / char_size);
	if (string_size)
		::memcpy(&album[0],address + offset,string_size); offset += string_size;

	string_size = *(WORD *)(address + offset); offset += 2;
	genre.resize(string_size / char_size);
	if (string_size)
		::memcpy(&genre[0],address + offset,string_size); offset += string_size;

	string_size = *(WORD *)(address + offset); offset += 2;
	date.resize(string_size / char_size);
	if (string_size)
		::memcpy(&date[0],address + offset,string_size); offset += string_size;

	string_size = *(WORD *)(address + offset); offset += 2;
	comment.resize(string_size / char_size);
	if (string_size)
		::memcpy(&comment[0],address + offset,string_size); offset += string_size;

	::UnmapViewOfFile(address);
	return true;
}
