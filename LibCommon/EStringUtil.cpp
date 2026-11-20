// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 北京格瑞特皮软件科技有限公司

#include "EString.h"
#include "DefineCommon.h"
#include <cstdio>

#ifdef WIN32
#include <string/pystring.h>
#else
#include <string/pystring.h>
#endif

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
    char data[64];
    sprintf(data, formatStr, value);
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
