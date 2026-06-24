#include "dpch.h"
#include "DLog.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace DEngine
{
	std::shared_ptr<spdlog::logger> DLog::s_CoreLogger;
	std::shared_ptr<spdlog::logger> DLog::s_ClientLogger;

	void DLog::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("DEngine");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}