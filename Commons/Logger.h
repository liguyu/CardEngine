/*
  日志组件，包装log4cplus-1.1.2-rc2，
  记录的信息内容可根据需要删减、补充定制
*/

#ifndef __LOGGER_INC
#define __LOGGER_INC

#ifdef COMMONS_EXPORTS
#define COMMONS_API __declspec(dllexport)
#else
#define COMMONS_API __declspec(dllimport)
#endif

#include "targetver.h"

namespace Engine
{
	namespace Logging
	{
		COMMONS_API void InitLogger(char * cfgFile);
		COMMONS_API void DebugLogging(const char * format, ...);
		COMMONS_API void InfoLogging(const char * format, ...);
		COMMONS_API void WarnLogging(const char * format, ...);
		COMMONS_API void ErrorLogging(const char * format, ...);
	}
}
#endif  // __LOGGER_INC
