// Ŭnicode please
#include "stdafx.h"
#include <string>
#include "sora/logger.h"

namespace sora {;

void ConsoleLogStream::WriteLog(const char *logger_name, LogLevel log_level, const char *msg) 
{
	printf("[%s] %s\n", logger_name, msg);
}

void ConsoleLogStream::Flush() 
{
	fflush(stdout);
}


////////////////////////////////////

sora::Logger<ConsoleLogStream> &SharedConsoleLogger() 
{
	// 템플릿 클래스 안에 대충 넣으니까 템플릿 인자가 없어서 안된다
	// 그래서 명시적으로 함수를 분리했다
	// 콘솔에 뭘 찍는 로그는 매크로와 연결되서 사실상
	// 공유해도 문제가 없고 그것이 더 추적하기 편할거같아서
	// 공용으로 접근할수있는거 하나 만들었다
	static sora::Logger<ConsoleLogStream> logger("Shared");
	return logger;
}

template<typename T>
struct LoggerNameFinder {
	LoggerNameFinder(T &name) : name(name) {}
	bool operator()(const AutoGenLoggerList::LoggerType &logger) 
	{
		return logger.name() == name;
	};
	T &name;
};

AutoGenLoggerList::AutoGenLoggerList() 
{
}
AutoGenLoggerList::~AutoGenLoggerList() 
{
}
AutoGenLoggerList::LoggerType &AutoGenLoggerList::GetLogger(const char *name) 
{
	LoggerListType &logger_list = GetInstance().logger_list_;
	auto found = find_if(logger_list.begin(), logger_list.end(), LoggerNameFinder<const char*>(name));
	if(found != logger_list.end()) {
		return *found;
	} else {
		LoggerType logger(name);
		logger_list.push_back(logger);
		return logger_list.back();
	}
}

void AutoGenLoggerList::SetAllState(bool state) 
{
	//지금까지 생성된 로그에 대해서만 처리가 가능
	auto it = logger_list_.begin();
	auto endit = logger_list_.end();
	for( ; it != endit ; it++) {
		it->enable = state;
	}
}
void AutoGenLoggerList::SetState(const std::string &name, bool state) 
{
	GetLogger(name.c_str());        //빈로그 미리 생성해야 이후에 확실히 적용
	auto found = find_if(logger_list_.begin(), logger_list_.end(), LoggerNameFinder<const std::string>(name));
	if(found != logger_list_.end()) {
		found->enable = state;
	}
}

}	//namespace sora