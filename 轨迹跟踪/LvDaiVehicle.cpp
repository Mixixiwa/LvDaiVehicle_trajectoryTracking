// LvDaiVehicle.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "serialPort.h"
#include <conio.h>

SERIALPORT serialPort;
SERIALPORT* pserial = &serialPort;

//按下q程序退出
bool quitFlag = false;
//W S A D按键
bool fourKey[4] = {'W','S','A','D'};

DWORD WINAPI PortSend(LPVOID lpParameter)
{
	pserial->StartThread();
	while (!quitFlag)
	{
		//发送数据 
		//w按键按下：前进
		if (fourKey[0])
		{
			pserial->m_aSpeedCommand = 300;
			pserial->m_bSpeedCommand = 300;
			pserial->SendFrontCommand();
			Sleep(50);
		}
		//s键按下
		if (fourKey[1])
		{
			pserial->m_aSpeedCommand = -300;
			pserial->m_bSpeedCommand = -300;
			pserial->SendRetreatCommand();
			Sleep(50);
		}
		//a键按下
		if (fourKey[2])
		{
			pserial->m_aSpeedCommand = -300;
			pserial->m_bSpeedCommand = 300;
			pserial->SendLeftTurnCommand();
			Sleep(50);
		}
		//d键按下
		if (fourKey[3])
		{
			pserial->m_aSpeedCommand = 300;
			pserial->m_bSpeedCommand = -300;
			pserial->SendRightTurnCommand();
			Sleep(50);
		}
		//循环发送当前转速问询指令
		pserial->SendAskSpeedNow();
		Sleep(50);
	}
	//程序退出前，发送停车指令
	pserial->SendStopCommand();

	pserial->StopThread();
	Sleep(10);
	cout << "发送线程已关闭" << endl;
	return 0;
}

int main()
{
	//串口号
	pserial->m_comNum = 2;
	memset(pserial->m_comBuf, 0, sizeof(pserial->m_comBuf));
	if (pserial->m_comNum < 10)
	{
		sprintf_s(pserial->m_comBuf, "COM%d", pserial->m_comNum);
	}
	else
	{
		sprintf_s(pserial->m_comBuf, "\\\\.\\COM%d", pserial->m_comNum);
	}

	if (!pserial->InitPort())
	{
		printf("Link Failed, Please Check the COM Number!=%s=\n", pserial->m_comBuf);
		return false;
	}
	else
	{
		printf("%s Opened!\n", pserial->m_comBuf);
	}
	HANDLE hSend = CreateThread(NULL, 0, &PortSend, NULL, 0, NULL);//创建发送线程
	int lifeSignal = 0; //心跳计时
	while (!quitFlag)
	{
		if (_kbhit())
		{
			switch (_getch())
			{
			case 'q':
				quitFlag = true;
				break;
			case 'w':
				fourKey[0] = true;
				break;
			case 's':
				fourKey[1] = true;
				break;
			case 'a':
				fourKey[2] = true;
				break;
			case 'd':
				fourKey[3] = true;
				break;
			default:
				break;
			}
		}
		else
		{
			memset(fourKey, 0, sizeof(fourKey));
		}

		Sleep(50);
	}
	if (hSend != NULL)
	{
		WaitForSingleObject(hSend, INFINITE);
		CloseHandle(hSend);
		hSend = NULL;
	}
	cout << "程序退出！" << endl;
	Sleep(3000);
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
