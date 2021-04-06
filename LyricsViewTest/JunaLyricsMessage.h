#pragma once

#include <Windows.h>
#include <string>


class JunaLyricsMessage
{
public:
	static const wchar_t * const RegisterMessageString;
	static const wchar_t * const DataFileMappingName;
	static const wchar_t * const DataMutexName;
	static const DWORD DataFileMappingMaxSize = 1024 * 64;

	static const wchar_t * const UserFileMappingName;
	static const wchar_t * const UserMutexName;
	static const DWORD UserFileMappingMaxSize = 256;

	static const unsigned MaxListenerCount = 4;

	enum enumEnterMode {EM_NoEnter,EM_Sender,EM_Listener};


	enum enumPlaybackEvent {
		PBE_Time = 0,
		PBE_New = 1,
		PBE_Stop = 2,
		PBE_Seek = 3,
		PBE_Pause = 4,
		PBE_PauseCancel = 5,
	};

private:
	UINT RegisterMessage;

	HANDLE hDataMutex;
	HANDLE hDataFileMapping;

	HANDLE hUserMutex;
	HANDLE hUserFileMapping;

	enumEnterMode EnterMode;

	HWND Listener;
	void ExitSender(void);
	void ExitListener(void);
public:
	enumEnterMode GetEnterMode(void) const {return EnterMode;}

	bool EnterSender(void);
	bool EnterListener(HWND listener);
	void Exit(void);

//Sender
	bool PostMessage(enumPlaybackEvent pb_event,unsigned int milisec,DWORD wait_ms);
	void PostMessageBroadCast(enumPlaybackEvent pb_event,unsigned int milisec);

	bool SetData(DWORD wait_ms,
		const wchar_t *path,size_t path_length,
		const wchar_t * title,size_t title_length,
		const wchar_t * artist,size_t artist_length,
		const wchar_t * album,size_t album_length,
		const wchar_t * genre,size_t genre_length,
		const wchar_t * date,size_t date_length,
		const wchar_t * comment,size_t comment_length);

//Listener
	struct MessageParam
	{
		enumPlaybackEvent pb_event;
		unsigned int milisec;
	};
	bool TestMessage(MessageParam &dest,UINT uMsg,WPARAM w,LPARAM l) const
	{
		if (uMsg != RegisterMessage)
			return false;
		dest.pb_event = (enumPlaybackEvent)w;
		dest.milisec = l;
		return true;
	}

	bool GetData(std::wstring &path,
		std::wstring &title,std::wstring &artist,std::wstring &album,
		std::wstring &genre,std::wstring &date,std::wstring &comment,
		DWORD wait_ms = INFINITE);


	JunaLyricsMessage(void);
	~JunaLyricsMessage();
};

