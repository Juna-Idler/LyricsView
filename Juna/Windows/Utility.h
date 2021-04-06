

#ifndef Juna_Windows_Utility_H
#define Juna_Windows_Utility_H

namespace Juna {
namespace Windows {

class DLLHandle
{
public:
	DLLHandle(void) : hDll(0) {}
	~DLLHandle(){Free();}
	bool Load(const wchar_t *filename);
	bool isLoaded(void) {return hDll != 0;}
	void Free(void);
	void Set(void *hdll) {hDll = hdll;}
	void *ReleaseHandle(void) {void *r = hDll;hDll = 0;return r;}
	void *GetProcAdress(const char *proc_name);


	void *GetHandle(void) {return hDll;}

private:
	void *hDll;
};

void *GetMyWindowHandle(void);
void AdjustWindowClientRect(void *hwnd,int client_width,int client_height);

}//namespace Windows
}//namespace Juna

//include "Source/Utility.cpp"
#endif
