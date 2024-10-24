#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h" // Needed for logging some types

extern std::shared_ptr<spdlog::logger> s_PAPILogger;

void InitLog();

#ifndef PAPI_NO_LOG
	#define PAPI_TRACE(format, ...)                  s_PAPILogger->trace(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_INFO(format, ...)                   s_PAPILogger->info(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_INFO(format, ...)                   s_PAPILogger->info(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_WARN(format, ...)                   s_PAPILogger->warn(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_ERROR(format, ...)                  s_PAPILogger->error(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_CRITICAL(format, ...)               s_PAPILogger->critical(format "\n" __VA_OPT__(,) __VA_ARGS__)

	#define PAPI_TRACE_NO_NEWLINE(format, ...)       s_PAPILogger->trace(format __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_INFO_NO_NEWLINE(format, ...)        s_PAPILogger->info(format __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_WARN_NO_NEWLINE(format, ...)        s_PAPILogger->warn(format __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_ERROR_NO_NEWLINE(format, ...)       s_PAPILogger->error(format __VA_OPT__(,) __VA_ARGS__)
	#define PAPI_CRITICAL_NO_NEWLINE(format, ...)    s_PAPILogger->critical(format __VA_OPT__(,) __VA_ARGS__)
#else
	#define PAPI_TRACE(...)      
	#define PAPI_INFO(...)       
	#define PAPI_WARN(...)       
	#define PAPI_ERROR(...)      
	#define PAPI_CRITICAL(...)

	#define PAPI_TRACE_NO_NEWLINE(format, ...)     
	#define PAPI_INFO_NO_NEWLINE(format, ...)      
	#define PAPI_WARN_NO_NEWLINE(format, ...)      
	#define PAPI_ERROR_NO_NEWLINE(format, ...)     
	#define PAPI_CRITICAL_NO_NEWLINE(format, ...)  
#endif
