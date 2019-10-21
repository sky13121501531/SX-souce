///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DeviceIndependentTask.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-26
// �������������豸�����޹ص�������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "XMLTask.h"

class DeviceIndependentTask : public XMLTask
{
public:
	DeviceIndependentTask();
	DeviceIndependentTask(std::string strXML);
	virtual ~DeviceIndependentTask();
public:
	bool SendCASXML(const std::string& dstUrl,const std::string& upXML);
	std::string GetBaseObjectName() {return std::string("DeviceIndependentTask");}
	virtual bool Stop(void){return true;};			//����ֹͣ�������豸�޹�������ֹͣ
	virtual bool IsRealTimeTask(){return true;};	//�豸�޹�������Ҫʵʱִ��
};