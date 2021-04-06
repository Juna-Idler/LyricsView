
#include <windows.h>
#include <crtdbg.h>


BOOL WINAPI DllMain(HINSTANCE,// hinstDLL,	// handle to DLL module
					DWORD fdwReason,	// reason for calling function
					LPVOID)// lpvReserved)	// reserved
{
    switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

//			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
			break;
		case DLL_PROCESS_DETACH:
//			_CrtDumpMemoryLeaks();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
    }
    return TRUE;
}

