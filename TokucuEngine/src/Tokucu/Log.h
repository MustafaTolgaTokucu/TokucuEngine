#pragma once


#include "core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h" 


namespace Tokucu {
	class TOKUCU_API Log
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
#define TKC_CORE_ERROR(...)		:: Tokucu::Log::GetCoreLogger()->error(__VA_ARGS__)
#define TKC_CORE_WARN(...)		:: Tokucu::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TKC_CORE_INFO(...)		:: Tokucu::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TKC_CORE_TRACE(...)		:: Tokucu::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TKC_CORE_FATAL(...)		:: Tokucu::Log::GetCoreLogger()->fatal(__VA_ARGS__)
//Client log macros
#define TKC_ERROR(...)		:: Tokucu::Log::GetClientLogger()->error(__VA_ARGS__)
#define TKC_WARN(...)		:: Tokucu::Log::GetClientLogger()->warn(__VA_ARGS__)
#define TKC_INFO(...)		:: Tokucu::Log::GetClientLogger()->info(__VA_ARGS__)
#define TKC_TRACE(...)		:: Tokucu::Log::GetClientLogger()->trace(__VA_ARGS__)
#define TKC_FATAL(...)		:: Tokucu::Log::GetClientLogger()->fatal(__VA_ARGS__)