/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <DefineLibCommon.h>

#define LOG_VERSION "0.1.0"


struct log_Event{
  va_list ap;
  const char *fmt;
  const char *file;
  struct tm *time = nullptr;
  void *udata;
  int line;
  const char *funName;
  int level;
} ;

typedef void (*log_LogFn)(log_Event *ev);
typedef void (*log_LockFn)(bool lock, void *udata);

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO,  __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN,  __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__)

const char* log_level_string(int level);
COMMON_DLL void log_set_lock(log_LockFn fn, void *udata);
COMMON_DLL void log_set_level(int level);
void log_set_quiet(bool enable);
COMMON_DLL int log_add_callback(log_LogFn fn, void *udata, int level);
int log_add_fp(FILE *fp, int level);

// 新增：文件日志管理 API（方案 A）
COMMON_DLL int log_set_log_dir(const char* dir_path);
COMMON_DLL int log_set_filename_pattern(const char* pattern);
COMMON_DLL int log_enable_file(const char* dir_path, const char* filename_pattern, int level);
COMMON_DLL const char* log_get_log_dir(void);
COMMON_DLL bool log_is_file_enabled(void);
COMMON_DLL void log_flush_file(void);

COMMON_DLL void log_log(int level, const char *file, int line, const char *funName, const char *fmt, ...);

#endif
