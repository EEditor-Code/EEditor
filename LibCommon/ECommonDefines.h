// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#ifndef E_COMMON_DEFINES_H
#define E_COMMON_DEFINES_H 1

// 日志宏定义
#include <log/log.h>

#define ELOG_DEBUG(...) log_debug(__VA_ARGS__);
#define SET_LOG_LEVEL_DEBUG log_set_level(LOG_DEBUG);

#define ELOG_INFO(...) log_info(__VA_ARGS__);
#define SET_LOG_LEVEL_INFO log_set_level(LOG_INFO);

#define ELOG_WARN(...) log_warn(__VA_ARGS__);
#define SET_LOG_LEVEL_WARN log_set_level(LOG_WARN);

#define ELOG_ERROR(...) log_error(__VA_ARGS__);
#define SET_LOG_LEVEL_ERROR log_set_level(LOG_ERROR);

/**基础类型定义(向量/矩阵/字符串等)。
 *  EMatrix4x4 后续需要重写, 以支持数据赋值。*/
#include <Variant/EVariant.h>
#include <Variant/EString.h>

// 智能指针定义
#include <SmartPtr/ESmartPtr.h>

// 数学常量
#define PI_VALUE 3.141592653589793
#define PI_HALF_VALUE 1.570796326794896
#define DEGREE_TO_RADIUS(a) a/180.0*PI_VALUE
#define RADIUS_TO_DEGREE(a) a/PI_VALUE*180.0

#endif
