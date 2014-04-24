#include <stdarg.h>
#include "LoggerAutoLink.h"
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>
#include "Logger.h"
using namespace log4cplus;

namespace Engine
{
	namespace Logging
	{
		void InitLogger(char *cfgFile)
		{
			log4cplus::initialize();
			std::string file(cfgFile);
			log4cplus::PropertyConfigurator::doConfigure(log4cplus::tstring(file.begin(),file.end()));
		}

		void DebugLogging(const char * format, ...)
		{
			char message[1000]={0};
			va_list args;
			va_start ( args, format );
			vsprintf ( message, format, args );
			va_end ( args );
			LOG4CPLUS_DEBUG(Logger::getRoot (), message);
		}

		void InfoLogging(const char * format, ...)
		{
			char message[1000]={0};
			va_list args;
			va_start ( args, format );
			vsprintf ( message, format, args );
			va_end ( args );
			LOG4CPLUS_INFO(Logger::getRoot (), message);
		}

		void WarnLogging(const char * format, ...)
		{
			char message[1000]={0};
			va_list args;
			va_start ( args, format );
			vsprintf ( message, format, args );
			va_end ( args );
			LOG4CPLUS_WARN(Logger::getRoot (), message);
		}

		void ErrorLogging(const char * format, ...)
		{
			char message[1000]={0};
			va_list args;
			va_start ( args, format );
			vsprintf ( message, format, args );
			va_end ( args );
			LOG4CPLUS_ERROR(Logger::getRoot (), message);
		}
	}
}