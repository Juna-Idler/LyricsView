#pragma once

/*
class base_ThreadClass
{
protected:
	unsigned int ThreadProc(void);
};
*/

#include <Windows.h>
#include <process.h>

template<class base_Class>
class ThreadClass : public base_Class
{
	HANDLE hThread;
public:
	ThreadClass(void) : hThread(NULL) {}
	~ThreadClass() {CloseThreadHandle();}

	void CloseThreadHandle(void)
	{
		if (hThread)
			::CloseHandle(hThread);
		hThread = NULL;
	}

	bool CreateThread(void *sequrity = NULL,unsigned int stack_size = 0,bool suspend = false,unsigned int *lpThreadId = NULL)
	{
		hThread = (HANDLE)::_beginthreadex(sequrity,stack_size,staticThreadProc,this,
						suspend ? CREATE_SUSPENDED : 0,lpThreadId);
		return hThread != NULL;
	}
	void ResumeThread(void)
	{
		::ResumeThread(hThread);
	}

	bool WaitThread(unsigned int milisec = INFINITE)
	{
		DWORD r = ::WaitForSingleObject( hThread, milisec );
		if (r == WAIT_OBJECT_0)
			return true;
		return false;
	}

private:
	static unsigned int __stdcall staticThreadProc(void *lpParameter)
	{
		ThreadClass *this_ = (ThreadClass *)lpParameter;
		return this_->ThreadProc();
	}
};

class CriticalSection
{
private:
	::CRITICAL_SECTION critical_section;
public:
	CriticalSection(void) 
	{
		::InitializeCriticalSection(&critical_section);
	}
	~CriticalSection()
	{
		::DeleteCriticalSection(&critical_section);
	}

	friend class Synchronizer;
	class Synchronizer
	{
	public:
		Synchronizer(CriticalSection &cs) : Locker(cs.critical_section)
			{::EnterCriticalSection(&Locker);}
		~Synchronizer(void) {::LeaveCriticalSection(&Locker);}

	private:
		CRITICAL_SECTION &Locker;
	};
};

