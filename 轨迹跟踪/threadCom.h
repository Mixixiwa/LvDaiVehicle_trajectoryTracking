#pragma once
#include <Windows.h>
#include <vector>
#include <mutex>
using namespace std;
class THREADCOM
{
public:
	THREADCOM();
	~THREADCOM();

public:	//参数
	HANDLE m_hThread;
	bool m_bExit;//线程退出标志位
	DWORD m_dwParam; //通知线程传入什么参数

public: //函数
	void StartThread();
	void StopThread();

	virtual void SetThreadData(DWORD dwParam);
	virtual DWORD GetThreadData();

	virtual void Run();

	static DWORD ThreadProc(LPVOID pParam);//没有类型的类型，用时转换
private:
	DWORD m_idThread;
};
