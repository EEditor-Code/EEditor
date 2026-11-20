// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#include "EItemMetaProperty.h"
#include <EItem.h>

namespace
{
/**矩阵属性变更序号(见 EItemMetaProperty::getMatrixChangeSeq)*/
uint64_t g_matrixChangeSeq = 0;
}

uint64_t EItemMetaProperty::getMatrixChangeSeq()
{
    return g_matrixChangeSeq;
}

//TODO: if m_funSetProperty==null, then the owner's value will not be set, how to handle this problem?
void EItemMetaProperty::setPropertyValue(const EVariant& value)
{
    // 仅设置属性值(写 meta 缓存 + autogen setter 写成员); 不通知 model。
    // 需要通知的调用方(UI 面板/RPC/Lua/undo 恢复)使用 setPropertyValueAndNotify。
    this->value = value;
    if(this->m_funSetProperty != NULL)
    {
        this->m_funSetProperty(this->m_owner, this->value);
    }
}

void EItemMetaProperty::setPropertyValueAndNotify(const EVariant& value)
{
    // 写值前记录真实旧值(统一经 getPropertyValue: getter 优先同步缓存, 无 getter 回落缓存)
    EVariant oldValue;
    this->getPropertyValue(oldValue);

    setPropertyValue(value);

    // 矩阵属性变更计数: 供"父链变换可能已变"的判据(见 getMatrixChangeSeq 注释)
    if (this->name == "ui_matrix")
    {
        ++g_matrixChangeSeq;
    }
}

void EItemMetaProperty::getPropertyValue(EVariant& value)
{
    if(this->m_funGetProperty != NULL)
    {
        this->m_funGetProperty(this->m_owner, this->value);
    }

    value = this->value;
}

long EItemMetaProperty::getTag()
{
    return (long)this;
}
