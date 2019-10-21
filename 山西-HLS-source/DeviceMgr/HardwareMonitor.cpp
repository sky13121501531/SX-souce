//////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����HardwareMonitor.h
// �����ߣ�wuyy	
// ����ʱ�䣺2014-6-25
// ����������Ӳ�����ģ��
///////////////////////////////////////////////////////////////////////////////////////////
#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include "HardwareMonitor.h"
#include "../Foundation/PropManager.h"
#include <IPExport.h>

#include <iphlpapi.h>
#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"wsock32.lib")

#include <iostream>
#include <string>
#include <list>
//1.��ȡ�豸ͨ����Ϣ��2.����ͨ����;��3.������ͨ����ת��Ĺ�ϵ
HardwareMonitor::HardwareMonitor(void)
{
	harddisk = true;
	casepower =true;
	board = true;
	fan =true;
}

HardwareMonitor::~HardwareMonitor(void)
{
}
void HardwareMonitor::BoardMonitor()
{
	PropManager p_manager;
	p_manager.IpVec;
	DWORD dwRetVal;
	IPAddr DestIp ;
	IPAddr SrcIp = 0; 
	ULONG MacAddr[2];       
	ULONG PhysAddrLen=6;  

	for (map<string,int>::iterator iter=p_manager.IpVec.begin();iter!=p_manager.IpVec.end();iter++)
	{
		DestIp=inet_addr(iter->first.c_str());
		dwRetVal = SendARP(DestIp, SrcIp, &MacAddr, &PhysAddrLen); //ping�忨ͨ�Ļ�����0
		PhysAddrLen=6; 
		if (dwRetVal!=0)
		{
			if (iter->second>0&&iter->second<17)
			{
				O_brokeBoardVec.push_back(iter->first.c_str());
			}
			else
			{
				T_brokeBoardVec.push_back(iter->first.c_str());
			}
		}	
	}
	if (O_brokeBoardVec.size()>0||T_brokeBoardVec.size()>0)
	{
		cout<<"�忨��ͨ"<<endl;
	}
}
void HardwareMonitor::CasePower()
{
	string cmd = "taskkill /IM tftpd32.exe";
	string str;
	if (O_brokeBoardVec.size() == 16||T_brokeBoardVec.size()==16)
	{
		system(cmd.c_str());
		Sleep(20000);

	}

	if (O_brokeBoardVec.size()==16&&T_brokeBoardVec.size()==16)
	{	
		str = "01�Ż�����Ϻ�02�Ż������";
		BrokeCasePower.push_back(str);
	}
	else if(O_brokeBoardVec.size()==16)
	{
		str = "01�Ż������";
		BrokeCasePower.push_back(str);
	}
	else if(T_brokeBoardVec.size()==16)
	{
		str = "02�Ż������";
		BrokeCasePower.push_back(str);
	}
}
void HardwareMonitor::FanMonitor()
{
	//�����޷��ж�Ĭ��Ϊ��
}
void HardwareMonitor::HarddiskMonitor()
{
	//ͨ���Ƿ��ܴ����ļ������жϸ�����Ӳ���Ƿ�����
	std::string bharddisk;
	std::string Testpath = "m:/dtest.txt";
	if (!CreateDirectory(Testpath.c_str(),NULL))
	{
		//Ӳ�̹���
		bharddisk = "d�̷�������";
		BrokeHarddiskVec.push_back(bharddisk);
		harddisk = false;
	}
	else
	{
		//Ӳ������   ɾ���ղŴ����Ĳ����ĵ�
		RemoveDirectory(Testpath.c_str());
	}

}