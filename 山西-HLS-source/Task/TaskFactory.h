///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TaskFactory.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-25
// �������������񹤳�����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

class XMLTask;
class TaskFactory
{
public:
	TaskFactory();
	virtual ~TaskFactory();
public:
	virtual XMLTask* CreateTask(std::string strStandardXML);

private:
	bool ValidateXML(std::string& xml);
};