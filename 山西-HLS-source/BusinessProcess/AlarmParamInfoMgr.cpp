#include "AlarmParamInfoMgr.h"
#include "./ChannelInfoMgr.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/StrUtil.h"
AlarmParamInfoMgr::AlarmParamInfoMgr(void)
{
	vecAlarmParam.clear();
	InitAlarmParamInfo();
}

AlarmParamInfoMgr::~AlarmParamInfoMgr(void)
{
}

bool AlarmParamInfoMgr::InitAlarmParamInfo( void )
{
	DBMANAGER::instance()->QueryAlarmParam(vecAlarmParam);
	return true;
}

bool AlarmParamInfoMgr::UpdateAlarmParam(std::vector<sAlarmParam>& vecalaramparam)
{
	for (std::vector<sAlarmParam>::iterator ptr=vecalaramparam.begin();ptr!=vecalaramparam.end();++ptr)
	{
		ACE_Guard<ACE_Thread_Mutex> guard(AlarmMapMutex);
		std::vector<sAlarmParam>::iterator ptr_in=vecAlarmParam.begin();
		for (;ptr_in!=vecAlarmParam.end();++ptr_in)
		{
			if (((*ptr).DVBType == (*ptr_in).DVBType) && ((*ptr).TypeID == (*ptr_in).TypeID) &&((*ptr).AlarmType == ALARM_ENVIRONMENT))//��������
			{
				(*ptr_in) = (*ptr);//�������õĲ�����Ϣ����ԭ���Ĳ�����Ϣ
				break;
			}
			else if (((*ptr).DVBType == (*ptr_in).DVBType) && ((*ptr).TypeID == (*ptr_in).TypeID) && ((*ptr).DeviceID == (*ptr_in).DeviceID) &&((*ptr).AlarmType == ALARM_PROGRAM))//��Ŀ����
			{
				if ((*ptr).ChannelID == (*ptr_in).ChannelID)
				{
					(*ptr_in) = (*ptr);//�������õĲ�����Ϣ����ԭ���Ĳ�����Ϣ
					break;
				}
			}
			else if (((*ptr).DVBType == (*ptr_in).DVBType) && ((*ptr).TypeID == (*ptr_in).TypeID) && ((*ptr).DeviceID == (*ptr_in).DeviceID) &&
					((*ptr).AlarmType == ALARM_FREQ))//��Ƶ����
			{
				if (((*ptr).Freq == (*ptr_in).Freq) && ((*ptr).SymbolRate == (*ptr_in).SymbolRate))
				{
					(*ptr_in) = (*ptr);//�������õĲ�����Ϣ����ԭ���Ĳ�����Ϣ
					break;
				}
				
			}
			else if (((*ptr).DVBType == (*ptr_in).DVBType) && ((*ptr).TypeID == (*ptr_in).TypeID) && ((*ptr).DeviceID == (*ptr_in).DeviceID) &&
				((((*ptr).AlarmType == ALARM_TR101_290))))//102_290����
			{
				if ((*ptr).SymbolRate == (*ptr_in).SymbolRate)
				{
					(*ptr_in) = (*ptr);//�������õĲ�����Ϣ����ԭ���Ĳ�����Ϣ
					break;
				}

			}
			else
			{
				continue;
			}
		}
		if (ptr_in==vecAlarmParam.end())
		{
			vecAlarmParam.push_back(*ptr);//�����õĲ�����Ϣ��ӵ��ڴ�
		}
		DBMANAGER::instance()->UpdateAlarmParam(*ptr);//���±���������Ϣ���ݿ�
	}
	return true;
}

bool AlarmParamInfoMgr::GetAlarmParam(const sCheckParam& checkparam,sAlarmParam& alarmparam)
{
	bool ret = false;
	ACE_Guard<ACE_Thread_Mutex> guard(AlarmMapMutex);
	switch(checkparam.AlarmType)
	{
	case ALARM_ENVIRONMENT://��������
		{
			for (std::vector<sAlarmParam>::iterator ptr=vecAlarmParam.begin();ptr!=vecAlarmParam.end();++ptr)
			{
				if (((*ptr).DVBType == checkparam.DVBType)&&((*ptr).AlarmType == checkparam.AlarmType)&&((*ptr).TypeID == checkparam.TypeID))
				{
					alarmparam = *ptr;
					ret = true;
					break;
				}
			}
			break;
		}
	case ALARM_PROGRAM://��Ŀ����
		{
			int bFindFreq = 0;//0û�ҵ����� 1�ҵ�All���� 2�ҵ�ȫƥ��
			sAlarmParam popularAlarmParam;//ͨ���� ��Ŀ��������
			for (std::vector<sAlarmParam>::iterator ptr=vecAlarmParam.begin();ptr!=vecAlarmParam.end();++ptr)
			{
				if (((*ptr).DVBType == checkparam.DVBType)&&((*ptr).AlarmType == checkparam.AlarmType)&& 
					((*ptr).TypeID == checkparam.TypeID)&&((*ptr).Freq == checkparam.Freq)&&(((*ptr).TypeID == "1")||((*ptr).TypeID == "0")))//ʧ���������ж�ChannelID
				{
					alarmparam = *ptr;
					ret = true;
					bFindFreq = 2;
					break;
				}
				if (((*ptr).DVBType == checkparam.DVBType)&&((*ptr).AlarmType == checkparam.AlarmType)&& 
					((*ptr).TypeID == checkparam.TypeID))
				{
					if(((*ptr).Freq == checkparam.Freq)&&((*ptr).ChannelID == checkparam.ChannelID))
					{
						alarmparam = *ptr;
						ret = true;
						bFindFreq = 2;
						break;
					}
					else if((*ptr).Freq =="All"||(*ptr).Freq =="ALL")
					{
						popularAlarmParam=*ptr;
						popularAlarmParam.ChannelID=checkparam.ChannelID;
						popularAlarmParam.Freq=checkparam.Freq;
						bFindFreq = 1;
					}
				}
			}
			if (bFindFreq == 1)
			{
				alarmparam = popularAlarmParam;
				ret = true;
			}
			break;
		}
	case ALARM_FREQ://Ƶ�㱨��
	case ALARM_TR101_290://290����
		{
			int bFindFreq = 0;//0û�ҵ����� 1�ҵ�All���� 2�ҵ�ȫƥ��
			sAlarmParam popularAlarmParam;//ͨ���� ��Ŀ��������
			for (std::vector<sAlarmParam>::iterator ptr=vecAlarmParam.begin();ptr!=vecAlarmParam.end();++ptr)
			{
				if (((*ptr).DVBType == checkparam.DVBType)&&((*ptr).AlarmType == checkparam.AlarmType)&&
					((*ptr).TypeID == checkparam.TypeID))
				{
					if(((*ptr).Freq == checkparam.Freq)&&((*ptr).ChannelID == checkparam.ChannelID))
					{
						alarmparam = *ptr;
						ret = true;
						bFindFreq = 2;
						break;
					}
					else if((*ptr).Freq =="All"||(*ptr).Freq =="ALL")
					{
						popularAlarmParam=*ptr;
						popularAlarmParam.Freq=checkparam.Freq;
						bFindFreq = 1;
					}
				}
			}
			if (bFindFreq == 1)
			{
				alarmparam = popularAlarmParam;
				ret = true;
			}
			break;
		}
	default:
		break;
	}
	return ret;
}

bool AlarmParamInfoMgr::UpdateAllAlarmParam( std::vector<sAlarmParam>& vecalaramparam )
{
	for (std::vector<sAlarmParam>::iterator ptr=vecalaramparam.begin();ptr!=vecalaramparam.end();++ptr)
	{
		if (ptr->Freq!="All")
		{
			continue;
		}
		ACE_Guard<ACE_Thread_Mutex> guard(AlarmMapMutex);
		std::vector<sAlarmParam>::iterator ptr_in=vecAlarmParam.begin();
		for (;ptr_in!=vecAlarmParam.end();++ptr_in)
		{
		    if (((*ptr).DVBType == (*ptr_in).DVBType) && ((*ptr).TypeID == (*ptr_in).TypeID) && ((*ptr).DeviceID == (*ptr_in).DeviceID) &&
				(((*ptr).AlarmType == ALARM_FREQ) || (((*ptr).AlarmType == ALARM_PROGRAM))))//��Ƶ��������Ŀ����
			{
				string freq=ptr_in->Freq;
				(*ptr_in) = (*ptr);//�������õĲ�����Ϣ����ԭ���Ĳ�����Ϣ
				ptr_in->Freq=freq;
				DBMANAGER::instance()->UpdateAlarmParam(*ptr_in);//���±���������Ϣ���ݿ�
			}
			else
			{
				continue;
			}
		}
		
	}
	return true;
}


bool AlarmParamInfoMgr::GetAlarmParamByDvbtype(enumDVBType eDvbtype, AlarmParamVec& vecRetAlarmParam)
{
	ACE_Guard<ACE_Thread_Mutex> guard(AlarmMapMutex);

	std::vector<sAlarmParam>::iterator paramIter = vecAlarmParam.begin();
	for (; paramIter!=vecAlarmParam.end(); paramIter++)
	{
		if ( (*paramIter).DVBType == eDvbtype)	//���������ͬ
		{
			vecRetAlarmParam.push_back( *paramIter );
		}
	}

	return true;
}