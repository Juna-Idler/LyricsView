

#include "JunaLyricsMessage.h"

static JunaLyricsMessageListener Listener;



bool __stdcall JLM_Initialize(HWND hwnd)
{
	return Listener.Initialize(hwnd);
}

void __stdcall JLM_Terminalize(void)
{
	Listener.Terminalize();
}

bool __stdcall JLM_ChangeWindowHandle(HWND hwnd)
{
	return Listener.ChangeWindowHandle(hwnd);
}


//Listener

UINT __stdcall JLM_GetMessageValue(void)
{
	return Listener.GetMessageValue();
}

static JunaLyricsMessageListener::MessageParam mparam;

bool __stdcall JLM_TestMessage(UINT uMsg,WPARAM w,LPARAM l)
{
	return Listener.TestMessage(mparam,uMsg,w,l);
}
JunaLyricsMessage::enumPlaybackEvent __stdcall JLM_GetMessagePBEvent(void) {return mparam.pb_event;}
unsigned int __stdcall JLM_GetMessageTime(void) {return mparam.milisec;}

static unsigned int total_milisec;
static std::wstring path;
static std::wstring title;
static std::wstring artist;
static std::wstring album;
static std::wstring genre;
static std::wstring date;
static std::wstring comment;


bool __stdcall JLM_GetData(unsigned int wait_ms)
{
	return Listener.GetData(total_milisec,path,title,artist,album,genre,date,comment,wait_ms);
}

unsigned int __stdcall JLM_GetLength(void) {return total_milisec;}
const wchar_t * __stdcall JLM_GetPath(void) {return path.c_str();}
const wchar_t * __stdcall JLM_GetTitle(void) {return title.c_str();}
const wchar_t * __stdcall JLM_GetArtist(void) {return artist.c_str();}
const wchar_t * __stdcall JLM_GetAlbum(void) {return album.c_str();}
const wchar_t * __stdcall JLM_GetGenre(void) {return genre.c_str();}
const wchar_t * __stdcall JLM_GetDate(void) {return date.c_str();}
const wchar_t * __stdcall JLM_GetComment(void) {return comment.c_str();}
