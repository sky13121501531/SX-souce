#pragma warning(disable:4996)
#include <time.h>
#include "TimeUtil.h"

/// <summary>��õ�ǰ������ʱ��ֵ</summary>
/// <param name=""></param>
/// <retvalue>string��������ʱ��</retvalue>
string TimeUtil::GetCurDateTime()
{
	string ret("");
	time_t cur_tm = time(0);
	struct tm *cur_t = localtime(&cur_tm);
	if (0 == cur_t) return ret;
	char now[64];	

	sprintf(now, "%04d-%02d-%02d %02d:%02d:%02d", cur_t->tm_year+1900, cur_t->tm_mon+1, cur_t->tm_mday, cur_t->tm_hour, cur_t->tm_min, cur_t->tm_sec);
	ret = now;
	return ret;
};

/// <summary>��õ�ǰ��ʱ��ֵ</summary>
/// <param name=""></param>
/// <retvalue>string����ʱ��</retvalue>
string TimeUtil::GetCurTime()
{
	string ret("");
	time_t cur_tm = time(0);
	struct tm *cur_t = localtime(&cur_tm);
	if (0== cur_t) return ret;
	char now[64];	

	sprintf(now, "%02d:%02d:%02d ",cur_t->tm_hour, cur_t->tm_min, cur_t->tm_sec);
	ret = now;
	return ret;
};


/// <summary>��õ�ǰ������ֵ</summary>
/// <param name=""></param>
/// <retvalue>string��������</retvalue>
string TimeUtil::GetCurDate()
{
	string ret("");
	time_t cur_tm = time(0);
	struct tm *cur_t = localtime(&cur_tm);
	if (0== cur_t) return ret;
	char now[64];	

	sprintf(now, "%04d-%02d-%02d", cur_t->tm_year+1900, cur_t->tm_mon+1, cur_t->tm_mday);
	ret = now;
	return ret;
};

//ʱ��ת��

/// <summary>time_t����ʱ��ת��Ϊ�ַ���</summary>
/// <param name="datetime"></param>
/// <retvalue>string��������ʱ��</retvalue>
string  TimeUtil::DateTimeToStr(time_t datetime)
{
	string ret("");
	struct tm *cur_t = localtime(&datetime);
	if (0 == cur_t) return ret;

	char now[64];	
	sprintf(now, "%04d-%02d-%02d %02d:%02d:%02d", cur_t->tm_year+1900, cur_t->tm_mon+1, cur_t->tm_mday, cur_t->tm_hour, cur_t->tm_min, cur_t->tm_sec);
	ret = now;
	return ret;
};

/// <summary>string����ʱ��ת��Ϊtime_t</summary>
/// <param name="datetime">��ʽΪ"yyyy-mm-dd- hh:mm:ss"</param>
/// <retvalue>string��������ʱ��</retvalue>
time_t TimeUtil::StrToDateTime(string datetime)
{
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	if (0== datetime.c_str()) return 0;

	int count = sscanf(datetime.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	if((datetime.find("/") != -1)&&(count < 3))
	{
		count = sscanf(datetime.c_str(), "%u/%u/%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	}
	if (count < 3) return 0;
	struct tm t;
	memset(&t, 0, sizeof(t));

	t.tm_year = year - 1900;
	t.tm_mon = mon - 1;
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = min;
	t.tm_sec = sec;
	return mktime(&t);
};

/// <summary>string����ʱ��ת��Ϊtime_t</summary>
/// <param name="datetime">��ʽΪ"yyyymmddhhmmss"</param>
/// <retvalue>string��������ʱ��</retvalue>
time_t TimeUtil::StrTimeToDateTime(string datetime)
{
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	if (0== datetime.c_str()) return 0;

	int count = sscanf(datetime.c_str(), "%4u%2u%2u%2u%2u%2u", &year, &mon, &day, &hour, &min, &sec);
	if (count < 3) return 0;
	struct tm t;
	memset(&t, 0, sizeof(t));

	t.tm_year = year - 1900;
	t.tm_mon = mon - 1;
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = min;
	t.tm_sec = sec;
	return mktime(&t);
};

/// <summary>time_tʱ��ת��Ϊstring</summary>
/// <param name="time"></param>
/// <retvalue>string����ʱ��</retvalue>
string TimeUtil::TimeToStr(time_t time)
{
	string ret("");
	char now[64];
	memset(now,NULL,64*sizeof(char));
	sprintf(now, "%02d:%02d:%02d",  int((time/3600)%24), int((time%3600)/60), int((time%3600)%60));
	ret = now;
	return ret;
};

/// <summary>ʱ���ַ���ת��Ϊʱ������</summary>
/// <param name="time">��ʽΪ"hh:mm:ss"����"hh:mm"����"hh"</param>
/// <retvalue>time_t����ʱ��</retvalue>
time_t TimeUtil::StrToTime(string time)
{
	int year, mon, day, hour, min, sec;
	year = 1970;
	mon = 1;
	day  = 1;
	hour = min = sec =0;
	if (0== time.c_str()) return 0;

	int count = sscanf(time.c_str(), "%u:%u:%u", &hour, &min, &sec);
	if (count == 0) { hour = 0; min = 0; sec = 0; }
	if (count == 1) { min = 0; sec = 0; }
	if (count == 2) { sec = 0; }
	struct tm t;
	memset(&t, 0, sizeof(t));

	t.tm_year = year - 1900;
	t.tm_mon = mon - 1;
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = min;
	t.tm_sec = sec;
	return mktime(&t);
};

/// <summary>ʱ���ַ���ת��Ϊ��</summary>
/// <param name="time">��ʽΪ"hh:mm:ss"����"hh:mm"����"hh"</param>
/// <retvalue>��</retvalue>
long TimeUtil::StrToSecondTime(string time)
{
	int year, mon, day, hour, min, sec;
	year = 1970;
	mon = 1;
	day  = 1;
	hour = min = sec =0;
	if (0== time.c_str()) return 0;

	int count = sscanf(time.c_str(), "%u:%u:%u", &hour, &min, &sec);
	if (count == 0) { hour = 0; min = 0; sec = 0; }
	if (count == 1) { min = 0; sec = 0; }
	if (count == 2) { sec = 0; }

	return (long)(((hour*60)+min)*60+sec);
};

//ȡ���ں�ʱ��
/// <summary>��õ�ǰ��ֵ</summary>
/// <param name=""></param>
/// <retvalue>string���͵���ֵ</retvalue>
string TimeUtil::GetCurDay()
{
	string ret("");
	time_t cur_tm = time(0);
	struct tm *cur_t = localtime(&cur_tm);
	if (0== cur_t) return ret;
	char now[64];	
	sprintf(now, "%02d",  cur_t->tm_mday);
	ret = now;
	return ret;
};

/// <summary>��õ�ǰ�·�ֵ</summary>
/// <param name=""></param>
/// <retvalue>string���͵��·�ֵ</retvalue>
string TimeUtil::GetCurMonth()
{
	string ret("");
	time_t cur_tm = time(0);
	struct tm *cur_t = localtime(&cur_tm);
	if (0== cur_t) return ret;
	char now[64];	
	sprintf(now, "%02d",  cur_t->tm_mon+1);
	ret = now;
	return ret;
};

/// <summary>��õ�ǰ���ֵ</summary>
/// <param name=""></param>
/// <retvalue>string���͵����ֵ</retvalue>
string TimeUtil::GetCuryear()
{
	string ret("");
	time_t cur_tm = time(0);
	struct tm *cur_t = localtime(&cur_tm);
	if (0== cur_t) return ret;
	char now[64];	
	sprintf(now, "%04d",  cur_t->tm_year+1900);
	ret = now;
	return ret;
};

/// <summary>����string������õ�ǰ���ֵ</summary>
/// <param name="datetime">��ʽΪyyyy-mm-dd hh:ss:mm</param>
/// <retvalue>string���͵����ֵ</retvalue>
string  TimeUtil::GetYearFromDatetime(string datetime)
{
	string ret= datetime;
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	if (0== datetime.c_str()) return datetime;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	string syear = "0000";
	sprintf((char *)syear.c_str(),"%04d",year);
	return syear;
};

/// <summary>����time_t������õ�ǰ���ֵ</summary>
/// <param name="datetime"></param>
/// <retvalue>string���͵����ֵ</retvalue>
string  TimeUtil::GetYearFromDatetime(time_t datetime)
{
	string ret("");
	ret = DateTimeToStr(datetime);

	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	sprintf((char *)ret.c_str(),"%04d",year);
	return ret;
};

/// <summary>����string������õ�ǰ���ֵ</summary>
/// <param name="datetime">��ʽΪyyyy-mm-dd hh:ss:mm</param>
/// <retvalue>string���͵��·�ֵ</retvalue>
string  TimeUtil::GetMonthFromDatetime(string datetime)
{
	string ret = datetime;
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	if (0== datetime.c_str()) return datetime;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	string smon = "00";
	sprintf((char *)smon.c_str(),"%02d",mon);;
	return smon;
};

/// <summary>����time_t������õ�ǰ���ֵ</summary>
/// <param name="datetime"></param>
/// <retvalue>string���͵��·�ֵ</retvalue>
string  TimeUtil::GetMonthFromDatetime(time_t datetime)
{
	string ret = DateTimeToStr(datetime);
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	sprintf((char *)ret.c_str(),"%02d",mon);;
	return ret;
};

/// <summary>����string������õ�ǰ��ֵ</summary>
/// <param name="datetime">��ʽΪyyyy-mm-dd hh:ss:mm</param>
/// <retvalue>string���͵���ֵ</retvalue>
string  TimeUtil::GetDayFromDatetime(string datetime)
{
	string ret= datetime;
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	if (0== datetime.c_str()) return datetime;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	string sday = "00";
	sprintf((char *)sday.c_str(),"%02d",day);
	return sday;
};

/// <summary>����time_t������õ�ǰ���ֵ</summary>
/// <param name="datetime"></param>
/// <retvalue>string���͵���ֵ</retvalue>
string  TimeUtil::GetDayFromDatetime(time_t datetime)
{
	string ret("");
	ret = DateTimeToStr(datetime);

	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	sprintf((char *)ret.c_str(),"%02d",day);
	return ret;
};

/// <summary>����string������õ�ǰ����ֵ</summary>
/// <param name="datetime">��ʽΪyyyy-mm-dd hh:ss:mm</param>
/// <retvalue>string���͵�����ֵ</retvalue>
string TimeUtil::GetDateFromDatetime(string datetime)
{
	string year,month,day;
	char ret[100];
	memset(ret,0,100);
	year = GetYearFromDatetime(datetime);
	month = GetMonthFromDatetime(datetime);
	day = GetDayFromDatetime(datetime);
	sprintf(ret,"%s-%s-%s",year.c_str(),month.c_str(),day.c_str());
	return ret;
};

/// <summary>����time_t������õ�ǰ����ֵ</summary>
/// <param name="datetime"></param>
/// <retvalue>string���͵�����ֵ</retvalue>
string TimeUtil::GetDateFromDatetime(time_t datetime)
{
	string year,month,day;
	char ret[100];
	memset(ret,0,100);
	year = GetYearFromDatetime(datetime);
	month = GetMonthFromDatetime(datetime);
	day = GetDayFromDatetime(datetime);
	sprintf(ret,"%s-%s-%s",year.c_str(),month.c_str(),day.c_str());
	return ret;
};

/// <summary>����string������õ�ǰʱ��ֵ</summary>
/// <param name="datetime">��ʽΪyyyy-mm-dd hh:ss:mm</param>
/// <retvalue>string���͵�ʱ��ֵ</retvalue>
string TimeUtil::GetTimeFromDatetime(string datetime)
{
	string ret= datetime;
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	if (0== datetime.c_str()) return datetime;
	int count = ::sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	sprintf((char *)ret.c_str(),"%02d:%02d:%02d",hour,min,sec);
	return ret;        
};

/// <summary>����time_t������õ�ǰʱ��ֵ</summary>
/// <param name="datetime"></param>
/// <retvalue>string���͵�ʱ��ֵ</retvalue>
string TimeUtil::GetTimeFromDatetime(time_t datetime)
{
	string ret = DateTimeToStr(datetime);
	int year, mon, day, hour, min, sec;
	year = mon = day = hour = min = sec =0;
	int count = sscanf(ret.c_str(), "%u-%u-%u %u:%u:%u", &year, &mon, &day, &hour, &min, &sec);
	sprintf((char *)ret.c_str(),"%02d:%02d:%02d",hour,min,sec);
	return ret;     
};

//ʱ�����
/// <summary>��ȡ������֮����������</summary>
/// <param name="SrcDateTime">ʱ�����</param>
/// <param name="DesDateTime">ʱ�����</param>
/// <retvalue>long���͵�����</retvalue>
long TimeUtil::DiffDay(string SrcDateTime,string DesDateTime)
{
	time_t Src =  StrToDateTime(SrcDateTime);
	time_t Dst =  StrToDateTime(DesDateTime);
	return (long)((Src - Dst) / ONEDAYLONG);
};

/// <summary>��ȡ������֮����������</summary>
/// <param name="SrcDateTime">ʱ�����</param>
/// <param name="DesDateTime">ʱ�����</param>
/// <retvalue>long���͵�����</retvalue>
long  TimeUtil::DiffDay(time_t SrcDateTime,time_t DesDateTime)
{
	return (long)((SrcDateTime - DesDateTime) / ONEDAYLONG);
};


long TimeUtil::DiffDate(string SrcDate,string DesDate)		//�Ƚ�����(��������ʱ�䣬ֻ������)
{
	string time = " 00:00:00";
	string SrcDateTime = SrcDate + time;
	string DesDateTime = DesDate + time;

	time_t Src =  StrToDateTime(SrcDateTime);
	time_t Dst =  StrToDateTime(DesDateTime);
	return (long)((Src - Dst) / ONEDAYLONG);
}


/// <summary>��ȡ������֮������Сʱ��</summary>
/// <param name="SrcDateTime">ʱ�����</param>
/// <param name="DesDateTime">ʱ�����</param>
/// <retvalue>long���͵�����</retvalue>
long  TimeUtil::DiffHour(string SrcDateTime,string DesDateTime)
{
	time_t Src =  StrToDateTime(SrcDateTime);
	time_t Dst =  StrToDateTime(DesDateTime);
	return (long)((Src - Dst) / ONEHOURLONG);
};

/// <summary>��ȡ������֮������Сʱ��</summary>
/// <param name="SrcDateTime">ʱ�����</param>
/// <param name="DesDateTime">ʱ�����</param>
/// <retvalue>long���͵�����</retvalue>
long  TimeUtil::DiffHour(time_t SrcDateTime,time_t DesDateTime)
{
	return (long)((SrcDateTime - DesDateTime) / ONEHOURLONG);
};

/// <summary>��ȡ������֮�����ķ�����</summary>
/// <param name="SrcDateTime">ʱ�����</param>
/// <param name="DesDateTime">ʱ�����</param>
/// <retvalue>long���͵�����</retvalue>
long  TimeUtil::DiffMinute(string SrcDateTime,string DesDateTime)
{
	time_t Src =  StrToDateTime(SrcDateTime);
	time_t Dst =  StrToDateTime(DesDateTime);
	return (long)((Src - Dst) / ONEMINUTELONG);
};

/// <summary>��ȡ������֮�����ķ�����</summary>
/// <param name="SrcDateTime">ʱ�����</param>
/// <param name="DesDateTime">ʱ�����</param>
/// <retvalue>long���͵�����</retvalue>
long  TimeUtil::DiffMinute(time_t SrcDateTime,time_t DesDateTime)
{
	return (long)((SrcDateTime - DesDateTime) / ONEMINUTELONG);
};

/// <summary>��ȡ������֮����������</summary>
/// <param name="SrcDateTime">ʱ�����</param>
/// <param name="DesDateTime">ʱ�����</param>
/// <retvalue>long���͵�����</retvalue>
long TimeUtil::DiffSecond(string SrcDateTime,string DesDateTime)
{
	time_t Src =  StrToDateTime(SrcDateTime);
	time_t Dst =  StrToDateTime(DesDateTime);
	return (long)(Src - Dst);
};

/// <summary>��ȡ������֮����������</summary>
/// <param name="SrcDateTime">ʱ�����</param>
/// <param name="DesDateTime">ʱ�����</param>
/// <retvalue>long���͵�����</retvalue>
long TimeUtil::DiffSecond(time_t SrcDateTime,time_t DesDateTime)
{
	return (long)(SrcDateTime - DesDateTime);
};

//���ڼ���
/// <summary>�������ڲ����������ֵ</summary>
/// <param name="date">ʱ�����</param>
/// <retvalue>long���͵�����</retvalue>
long  TimeUtil::DateIsWeekDay(string date)
{
	time_t d = StrToDateTime(date);
	struct tm *cur_t = localtime(&d);
	if (0 == cur_t) return -1;
	return cur_t->tm_wday;
};

/// <summary>�������ڲ����������ֵ</summary>
/// <param name="date">ʱ�����</param>
/// <retvalue>long���͵�����</retvalue>
long  TimeUtil::DateIsWeekDay(time_t date)
{
	struct tm * cur_t = localtime(&date);
	if (0 == cur_t) return -1;
	return cur_t->tm_wday;
};

//ʱ��Ӽ�
/// <summary>��ʱ�����ָ��������</summary>
/// <param name="datetime">ʱ�����</param>
/// <param name="Days">����</param>
/// <retvalue>string���͵�ʱ��</retvalue>
string TimeUtil::CalDay(string datetime,int Days)
{
	time_t curd = StrToDateTime(datetime);
	curd += (Days * ONEDAYLONG);
	string ret = DateTimeToStr(curd);
	return ret;
};

/// <summary>��ʱ�����ָ��������</summary>
/// <param name="datetime">ʱ�����</param>
/// <param name="Days">����</param>
/// <retvalue>string���͵�ʱ��</retvalue>
string TimeUtil::CalDay(time_t datetime, int Days)
{
	time_t curd = datetime;
	curd += (Days * ONEDAYLONG);
	string ret = DateTimeToStr(curd);
	return ret;
};

/// <summary>��ʱ�����ָ����Сʱ��</summary>
/// <param name="datetime">ʱ�����</param>
/// <param name="Hours">Сʱ��</param>
/// <retvalue>string���͵�ʱ��</retvalue>
string TimeUtil::CalHour(string datetime, int Hours)
{
	time_t curd = StrToDateTime(datetime);
	curd += (Hours * ONEHOURLONG);
	string ret = DateTimeToStr(curd);
	return ret;
};

/// <summary>��ʱ�����ָ����Сʱ��</summary>
/// <param name="datetime">ʱ�����</param>
/// <param name="Hours">Сʱ��</param>
/// <retvalue>string���͵�ʱ��</retvalue>
string TimeUtil::CalHour(time_t datetime, int Hours)
{
	time_t curd = datetime;
	curd += (Hours * ONEHOURLONG);
	string ret = DateTimeToStr(curd);
	return ret;
};

/// <summary>��ʱ�����ָ���ķ�����</summary>
/// <param name="datetime">ʱ�����</param>
/// <param name="Minutes">������</param>
/// <retvalue>string���͵�ʱ��</retvalue>
string TimeUtil::CalMinutes(string datetime, int Minutes)
{
	time_t curd = StrToDateTime(datetime);
	curd += (Minutes * ONEMINUTELONG);
	string ret = DateTimeToStr(curd);
	return ret;
};

/// <summary>��ʱ�����ָ���ķ�����</summary>
/// <param name="datetime">ʱ�����</param>
/// <param name="Minutes">������</param>
/// <retvalue>string���͵�ʱ��</retvalue>
string TimeUtil::CalMinutes(time_t datetime, int Minutes)
{
	time_t curd = datetime;
	curd += (Minutes * ONEMINUTELONG);
	string ret = DateTimeToStr(curd);
	return ret;
};

std::string TimeUtil::DateTimeToString( time_t time )
{
	string ret("");
	struct tm *cur_t = localtime(&time);
	if (0 == cur_t) return ret;

	char now[64];	
	sprintf(now, "%04d%02d%02d%02d%02d%02d", cur_t->tm_year+1900, cur_t->tm_mon+1, cur_t->tm_mday, cur_t->tm_hour, cur_t->tm_min, cur_t->tm_sec);
	ret = now;
	return ret;
}

std::string TimeUtil::GetEndLessTime()
{
	return DateTimeToStr(StrToDateTime(CalDay(TimeUtil::GetCurDateTime(),365*10)));
}