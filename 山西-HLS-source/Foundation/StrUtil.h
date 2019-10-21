#ifndef __STRUTIL__
#define __STRUTIL__

#include <string>
#include "TypeDef.h"

using namespace std;

class StrUtil
{
public:

static string RoundFloat(double fOriginal,int nSaveBits);

static int Str2Int(string Str);
static string Int2Str(int integer);

static long Str2Long(string Str);
static string Long2Str(long value);


static long CiFang(long baseInt,long numCF);

static float Str2Float(string str);
static float Str2FloatForFMFreq(string str);
static string Float2Str(float floater);
static string Float2Str1(float floater);//�������뱣��һλС��

static int Str2IntForQam(string str);

static void ConvertUtf8ToGBK(const char* strUtf8,std::string& strGBK);//utf8ת��ΪGBK
static void ConvertGBKToUtf8(const char* strGBK,std::string& strUtf8);//Gbkת��Ϊutf8

//static void ConvertETR290TypeId(eDVBType dvbtype,std::string srcTypeID,std::string& dstTypeID);
//static void ConvertProgramTypeId(eDVBType dvbtype,std::string srcTypeID,std::string& dstTypeID);

static void ConvertEquipTypeIn(string& type);
static void ConvertETR290TypeIDToAlarmID(sCheckParam& parm);
static void ConvertProgramTypeIDToAlarmID(sCheckParam& parm);
static void ConvertQualityTypeIDToAlarmID(sCheckParam& parm);
static void ConvertEnvTypeIDToAlarmID(sCheckParam& parm);

};
#endif

