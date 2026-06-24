#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace DEngine
{
	class D_API DLog
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

//Core log macros
#define D_CORE_TRACE(...) ::DEngine::DLog::GetCoreLogger()->trace(__VA_ARGS__)
#define D_CORE_INFO(...)  ::DEngine::DLog::GetCoreLogger()->info(__VA_ARGS__)
#define D_CORE_WARN(...)  ::DEngine::DLog::GetCoreLogger()->warn(__VA_ARGS__)
#define D_CORE_ERROR(...) ::DEngine::DLog::GetCoreLogger()->error(__VA_ARGS__)
#define D_CORE_FATAL(...) ::DEngine::DLog::GetCoreLogger()->fatal(__VA_ARGS__)

//App log macros
#define D_TRACE(...) ::DEngine::DLog::GetClientLogger()->trace(__VA_ARGS__)
#define D_INFO(...)  ::DEngine::DLog::GetClientLogger()->info(__VA_ARGS__)
#define D_WARN(...)  ::DEngine::DLog::GetClientLogger()->warn(__VA_ARGS__)
#define D_ERROR(...) ::DEngine::DLog::GetClientLogger()->error(__VA_ARGS__)
#define D_FATAL(...) ::DEngine::DLog::GetClientLogger()->fatal(__VA_ARGS__)
