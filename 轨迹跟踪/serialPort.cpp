#include "serialPort.h"

SERIALPORT::SERIALPORT()
{
	m_hComm = NULL;
}

SERIALPORT::~SERIALPORT()
{

}

bool SERIALPORT::InitPort()
{
	if (m_hComm == NULL)
	{
		m_hComm = CreateFileA(m_comBuf,
			GENERIC_READ | GENERIC_WRITE,//可读可写
			0,
			NULL,
			OPEN_EXISTING,//打开已有的，非创建
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
			NULL);
		if (m_hComm == INVALID_HANDLE_VALUE)
		{
			printf("Serial Port Open Failed:%d\n", GetLastError());
			return false;
		}
	}
	DCB dcb;
	bool bRet = GetCommState(m_hComm, &dcb);//获取串口状态，返回dcb句柄
	if (!bRet)
	{
		if (m_hComm)
		{
			CloseHandle(m_hComm);
			m_hComm = NULL;
		}
		return false;
	}
	dcb.BaudRate = baudRate;
	dcb.ByteSize = byteSize; 
	dcb.Parity = parity; 
	dcb.StopBits = stopBits; 
	bRet = SetCommState(m_hComm, &dcb);
	if (!bRet)
	{
		printf("Status Set Failed:%d\n", GetLastError());
		if (m_hComm)
		{
			CloseHandle(m_hComm);
			m_hComm = NULL;
		}
		return false;
	}
	//设置缓冲区大小
	SetupComm(m_hComm, 2048, 2048);
	//清除标志位
	PurgeComm(m_hComm, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT); //清空缓冲区
	return true;
}

void SERIALPORT::SendAskSpeedNow()
{
	//ED 02 00 00 00 00 00 00 00 00 00 00
	BYTE buf[50];
	memset(buf, 0, sizeof(buf));
	buf[0] = 0xED;
	buf[1] = 0x02;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 0;
	buf[6] = 0;
	buf[7] = 0;
	buf[8] = 0;
	buf[9] = 0;
	buf[10] = 0;
	buf[11] = 0;
	SendPortMesg(buf, 12);
}

void SERIALPORT::SendStopCommand()
{
	//E0 03 00 00 00 00 00 00 00 00 00 00
	BYTE buf[50];
	memset(buf, 0, sizeof(buf));
	buf[0] = 0xE0;
	buf[1] = 0x03;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 0;
	buf[6] = 0;
	buf[7] = 0;
	buf[8] = 0;
	buf[9] = 0;
	buf[10] = 0;
	buf[11] = 0;
	SendPortMesg(buf, 12);
}

void SERIALPORT::SendFrontCommand()
{
	//A电机
	int a = m_aSpeedCommand / 0.3;
	//B电机
	int b = m_bSpeedCommand / 0.3;
	BYTE buf[50];
	memset(buf, 0, sizeof(buf));
	buf[0] = 0xE0;
	buf[1] = 0x03;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 0;
	buf[6] = (a >> 8) & 0xFF;
	buf[7] = a & 0xFF;
	buf[8] = 0;
	buf[9] = 0;
	buf[10] = (b >> 8) & 0xFF;
	buf[11] = b & 0xFF;
	if (m_aSpeedNow < 0 && m_bSpeedNow < 0)
	{
		SendStopCommand();
		SendPortMesg(buf, 12);
	}
	else
	{
		SendPortMesg(buf, 12);
	}
}

void SERIALPORT::SendRetreatCommand()
{
	//A电机
	int a = m_aSpeedCommand / 0.3;
	//B电机
	int b = m_bSpeedCommand / 0.3;
	BYTE buf[50];
	memset(buf, 0, sizeof(buf));
	buf[0] = 0xE0;
	buf[1] = 0x03;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0xFF;
	buf[5] = 0xFF;
	buf[6] = (a >> 8) & 0xFF;
	buf[7] = a & 0xFF;
	buf[8] = 0xFF;
	buf[9] = 0xFF;
	buf[10] = (b >> 8) & 0xFF;
	buf[11] = b & 0xFF;
	
	if (m_aSpeedNow > 0 && m_bSpeedNow > 0)
	{
		SendStopCommand();
		SendPortMesg(buf, 12);
	}
	else
	{
		SendPortMesg(buf, 12);
	}
}

void SERIALPORT::SendLeftTurnCommand()   //前进时，实现左转，A可正转可反转，B为正转
{
	//A电机为左电机
	int a = m_aSpeedCommand / 0.3;
	//B电机为右电机
	int b = m_bSpeedCommand / 0.3;
	BYTE buf[50],bufA0[50];
	memset(buf, 0, sizeof(buf));
	memset(bufA0, 0, sizeof(bufA0));
	bufA0[0] = 0xE0;
	bufA0[1] = 0x01;
	bufA0[2] = 0;
	bufA0[3] = 0;
	bufA0[4] = 0;
	bufA0[5] = 0;
	bufA0[6] = 0;
	bufA0[7] = 0;
	bufA0[8] = 0;
	bufA0[9] = 0;
	bufA0[10] = 0;
	bufA0[11] = 0;
	if (a >= 0)  //A的目标指令为正转
	{
		buf[0] = 0xE0;
		buf[1] = 0x03;
		buf[2] = 0;
		buf[3] = 0;
		buf[4] = 0;
		buf[5] = 0;
		buf[6] = (a >> 8) & 0xFF;
		buf[7] = a & 0xFF;
		buf[8] = 0;
		buf[9] = 0;
		buf[10] = (b >> 8) & 0xFF;
		buf[11] = b & 0xFF;
		if (m_aSpeedNow >= 0)  //若A的转速为正转,直接发送目标指令
		{
			SendPortMesg(buf, 12);
		}
		else                    //若A的转速为反转,先发送0转速指令，再直接发送目标指令
		{
			SendPortMesg(bufA0, 12);
			SendPortMesg(buf, 12);
		}
		
	}
	else
	{
		////若A的目标指令为反转
		buf[0] = 0xE0;
		buf[1] = 0x03;
		buf[2] = 0;
		buf[3] = 0;
		buf[4] = 0xFF;
		buf[5] = 0xFF;
		buf[6] = (a >> 8) & 0xFF;
		buf[7] = a & 0xFF;
		buf[8] = 0;
		buf[9] = 0;
		buf[10] = (b >> 8) & 0xFF;
		buf[11] = b & 0xFF;
		
		if (m_aSpeedNow <= 0)  //若A的转速为反转,直接发送目标指令
		{
			SendPortMesg(buf, 12);
		}
		else                    //若A的转速为正转,先发送0转速指令，再直接发送目标指令
		{
			SendPortMesg(bufA0, 12);
			SendPortMesg(buf, 12);
		}	
	}
}

void SERIALPORT::SendRightTurnCommand()//前进时，实现右转，A为正转，B可正转可反转
{
	//A电机为左电机
	int a = m_aSpeedCommand / 0.3;
	//B电机为右电机
	int b = m_bSpeedCommand / 0.3;
	BYTE buf[50], bufB0[50];
	memset(buf, 0, sizeof(buf));
	memset(bufB0, 0, sizeof(bufB0));
	bufB0[0] = 0xE0;
	bufB0[1] = 0x02;
	bufB0[2] = 0;
	bufB0[3] = 0;
	bufB0[4] = 0;
	bufB0[5] = 0;
	bufB0[6] = 0;
	bufB0[7] = 0;
	bufB0[8] = 0;
	bufB0[9] = 0;
	bufB0[10] = 0;
	bufB0[11] = 0;
	if (b >= 0)  //B的目标指令为正转
	{
		buf[0] = 0xE0;
		buf[1] = 0x03;
		buf[2] = 0;
		buf[3] = 0;
		buf[4] = 0;
		buf[5] = 0;
		buf[6] = (a >> 8) & 0xFF;
		buf[7] = a & 0xFF;
		buf[8] = 0;
		buf[9] = 0;
		buf[10] = (b >> 8) & 0xFF;
		buf[11] = b & 0xFF;
		if (m_bSpeedNow >= 0)  //若B的转速为正转,直接发送目标指令
		{
			SendPortMesg(buf, 12);
		}
		else                    //若B的转速为反转,先发送0转速指令，再直接发送目标指令
		{
			SendPortMesg(bufB0, 12);
			SendPortMesg(buf, 12);
		}

	}
	else
	{
		////若B的目标指令为反转
		buf[0] = 0xE0;
		buf[1] = 0x03;
		buf[2] = 0;
		buf[3] = 0;
		buf[4] = 0;
		buf[5] = 0;
		buf[6] = (a >> 8) & 0xFF;
		buf[7] = a & 0xFF;
		buf[8] = 0xFF;
		buf[9] = 0xFF;
		buf[10] = (b >> 8) & 0xFF;
		buf[11] = b & 0xFF;

		if (m_bSpeedNow <= 0)  //若B的转速为反转,直接发送目标指令
		{
			SendPortMesg(buf, 12);
		}
		else                    //若B的转速为正转,先发送0转速指令，再直接发送目标指令
		{
			SendPortMesg(bufB0, 12);
			SendPortMesg(buf, 12);
		}
	}
}

void SERIALPORT::Run()
{
	Sleep(10);
	DWORD dwError = 0;
	COMSTAT comStat;
	bool bRet = true;
	BYTE recvTemp[512]; //临时缓冲区
	memset(recvTemp, 0, sizeof(recvTemp));
	
	DWORD dwRead = 0;
	memset(&comStat, 0, sizeof(comStat));
	ClearCommError(m_hComm, &dwError, &comStat);
	if (comStat.cbInQue > 0)
	{
		OVERLAPPED olRead;
		memset(&olRead, 0, sizeof(olRead));
		olRead.hEvent= CreateEvent(NULL, true, false, NULL);//自动触发，原先状态为false
		if (comStat.cbInQue < 512)
		{
			bRet = ReadFile(m_hComm, recvTemp, comStat.cbInQue, &dwRead, &olRead);
			printf("收到%d个数据：", dwRead);
			for (int i = 0;i < dwRead;i++)
			{
				printf("%02x ", recvTemp[i]);
			}
			printf("\n");

			//处理接收数据
			for (int i = 0;i < dwRead - 1;i++)
			{
				if (recvTemp[i] == 0xED && recvTemp[i + 1] == 0x02)
				{
					short a = (recvTemp[i + 2] << 8) | recvTemp[i + 3];
					short b = (recvTemp[i + 4] << 8) | recvTemp[i + 5];
					if (a > 32768)//负数
					{
						a = a - 65535;
					}
					if (b > 32768)//负数
					{
						b = b - 65535;
					}
					m_aSpeedNow = a * 0.3;
					m_bSpeedNow = b * 0.3;
					i += 13;
					//printf("当前A电机转速：%.1f, B转速：%.1f\n", m_aSpeedNow, m_bSpeedNow);
				}
			}
		}
		else
		{
			//放置缓冲区溢出，清空
			PurgeComm(m_hComm, PURGE_RXABORT | PURGE_RXCLEAR); //清空接收缓冲区
			return;
		}
		if (olRead.hEvent != NULL)
		{
			CloseHandle(olRead.hEvent);
			olRead.hEvent = NULL;
		}

		//判断是否重叠
		if (!bRet)//操作正在后台执行
		{
			if (ERROR_IO_PENDING == GetLastError())
			{
				while (!bRet)//等待执行完成
				{
					bRet = GetOverlappedResult(m_hComm, NULL, &dwRead, true);
					if (GetLastError() != ERROR_IO_INCOMPLETE)//清楚一次错误
					{
						ClearCommError(m_hComm, &dwError, &comStat);
						break;
					}
				}
			}
		}
	}
}

void SERIALPORT::SendPortMesg(BYTE* sendMesgBuf, int len)
{
	DWORD dwNum = 0;
	OVERLAPPED m_ovWrite;//用于写入数据
	memset(&m_ovWrite, 0, sizeof(m_ovWrite));
	m_ovWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);//自动触发，原先状态为false
	bool rtn = WriteFile(m_hComm, sendMesgBuf, len, &dwNum, &m_ovWrite);
	WaitForSingleObject(m_ovWrite.hEvent, 20);
	GetOverlappedResult(m_hComm, &m_ovWrite, &dwNum, false);
	CloseHandle(m_ovWrite.hEvent);
	m_ovWrite.hEvent = NULL;
	if (!FlushFileBuffers(m_hComm))
	{
		printf("Flush:error=%d\n", GetLastError());
	}
}
