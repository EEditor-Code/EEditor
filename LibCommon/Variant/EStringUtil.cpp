// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#include "EString.h"
#include "DefineCommon.h"
#include <cctype>
#include <cstdio>

#ifdef WIN32
#include <string/pystring.h>
#else
#include <string/pystring.h>
#endif

namespace
{
bool isSafeDoubleFormat(const char* fmt)
{
    if (fmt == nullptr || *fmt == '\0')
    {
        return false;
    }

    int conversions = 0;
    for (const char* p = fmt; *p != '\0'; ++p)
    {
        if (*p != '%')
        {
            continue;
        }

        ++p;
        if (*p == '%')
        {
            continue;
        }

        while (*p == '-' || *p == '+' || *p == ' ' || *p == '#' || *p == '0')
        {
            ++p;
        }
        while (std::isdigit(static_cast<unsigned char>(*p)))
        {
            ++p;
        }
        if (*p == '.')
        {
            ++p;
            while (std::isdigit(static_cast<unsigned char>(*p)))
            {
                ++p;
            }
        }
        if (*p == 'l')
        {
            ++p;
        }
        if (*p != 'f' && *p != 'F' && *p != 'e' && *p != 'E' &&
            *p != 'g' && *p != 'G' && *p != 'a' && *p != 'A')
        {
            return false;
        }
        ++conversions;
    }
    return conversions == 1;
}
}

namespace EStringUtil
{

EString subStr(const EString& s, size_t posBegin, size_t len)
{
    if (posBegin >= s.size() || len == 0 || posBegin + len > s.size() || s.empty())
    {
        ELOG_INFO("posBegin or len is not correct");
        return EString();
    }
    return s.substr(posBegin, len);
}

void remove(EString& s, const EString& subStr)
{
    if (subStr.empty())
    {
        return;
    }

    size_t lenOld = subStr.size();
    size_t posFindBegin = 0;

    while (true)
    {
        size_t posOld = s.find(subStr, posFindBegin);
        if (posOld == EString::npos)
            break;

        s.erase(posOld, lenOld);
        posFindBegin = posOld;
    }
}

EString replace(const EString& s, const EString& oldStr, const EString& newStr)
{
    return pystring::replace(s, oldStr, newStr);
}

EStringList split(const EString& s, const EString& sep)
{
    EStringList result;
    pystring::split(s, result, sep);
    if (result.empty())
    {
        result.push_back(s);
    }
    return result;
}

EString number(double value, const char* formatStr)
{
    const char* fmt = isSafeDoubleFormat(formatStr) ? formatStr : "%f";
    char data[64];
    int written = std::snprintf(data, sizeof(data), fmt, value);
    if (written < 0)
    {
        return EString();
    }
    return EString(data);
}

bool listContains(const EStringList& list, const EString& str)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        if (list[i] == str)
            return true;
    }
    return false;
}

EString listGetPrintStr(const EStringList& strList)
{
    EString temp = EString("EStringList.size=") + std::to_string(strList.size()) + "\n";
    for (size_t i = 0; i < strList.size(); i++)
    {
        temp += strList[i] + "\n";
    }
    return temp;
}

} // namespace EStringUtil
