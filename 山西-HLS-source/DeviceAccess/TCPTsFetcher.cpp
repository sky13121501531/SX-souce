
#include "TCPTsFetcher.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "ace/OS.h"
#include "ace/Synch.h"
#include "./DeviceAccessMgr.h"
#include "../Communications/TsSender.h"
#include "DeviceAccess.h"
#include "../Foundation/XmlParser.h"
#include "../Communications/TSServer.h"

ACE_Thread_Mutex TCPTsFetcher::TaskRoundMutex;

TCPTsFetcher::TCPTsFetcher(int deviceid) : TsFetcher(deviceid)
{
	TsSendTaskPointer = NULL;//��Ƶ����
	TsSendRoundTaskPointer = NULL;//�ֲ�����
	RecordTaskPointerVec.clear();
	bFlag = false;
	bSendSwtich=false;
	DeviceTaskXml = OSFunction::CreateTaskTypeXml(DeviceId);
	CurDeviceXml = "";
	string TsIp;
	PROPMANAGER::instance()->GetDeviceTSIP(deviceid,TsIp);
	unsigned int tsport=0;
	PROPMANAGER::instance()->GetDeviceTsPort(deviceid,tsport);

	string mulitcase = TsIp + string(":")+ StrUtil::Int2Str(tsport);
	URL = "http://"+mulitcase;

	const int PKGSIZE = 188;
	int value =PKGSIZE*4096;	
	TaskNum=0;
	DeviceSock.set_option(SOL_SOCKET,SO_SNDBUF,(char*)&value,sizeof(value));
	DeviceSock.set_option(SOL_SOCKET,SO_RCVBUF,(char*)&value,sizeof(value));
}

TCPTsFetcher::~TCPTsFetcher()
{

}
int TCPTsFetcher::Start()
{
	open(0);
	return 0;
}

int TCPTsFetcher::open(void*)
{
	bFlag = true;
	NewPackageHead = false;

	//���ý��ն��л�������С
	msg_queue()->high_water_mark(188*8192);
	msg_queue()->low_water_mark(188*128);
	this->activate();

	return 0;
}

int TCPTsFetcher::svc()
{
	int ReadSize	= 188*7;	//ÿ�λ�����ݵĴ�С
	int BufLen		= 188*70;	//ÿ�η��ͻ���д�ļ��Ŀ��С 

	unsigned char* RcvBuf	= new unsigned char[ReadSize];		//����BUF
	unsigned char* TsBuf	= new unsigned char[BufLen];		//����BUF

	memset(RcvBuf,0,ReadSize);
	memset(TsBuf,0,BufLen);
	
	int PacketLen = 0;
	ACE_Time_Value PutOutTime(ACE_OS::gettimeofday()+ACE_Time_Value(5));
	ACE_Time_Value RecvTimeOut(10);

	while(bFlag)
	{
		if (CurDeviceXml.length()<1||TaskNum<=0)
		{
			OSFunction::Sleep(0,100);
			continue;
		}

		while(URL.length()<1)
		{
			SendTsDeviceXml();
			OSFunction::Sleep(2);
		}

		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]��ʼ��ȡTS��\n\t\t��ַ<%s>!\n",DeviceId,URL.c_str()));
		
		URLChanged = false;
		int FailCount = 0;

		//������Ͷ��к�¼��Ļ������
		/*
		TaskRoundMutex.acquire();
		if (TsSendTaskPointer != NULL)
		{
			TsSendTaskPointer->msg_queue()->flush();
		}
		TaskRoundMutex.release();
		*/
		TaskPointerMutex.acquire();
		std::vector<ACE_Task<ACE_MT_SYNCH>*>::iterator ptr = RecordTaskPointerVec.begin();
		for(;ptr!=RecordTaskPointerVec.end();++ptr)
		{
			if ( *ptr != NULL)
			{
				(*ptr)->msg_queue()->flush();
			}
		}
		TaskPointerMutex.release();

		DeviceSock.close();
		DEVICEACCESSMGR::instance()->GetTsReceiveHandle(URL,DeviceSock);//����urlָ���ķ���������������socket

		while(URLChanged == false && bFlag)
		{
			if (URL.length()<1 || DeviceSock.get_handle() == NULL||TaskNum<=0)
				break;

			memset(RcvBuf,0,ReadSize);
			int RecvLen = DeviceSock.recv(RcvBuf,ReadSize,&RecvTimeOut);//��������

			if (RecvLen <= 0)
			{
				//���»�ȡSocket
				TaskMutex.acquire();
				if (URL != "")
				{
					ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����������!\n",DeviceId));
					DeviceSock.close();
					DEVICEACCESSMGR::instance()->GetTsReceiveHandle(URL,DeviceSock);
				}
				TaskMutex.release();
				//��ȡ����ʧ�ܳ���5�Σ����忨���ͳ�ʼ��ָ��ֱ�����ͳɹ�
				string retXML;
				while(++FailCount >= 5 && retXML.length()<1)
				{
					string msg = string("ͨ��[") + StrUtil::Int2Str(DeviceId) + string("]���ͳ�ʼ��ָ��");
					//SYSMSGSENDER::instance()->SendMsg(msg);

					DEVICEACCESSMGR::instance()->SendTaskMsg(DeviceId,DeviceTaskXml,retXML);
					OSFunction::Sleep(0,500);
				}				
				continue;
			}
			if (BufLen-PacketLen > RecvLen)
			{
				memcpy(TsBuf+PacketLen,RcvBuf,RecvLen);	
				PacketLen += RecvLen;
				continue;
			}
			
			FailCount = 0;

			//��һ�ν��յ����ݰ��������ͷ����֤�ܹ����ź�¼���ļ��ܹ�����
			unsigned char* pPackage = NULL;
			//if (NewPackageHead == true)
			//{
			//	int beginPos = FindBegin(TsBuf,PacketLen);
			//	PacketLen -= beginPos;
			//	pPackage = TsBuf + beginPos;
			//	NewPackageHead = false;
			//}
			//else
			{
				pPackage = TsBuf;
			}

		/*	
			TSSERVERMGR::instance()->AddTsData(StrUtil::Int2Str(DeviceId),(char*)TsBuf,PacketLen);
			TaskRoundMutex.acquire();
			if(bSendSwtich)
			{
				TSSERVERMGR::instance()->AddTsData("roundstream",(char*)TsBuf,PacketLen);
			}
			TaskRoundMutex.release();
		*/
			//��Ƶ�ֲ�����ָ�벻Ϊ�գ��򽫻�����ݷ�����Ƶ�����̵߳����ݶ�����
			TaskRoundMutex.acquire();
			if (TsSendRoundTaskPointer != NULL && bSendSwtich == true)
			{
				ACE_Message_Block *mb = NULL;
				try
				{
					mb = new ACE_Message_Block(PacketLen);
					if (mb != NULL)
					{
						memcpy(mb->wr_ptr(),pPackage,PacketLen);
						mb->wr_ptr(PacketLen);

						if(-1 == TsSendRoundTaskPointer->putq(mb,&PutOutTime))
						{
							mb->release();
							TsSendRoundTaskPointer->msg_queue()->flush();
						}
					}					
				}
				catch (...)
				{
					if (mb != NULL)
						mb->release();
					ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d] TCPTsFetcherʹ��ACE_Message_Block����\n",DeviceId));
				}
			}
			TaskRoundMutex.release();

			//��Ƶ����ָ�벻Ϊ�գ��򽫻�����ݷ�����Ƶ�����̵߳����ݶ�����
			TaskPointerMutex.acquire();
			if (TsSendTaskPointer != NULL)
			{
				ACE_Message_Block *mb = NULL;
				try
				{
					mb = new ACE_Message_Block(PacketLen);
					if (mb != NULL)
					{
						memcpy(mb->wr_ptr(),pPackage,PacketLen);
						mb->wr_ptr(PacketLen);
						if(-1 == TsSendTaskPointer->putq(mb,&PutOutTime))
						{
							mb->release();
							TsSendTaskPointer->msg_queue()->flush();
						}
					}					
				}
				catch(...)
				{
					if (mb != NULL)
						mb->release();
					ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d] TCPTsFetcherʹ��ACE_Message_Block����\n",DeviceId));
				}
				
			}
			//��Ƶ¼������ָ�벻Ϊ�գ��򽫻�����ݷ�����Ƶ�����̵߳����ݶ�����
			std::vector<ACE_Task<ACE_MT_SYNCH>*>::iterator ptr = RecordTaskPointerVec.begin();
			for(;ptr!=RecordTaskPointerVec.end();++ptr)
			{
				if ((*ptr) != NULL)//¼������ָ�벻Ϊ�գ��򽫻�����ݷ���¼����������ݶ�����
				{
					ACE_Message_Block *mb = NULL;
					try
					{
						mb = new ACE_Message_Block(PacketLen);
						if (mb != NULL)
						{
							memcpy(mb->wr_ptr(),pPackage,PacketLen);
							mb->wr_ptr(PacketLen);
							if(-1 == (*ptr)->putq(mb,&PutOutTime))
							{
								mb->release();
								(*ptr)->msg_queue()->flush();
							}
						}
					}
					catch(...)
					{
						if (mb != NULL)
							mb->release();
						ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d] TCPTsFetcherʹ��ACE_Message_Block����\n",DeviceId));
					}
				}
			}
			TaskPointerMutex.release();

			memset(TsBuf,0,BufLen);
			PacketLen = 0;

			memcpy(TsBuf+PacketLen,RcvBuf,RecvLen);	//�������ڴ�
			PacketLen += RecvLen;
		}
		DeviceSock.close();

		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ֹͣ��ȡTS��\n",DeviceId));
	}

	delete RcvBuf;
	delete TsBuf;
	bFlag = false;
//	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ��Ƶ���ݻ�ȡ�߳�ִֹͣ��\n"));

	return 0;
}

bool TCPTsFetcher::SetTsSendTask(TsSender* task)
{
	if (task == NULL)
		return false;

	TaskPointerMutex.acquire();
	TsSendTaskPointer = task;//������Ƶ�����ָ��
	TaskPointerMutex.release();
	return true;
}

bool TCPTsFetcher::SetTsRoundTask(TsSender* task)
{
	if (task == NULL)
		return false;

	TaskRoundMutex.acquire();
	TsSendRoundTaskPointer = task;//������Ƶ¼�������ָ��
	TaskRoundMutex.release();
	return true;
}

bool TCPTsFetcher::SetRecordTask( ACE_Task<ACE_MT_SYNCH>* task )
{
	if(task==NULL)
		return false;
	TaskPointerMutex.acquire();
	RecordTaskPointerVec.push_back(task);
	TaskPointerMutex.release();
	return true;
}

int TCPTsFetcher::Stop()
{
	bFlag = false;
	this->wait();

	return 0;
}

bool TCPTsFetcher::SetTsDeviceXml( std::string devicexml )
{
	bool ret = false;
	
	if(devicexml.length()<1)
		return false;

	if(devicexml==CurDeviceXml)
		return true;
	else
	{	
		TaskMutex.acquire();
		CurDeviceXml=devicexml;
		//URL = "";			//���»�ȡ��ַ 
		URLChanged=true;
		DeviceSock.close();
		TaskMutex.release();
	}
	return ret;
}

bool TCPTsFetcher::SendTsDeviceXml()
{
	TaskMutex.acquire();
	bool NoTaskXml = CurDeviceXml.empty();
	TaskMutex.release();

	if (NoTaskXml == true)
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]������ָ��\n",DeviceId));
		return true;
	}

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]��������ָ��\n",DeviceId));

//	NewPackageHead = true;	//�����µ�����ָ���ʱ����Ҫ����ͷ����

	std::string retXML;

	TaskMutex.acquire();
	DEVICEACCESSMGR::instance()->SendTaskMsg(DeviceId,CurDeviceXml,retXML);
	TaskMutex.release();

	if (retXML.length()<1)
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]��ȡ������Ϣʧ��\n",DeviceId));
		return false;
	}
	
	string retvalue;
	XmlParser parser(retXML.c_str());
	pXMLNODE returtnNode = parser.GetNodeFromPath("Msg/Return");
	parser.GetAttrNode(returtnNode,"Value",retvalue);

	if (retvalue != "0")
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]��ȡ��ַʧ��\n",DeviceId));
		return false;
	}

	pXMLNODE queryNode = parser.GetNodeFromPath("Msg/Stream/MediaStream");
	parser.GetAttrNode(queryNode,"URL",URL);
	if (URL.length()<1)
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]��ȡ��ַʧ��\n",DeviceId));
		return false;
	}
	return true;
}

bool TCPTsFetcher::DelRecordTask( ACE_Task<ACE_MT_SYNCH>* task )
{
	TaskPointerMutex.acquire();
	std::vector<ACE_Task<ACE_MT_SYNCH>*>::iterator ptr=RecordTaskPointerVec.begin();
	for(;ptr!=RecordTaskPointerVec.end();)
	{		
		if((*ptr)==task)
		{
			delete *ptr;
			*ptr = NULL;
			ptr=RecordTaskPointerVec.erase(ptr++);
			break;
		}
		else
			ptr++;		
	}
	TaskPointerMutex.release();
	return true;
}

void TCPTsFetcher::SetSendSwitch( bool sendswitch )
{
	TaskRoundMutex.acquire();
	bSendSwtich=sendswitch;
	TaskRoundMutex.release();

	if (sendswitch == true)
	{
		NewPackageHead = true;	//�л��ֲ�����ָ���ʱ����ѽ����ͷ����
	}
}

void TCPTsFetcher::IncreaseTaskNum()
{
	TaskNumMutex.acquire();
	TaskNum++;
	TaskNumMutex.release();
}

void TCPTsFetcher::DecreaseTaskNum()
{
	TaskNumMutex.acquire();
	TaskNum--;
	TaskNumMutex.release();
}

void TCPTsFetcher::StopHistoryTask()
{
	return;
}
void TCPTsFetcher::SetReSendSwitch(bool sendswitch)
{
	return;
}