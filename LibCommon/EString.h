// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 北京格瑞特皮软件科技有限公司

#ifndef E_STRING_H
#define E_STRING_H

#include <string>
#include <vector>
#include <DefineLibCommon.h>

using EString = std::string;
using EStringList = std::vector<EString>;

namespace EStringUtil
{
    inline bool isEmpty(const EString& s) { return s.empty(); }

    COMMON_DLL EString subStr(const EString& s, size_t posBegin, size_t len);
    inline EString left(const EString& s, size_t len) { return s.substr(0, len); }
    inline EString right(const EString& s, size_t len) { return s.substr(s.size() - len); }

    inline void removeLeft(EString& s, size_t len) { s.erase(0, len); }
    COMMON_DLL void remove(EString& s, const EString& subStr);

    COMMON_DLL EString replace(const EString& s, const EString& oldStr, const EString& newStr);

    COMMON_DLL EStringList split(const EString& s, const EString& sep);

    inline bool contains(const EString& s, const EString& subStr) {
        return s.find(subStr) != EString::npos;
    }

    inline EString number(int value)         { return std::to_string(value); }
    inline EString number(long value)        { return std::to_string(value); }
    inline EString number(size_t value)      { return std::to_string(value); }
    COMMON_DLL EString number(double value, const char* formatStr = "%f");

    inline EString boolValue(bool value)     { return value ? "true" : "false"; }
    inline EString fromStdString(const std::string& s) { return s; }

    COMMON_DLL bool listContains(const EStringList& list, const EString& str);
    COMMON_DLL EString listGetPrintStr(const EStringList& strList);
}

#endif
