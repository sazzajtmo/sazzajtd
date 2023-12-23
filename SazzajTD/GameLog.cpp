#include "GameLog.h"
#include <format>
#include <cstdarg>
#include <iostream>

#ifdef _WINDOWS
	#include <Windows.h>
#endif

const size_t MAX_LOG_SIZE = 4096;

cGameLog* cGameLog::s_instance(nullptr);

cGameLog::cGameLog()
{
}

cGameLog::~cGameLog()
{
}

cGameLog* cGameLog::GetInstance()
{
	if( !s_instance )
		s_instance = new cGameLog();

	return nullptr;
}

void cGameLog::DestroyInstance()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

void cGameLog::Print(const std::string& fmt, ... )
{
	char szBuffer[MAX_LOG_SIZE] = { 0 };

	va_list args;
	va_start(args, fmt.c_str());
	_vsnprintf_s( szBuffer, MAX_LOG_SIZE - 2u, _TRUNCATE, fmt.c_str(), args);//-3 to leave room for \r\n at the very end
	size_t bufferLen = strnlen_s(szBuffer, _TRUNCATE);
	_vsnprintf_s( szBuffer + bufferLen, MAX_LOG_SIZE - bufferLen - 1, MAX_LOG_SIZE - bufferLen - 1, "\n", args);
	va_end(args);

	std::cout << szBuffer;	//for console

#ifdef _WIN32
	OutputDebugStringA( szBuffer );
#endif
}
