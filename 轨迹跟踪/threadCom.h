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

public:	//����
	HANDLE m_hThread;
	bool m_bExit;//�߳��˳���־λ
	DWORD m_dwParam; //֪ͨ�̴߳���ʲô����

public: //����
	void StartThread();
	void StopThread();

	virtual void SetThreadData(DWORD dwParam);
	virtual DWORD GetThreadData();

	virtual void Run();

	static DWORD ThreadProc(LPVOID pParam);//û�����͵����ͣ���ʱת��
private:
	DWORD m_idThread;
};
