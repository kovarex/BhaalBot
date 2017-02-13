// We're intentionaly not using #pragma once
#include <stdlib.h>

#ifndef DISABLE_LOGGING
  #include <Logger.hpp>

/** If logger object has been created, log to it, otherwise log to the console. */
  #define LOG(...) \
  do \
  { \
    ::logger->log(__FILE__, __LINE__, __VA_ARGS__); \
  } \
  while (0)

// Definitions of ENABLE_LOG_DEBUG may change between invocations of this header
// file, so we need to redefine these three every time.
  #undef LOG_DEBUG
  #undef LOG_VERBOSE
  #undef LOG_DEBUG_ACTIVE

  #if defined(ENABLE_DEBUG_LOG) && defined(DEBUG)

    #define LOG_DEBUG(...) LOG(LogLevel::Debug, __VA_ARGS__)
    #define LOG_VERBOSE(...) LOG(LogLevel::Verbose, __VA_ARGS__)
    #define LOG_DEBUG_ACTIVE 1

  #else // defined(ENABLE_DEBUG_LOG) && defined(DEBUG)

    #define LOG_DEBUG(...) ((void)0)
    #define LOG_VERBOSE(...) \
  do \
  { \
    ::logger->log(__FILE__, __LINE__, LogLevel::Verbose, __VA_ARGS__); \
  } \
  while (0)
    #define LOG_DEBUG_ACTIVE 0

  #endif // defined(ENABLE_DEBUG_LOG) && defined(DEBUG)

  #define FLUSH_LOG() logger->flush();

  #define LOG_INFO(...) LOG(LogLevel::Info, __VA_ARGS__)
  #define LOG_NOTICE(...) LOG(LogLevel::Notice, __VA_ARGS__)
  #define LOG_WARNING(...) LOG(LogLevel::Warning, __VA_ARGS__)
  #define LOG_ERROR(...) LOG(LogLevel::Error, __VA_ARGS__)

  #define LOG_STACKTRACE() \
  do \
  { \
    ::logger->logStacktrace(nullptr); \
  } \
  while (0)

  #define LOG_AND_ABORT(...) \
  do \
  { \
    LOG_STACKTRACE(); \
    LOG_ERROR(__VA_ARGS__); \
    FLUSH_LOG(); \
    Logger::skipStacktrace = true; \
    abort(); \
  } \
  while (0)

#define LOG_AND_ABORT_NEVER_THROW(...) \
  do \
  { \
    LOG_STACKTRACE(); \
    LOG_ERROR(__VA_ARGS__); \
    FLUSH_LOG(); \
    abort(); \
  } \
  while (0)

#define LOG_DEBUG_VALUE(value) \
  LOG_DEBUG(# value " = %s", StringUtil::str(value).c_str())

#else // DISABLE_LOGGING

  #define LOG_DEBUG(...) ((void)0)
  #define LOG_DEBUG_ACTIVE 0

  #define LOG_VERBOSE(...) ((void)0)

  #define FLUSH_LOG() ((void)0)

  #define LOG_INFO(...) ((void)0)
  #define LOG_NOTICE(...) ((void)0)
  #define LOG_WARNING(...) ((void)0)
  #define LOG_ERROR(...) ((void)0)
  #define LOG_STACKTRACE() ((void)(0))

  #define LOG_AND_ABORT(...) abort()
  #define LOG_AND_ABORT_NEVER_THROW(...) abort()
  #define LOG_DEBUG_VALUE(value) ((void)0)

#endif // DISABLE_LOGGING
