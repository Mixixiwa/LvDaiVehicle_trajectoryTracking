#pragma once
#include "threadCom.h"
class SERIALPORT : public THREADCOM
{
public:
	SERIALPORT();
	~SERIALPORT();

public:
	//ָ��
	float m_aSpeedCommand = 0;
	float m_bSpeedCommand = 0;

	//����
	float m_aSpeedNow = 0;
	float m_bSpeedNow = 0;


//���ڶ�ȡ���
public: //����
	HANDLE m_hComm;
	//�˿ں�
	int m_comNum = 0;
	char m_comBuf[50];

public:
	bool InitPort();//��ʼ�����ڣ�����״̬��������������

	//��������
	//ѯ�ʵ�ǰת��
	void SendAskSpeedNow();
	//ͣ��ָ��
	void SendStopCommand();
	//ǰ��ָ��
	void SendFrontCommand();
	//����ָ��
	void SendRetreatCommand();
	//��תָ��
	void SendLeftTurnCommand();
	//��תָ��
	void SendRightTurnCommand();

private:
	//�����������
	void ReadSpeed();


private: //����
	DWORD baudRate = 115200;//
	BYTE byteSize = 8; //ÿ���ֽ���8λ
	BYTE parity = NOPARITY; //����żУ��λ
	BYTE stopBits = ONESTOPBIT; //һ��ֹͣλ

private:
	virtual void Run();
	void SendPortMesg(BYTE *sendMesgBuf, int len); //���ʹ�������

};

