#include "RunPlanInfoMgr.h"
#include "DownXMLProcess.h"
#include "../Communications/CommunicationMgr.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation//XmlParser.h"
#include "../Foundation/StrUtil.h"
using namespace std;

RunPlanInfoMgr::RunPlanInfoMgr()
{
	InitPlanInfo();//��ʼ��
}

RunPlanInfoMgr::~RunPlanInfoMgr()
{
}

bool RunPlanInfoMgr::InitPlanInfo()
{
	ACE_Guard<ACE_Thread_Mutex> guard(m_RunPlanMutex);
	DBMANAGER::instance()->QueryAllRunPlanParam(mRunPlanParamVec);//���ݿ��в�ѯ����ͼ��Ϣ
	return true;
}

bool RunPlanInfoMgr::UpdatePlanInfo(enumDVBType dvbtype,const std::string& ChannelID,const RunPlanParamVec& NewRunPlanParamVec)
{
	try
	{
		ACE_Guard<ACE_Thread_Mutex> guard(m_RunPlanMutex);
		//ɾ��ԭ��dvbtype������ͼ
		RunPlanParamVec::iterator ptr = mRunPlanParamVec.begin();
		for (;ptr!=mRunPlanParamVec.end();)
		{
			if (ptr->dvbtype == dvbtype /*&& ptr->ChannelID == ChannelID*/)
				ptr = mRunPlanParamVec.erase(ptr);
			else
				++ ptr;
		}
		//����µ�ChannelID������ͼ
		RunPlanParamVec::const_iterator ptr_new = NewRunPlanParamVec.begin();
		for (;ptr_new!=NewRunPlanParamVec.end();++ptr_new)
		{
			mRunPlanParamVec.push_back(*ptr_new);
		}

		DBMANAGER::instance()->UpdateRunPlanParam(dvbtype,ChannelID,NewRunPlanParamVec);//��������ͼ��Ϣ
	}
	catch ( ... )
	{
		string msg = "��������ͼ��Ϣʧ��!!\n";
		//SYSMSGSENDER::instance()->SendMsg(msg, UNKNOWN, VS_MSG_SYSALARM);
		//ACE_DEBUG((LM_ERROR,"(%T| %t)��������ͼ��Ϣʧ��!!\n"));
		return false;
	}
	
	return  true;
}
bool RunPlanInfoMgr::InRunPlan(enumDVBType dvbtype,const std::string& ChannelID)
{
	std::string curdatetime=TimeUtil::GetCurDateTime();
	std::string curday=TimeUtil::GetCurDay();
	std::string curmonth=TimeUtil::GetCurMonth();
	std::string dateweek=StrUtil::Long2Str(TimeUtil::DateIsWeekDay(curdatetime));
	std::string curtime=TimeUtil::GetCurTime();

	try
	{
		ACE_Guard<ACE_Thread_Mutex> guard(m_RunPlanMutex);
		
		RunPlanParamVec::iterator ptr = mRunPlanParamVec.begin();
		for (;ptr!=mRunPlanParamVec.end();++ptr)
		{
			//ģ��ģ���жϣ�ģ����������������ͼ��Ϊͣ�����߼��ޣ�����ͼ�е�Type��Ч
			if(ATV==dvbtype||CTV==dvbtype||RADIO==dvbtype||AM==dvbtype)
			{
				if((*ptr).dvbtype==dvbtype && (*ptr).ChannelID==ChannelID && \
					curdatetime>=(*ptr).ValidStartDateTime && curdatetime<=(*ptr).ValidEndDateTime)
				{
					if(((*ptr).DayOfWeek=="ALL" || dateweek==(*ptr).DayOfWeek) && \
						((*ptr).StartTime<=curtime && curtime<=(*ptr).EndTime))					//ѭ��
					{
						return false;
					}
					else if((*ptr).StartDateTime!="" && (*ptr).EndDateTime!="" && \
						(curdatetime>=(*ptr).StartDateTime && curdatetime<=(*ptr).EndDateTime))	//����
					{
						return false;
					}
				}
			}
			else if(DVBC==dvbtype||DVBS==dvbtype||CTTB==dvbtype)
			{
				if((*ptr).dvbtype==dvbtype && (*ptr).ChannelID==ChannelID && ((*ptr).Type==0 ||(*ptr).Type==2) && \
					curdatetime>=(*ptr).ValidStartDateTime && curdatetime<=(*ptr).ValidEndDateTime)
				{
					if(((((*ptr).Month=="ALL" || (*ptr).Month==curmonth) && curday==(*ptr).DayOfMonth)||((*ptr).DayOfWeek=="ALL" ||(*ptr).DayOfWeek=="0"|| dateweek==(*ptr).DayOfWeek)) && \
						((*ptr).StartTime<=curtime && curtime<=(*ptr).EndTime))					//ѭ��
					{
						return false;
					}
					else if((*ptr).StartDateTime!=""&&(*ptr).EndDateTime!=""&&(curdatetime>=(*ptr).StartDateTime&&curdatetime<=(*ptr).EndDateTime))
					{
						return false;
					}
				}
			}
		}
	}
	catch (...)
	{
		string msg = "����ͼ�жϺ����쳣!!\n";
		//SYSMSGSENDER::instance()->SendMsg(msg, UNKNOWN, VS_MSG_SYSALARM);
	}
	
	return true;
}



//����dvbtype��ȡ����ͼ��Ϣ
bool RunPlanInfoMgr::GetRunPlanByDvbtype(enumDVBType eDvbtype, RunPlanParamVec& vecRunPlanInfo)	
{
	ACE_Guard<ACE_Thread_Mutex> guard(m_RunPlanMutex);

	RunPlanParamVec::iterator runplanIter = mRunPlanParamVec.begin();
	for (; runplanIter!=mRunPlanParamVec.end(); runplanIter++)
	{
		if ( (*runplanIter).dvbtype == eDvbtype )
		{
			vecRunPlanInfo.push_back( *runplanIter );
		}
	}
	//û�и����͵�����ͼ��Ϣ����Ϊ��ȡʧ��
	if (vecRunPlanInfo.empty())
		return false;

	return true;
}


// ��ⱨ��ʱ���Ƿ�������ͼ����
bool RunPlanInfoMgr::CheckAlarmTime(enumDVBType dvbtype,const std::string& ChannelID,time_t& AlarmTime)
{
	try
	{
		std::string curdatetime=TimeUtil::DateTimeToStr(AlarmTime);		
		std::string curDate = TimeUtil::GetDateFromDatetime(AlarmTime);
		std::string curday=TimeUtil::GetDayFromDatetime(curdatetime);
		std::string curmonth=TimeUtil::GetMonthFromDatetime(curdatetime);
		std::string dateweek=StrUtil::Long2Str(TimeUtil::DateIsWeekDay(curdatetime));
		std::string curtime=TimeUtil::TimeToStr(AlarmTime);

		ACE_Guard<ACE_Thread_Mutex> guard(m_RunPlanMutex);

		RunPlanParamVec::iterator ptr = mRunPlanParamVec.begin();
		for (;ptr!=mRunPlanParamVec.end();ptr++)
		{
			//ģ��ģ���жϣ�ģ����������������ͼ��Ϊͣ�����߼��ޣ�����ͼ�е�Type��Ч
			if(ATV==dvbtype||CTV==dvbtype||RADIO==dvbtype||AM==dvbtype)
			{
				if((*ptr).dvbtype==dvbtype && (*ptr).ChannelID==ChannelID && \
					curdatetime>=(*ptr).ValidStartDateTime && curdatetime<=(*ptr).ValidEndDateTime)
				{
					if(((*ptr).DayOfWeek=="ALL" || dateweek==(*ptr).DayOfWeek) && \
						((*ptr).StartTime<=curtime && curtime<=(*ptr).EndTime))					//ѭ��
					{
						std::string tempEndDateTime = curDate + " " + (*ptr).EndTime;
						AlarmTime = TimeUtil::StrToDateTime(tempEndDateTime)+1;		//��ʱ������Ϊ����ͼ�Ľ���ʱ��+1
					}
					else if((*ptr).StartDateTime!="" && (*ptr).EndDateTime!="" && \
						(curdatetime>=(*ptr).StartDateTime && curdatetime<=(*ptr).EndDateTime))	//����
					{
						std::string tempEndDateTime = (*ptr).EndDateTime;
						AlarmTime = TimeUtil::StrToDateTime(tempEndDateTime)+1;		//��ʱ������Ϊ����ͼ�Ľ���ʱ��+1
					}
				}
			}
			else if(DVBC==dvbtype||DVBS==dvbtype)
			{
				if((*ptr).dvbtype==dvbtype && (*ptr).ChannelID==ChannelID && ((*ptr).Type==0 ||(*ptr).Type==2) && \
					curdatetime>=(*ptr).ValidStartDateTime && curdatetime<=(*ptr).ValidEndDateTime)
				{
					if(((((*ptr).Month=="ALL" || (*ptr).Month==curmonth) && curday==(*ptr).DayOfMonth)||(dateweek==(*ptr).DayOfWeek)) && \
						((*ptr).StartTime<=curtime && curtime<=(*ptr).EndTime))					//ѭ��
					{
						std::string tempEndDateTime = curDate + " " + (*ptr).EndTime;
						AlarmTime = TimeUtil::StrToDateTime(tempEndDateTime)+1;		//��ʱ������Ϊ����ͼ�Ľ���ʱ��+1
					}
					else if((*ptr).StartDateTime!=""&&(*ptr).EndDateTime!=""&&(curdatetime>=(*ptr).StartDateTime&&curdatetime<=(*ptr).EndDateTime))
					{
						std::string tempEndDateTime = (*ptr).EndDateTime;
						AlarmTime = TimeUtil::StrToDateTime(tempEndDateTime)+1;		//��ʱ������Ϊ����ͼ�Ľ���ʱ��+1
					}
				}
			}
		}
	}
	catch (...)
	{
		string msg = "����ʱ���������ͼУ�������쳣!!\n";
		//SYSMSGSENDER::instance()->SendMsg(msg, UNKNOWN, VS_MSG_SYSALARM);
	}
	
	return true;
}