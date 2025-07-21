#pragma once
#include "threadCom.h"
class SERIALPORT : public THREADCOM
{
public:
	SERIALPORT();
	~SERIALPORT();

public:
	//指令
	float m_aSpeedCommand = 0;
	float m_bSpeedCommand = 0;

	//反馈
	float m_aSpeedNow = 0;
	float m_bSpeedNow = 0;


//串口读取相关
public: //参数
	HANDLE m_hComm;
	//端口号
	int m_comNum = 0;
	char m_comBuf[50];

public:
	bool InitPort();//初始化串口，设置状态参数、缓冲区等

	//发送数据
	//询问当前转速
	void SendAskSpeedNow();
	//停车指令
	void SendStopCommand();
	//前进指令
	void SendFrontCommand();
	//后退指令
	void SendRetreatCommand();
	//左转指令
	void SendLeftTurnCommand();
	//右转指令
	void SendRightTurnCommand();

private:
	//处理接收数据
	void ReadSpeed();


private: //参数
	DWORD baudRate = 115200;//
	BYTE byteSize = 8; //每个字节有8位
	BYTE parity = NOPARITY; //无奇偶校验位
	BYTE stopBits = ONESTOPBIT; //一个停止位

private:
	virtual void Run();
	void SendPortMesg(BYTE *sendMesgBuf, int len); //发送串口数据

};

