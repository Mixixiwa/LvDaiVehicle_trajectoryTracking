#include "threadCom.h"

THREADCOM::THREADCOM()
{
	m_hThread = NULL;
	m_bExit = false;
	m_dwParam = 0;
	m_idThread = 0;
}

THREADCOM::~THREADCOM()
{
	if (!m_bExit)
	{
		StopThread();
	}
}

void THREADCOM::StartThread()
{
	m_hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, this, 0, &m_idThread);
}

DWORD THREADCOM::ThreadProc(LPVOID pParam)
{
	THREADCOM *pThis = (THREADCOM*)pParam;
	while (!pThis->m_bExit)
	{
		pThis->Run();
	}
	return true;
}

void THREADCOM::StopThread()
{
	if (m_hThread)
	{
		m_bExit = true;
		::WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

void THREADCOM::Run()
{
	Sleep(70);
}

void THREADCOM::SetThreadData(DWORD dwParam)
{
	if (m_dwParam != dwParam)
	{
		m_dwParam = dwParam;
	}
}

DWORD THREADCOM::GetThreadData()
{
	return m_dwParam;
}
