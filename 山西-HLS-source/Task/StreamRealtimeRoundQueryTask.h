///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����StreamRealtimeRoundQueryTask.cpp
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-24
// ����������ʵʱ��Ƶ�ֲ�������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"
#include <string>
#include <vector>
#include "ace/Task.h"
using namespace std;

class Device2ChanelID{
public:
	std::string position;
	std::string	program;
	std::string programid;
	std::string freq;
	std::string	serviceid;
	std::string	videoid;
	std::string	audioid;
	int deviceid;
	std::string chanelid;
	std::string bps;
	bool bhd;
};
class StreamRealtimeRoundQueryTask : public DeviceRelatedTask
{
public:
	StreamRealtimeRoundQueryTask();
	StreamRealtimeRoundQueryTask(std::string strXML);
	virtual ~StreamRealtimeRoundQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	virtual bool IsRealTimeTask(){return true;};	//ʵʱ����
	virtual bool IsVedioTask(){return true;};		//����Ƶ����

	bool SetRelatedTask(StreamRealtimeRoundQueryTask* task){RelatedTask = task;return true;};
	bool SetStopSignal(){StopSignal = true;return true;};		//�����������
	std::string ReportCreateXML();    //���������ϱ�xml
	bool ChangeDevCommand(string incmd,string turnid,string coderid,string& outcmd);

private:
	bool bOneDevice;
	bool bSendToIdleDev;					//bOneDevice:һ��ͨ���ж�,bSendToIdleDev:�Ƿ������ͨ�����͹���Ƶ����
	int idleDeviceID,busyDeviceID;			//idleDeviceID:����ͨ����busyDeviceID:ִ������ͨ����
	long RoundTime;							//�ֲ����
	int WindowNumber;                      //���洰����
	int RunTime;							//��Ƶ����ʱ��
	std::string Program;                    //��Ŀ����
	std::vector<std::string> DecivcXMLVec;	//Ӳ��������
	std::vector<Device2ChanelID> MultiIndex;	//Ӳ��������
	std::vector<Device2ChanelID> MultiCurIndex;	//Ӳ��������

	std::vector<int> DeviceMul1st;			 //1�����տ�ͨ����//FRee_fanrong
	std::vector<int> deviceindexmul;

	size_t ChannelNum;						//�ֲ���Ƶ����
	size_t ChannelIndex;					//������
	int MultiNum; //�໭���ֲ��Ĵ���

	bool StopSignal;							//ֹͣ�ź�
	StreamRealtimeRoundQueryTask* RelatedTask;	//��������,���ڴ�����������ֹͣ
	std::string m_startxml;
	std::string m_stopxml;
	std::string m_sOsd;
	//
	std::vector<int> m_tundevlist;
	std::vector<int> m_coddevlist;
	int m_roundnum;
};