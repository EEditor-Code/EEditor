// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#include "EItemMetaFunction.h"
#include "DefineCommon.h"
#include "EItemMetaProperty.h"
#include <cstdint>

struct EItemMetaFunctionData
{
    EItemMetaPropertyPtr m_valueReturn;
    EList<EItemMetaPropertyPtr> m_propertys;
    intptr_t m_owner;
};

EItemMetaFunction::EItemMetaFunction()
{
    this->m_data = new EItemMetaFunctionData;
    this->m_data->m_owner = NULL;
    // this->m_data->m_valueReturn = NewSmartPtr(EItemMetaProperty)();
    this->m_data->m_valueReturn = new EItemMetaProperty();
    this->m_data->m_valueReturn->typeName = "void";
    
}

EItemMetaFunction::~EItemMetaFunction()
{
    delete m_data;
}

void EItemMetaFunction::addProperty(const EItemMetaPropertyPtr& p)
{
    m_data->m_propertys.push_back(p);
}

EItemMetaPropertyPtr EItemMetaFunction::getProperty(int index)
{
    EItemMetaPropertyPtr result = nullptr;
    if(index >=0 && index <m_data->m_propertys.size())
    {
        result = m_data->m_propertys[index];
    }
    return result;
}

size_t EItemMetaFunction::getPropertyCount()
{
    return m_data->m_propertys.size();
}


void EItemMetaFunction::setOwner(intptr_t owner)
{
    this->m_data->m_owner = owner;
}

intptr_t EItemMetaFunction::getOwner()
{
    return this->m_data->m_owner;
}
    
void EItemMetaFunction::setFunType(FunType type)
{
    this->m_funType = type;
}

EItemMetaFunction::FunType EItemMetaFunction::getFunType()
{
    return this->m_funType;
}

void EItemMetaFunction::setName(const EString& name)
{
    this->m_name = name;
}

EString EItemMetaFunction::getName()
{
    return this->m_name;
}

void EItemMetaFunction::setGroupName(const EString& name)
{
    this->m_groupName = name;
}

EString EItemMetaFunction::getGroupName()
{
    return this->m_groupName;
}

void EItemMetaFunction::setIndex(int index)
{
    this->m_index = index;
}

int EItemMetaFunction::getIndex()
{
    return this->m_index;
}

void EItemMetaFunction::setRegOrder(int order)
{
    this->m_regOrder = order;
}

int EItemMetaFunction::getRegOrder()
{
    return this->m_regOrder;
}

void EItemMetaFunction::setReturnProperty(const EItemMetaPropertyPtr& p)
{
    this->m_data->m_valueReturn = p;

}

EItemMetaPropertyPtr EItemMetaFunction::getReturnProperty()
{
    return this->m_data->m_valueReturn;
}

void EItemMetaFunction::call(EVariant* result, EList<EVariant*> vList)
{
    if(vList.size() == 0)
    {
        this->call(result);
    }
    else if(vList.size() == 1)
    {
        this->call(result, vList[0]);
    }
    else if(vList.size() == 2)
    {
        this->call(result, vList[0], vList[1]);
    }
    else if(vList.size() == 3)
    {
        this->call(result, vList[0], vList[1], vList[2]);
    }
    else if(vList.size() == 4)
    {
        this->call(result, vList[0], vList[1], vList[2], vList[3]);
    }
    else if(vList.size() == 5)
    {
        this->call(result, vList[0], vList[1], vList[2], vList[3], vList[4]);
    }
}

void EItemMetaFunction::call( EVariant* result)
{
    this->callInternal((intptr_t)nullptr, result);
}

void EItemMetaFunction::call( EVariant* result, EVariant* v1)
{
    this->callInternal((intptr_t)nullptr, result, v1);
}

void EItemMetaFunction::call( EVariant* result, EVariant* v1, EVariant* v2)
{
    this->callInternal((intptr_t)nullptr, result, v1, v2);
}

void EItemMetaFunction::call( EVariant* result, EVariant* v1, EVariant* v2, EVariant* v3)
{
    this->callInternal((intptr_t)nullptr, result, v1, v2, v3);
}

void EItemMetaFunction::call( EVariant* result, EVariant* v1, EVariant* v2, EVariant* v3, EVariant* v4)
{
    this->callInternal((intptr_t)nullptr, result, v1, v2, v3, v4);
}

void EItemMetaFunction::call( EVariant* result, EVariant* v1, EVariant* v2, EVariant* v3, EVariant* v4, EVariant* v5)
{
    this->callInternal((intptr_t)nullptr, result, v1, v2, v3, v4, v5);
}

long EItemMetaFunction::getTag()
{
    return (long)this;
}

void EItemMetaFunction::callInternal( intptr_t owner, ... )
{
    // ELOG_DEBUG("attention----------");
}
