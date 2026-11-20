// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 北京格瑞特皮软件科技有限公司

#ifndef DEFINE_LIB_COMMON
#define DEFINE_LIB_COMMON 1

/*
 * 跨平台导出宏。
 * - Windows: 构建 LibCommon 时定义 LIB_COMMON_EXPORT => dllexport；使用方默认 dllimport。
 * - GCC/Clang: 默认可见性导出(配合 -fvisibility=hidden 时仍可导出本库符号)。
 * - NO_DLL_STATEMENT: 强制空宏(用于静态库或仅生成 compile_commands 的场景)。
 */
#ifdef NO_DLL_STATEMENT
    #define COMMON_DLL
#elif defined(_WIN32)
    #ifdef LIB_COMMON_EXPORT
        #define COMMON_DLL __declspec(dllexport)
    #else
        #define COMMON_DLL __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #define COMMON_DLL __attribute__((visibility("default")))
#else
    #define COMMON_DLL
#endif

#endif
