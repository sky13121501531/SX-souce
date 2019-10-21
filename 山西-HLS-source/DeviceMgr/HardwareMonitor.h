///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����HardwareMonitor.h
// �����ߣ�wuyy	
// ����ʱ�䣺2014-6-25
// ����������Ӳ�����ģ��
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
class HardwareMonitor
{
public:
	HardwareMonitor(void);
	~HardwareMonitor(void);
public:
	void CasePower();   //�жϻ����Դ����
	void HarddiskMonitor(); //�ж�Ӳ�̹���	
	void FanMonitor();  //�жϷ��ȹ���
	void BoardMonitor(); //�жϰ忨����

	bool harddisk;
	bool casepower;
	bool fan    ;
	bool board;
	std::vector<std::string> BrokeHarddiskVec;
	std::vector<std::string> O_brokeBoardVec,T_brokeBoardVec; //�ֱ��ŵ�һ�������ϵ����л��忨�͵ڶ��������ϵ����л���
	std::vector<std::string> BrokeCasePower;
};
