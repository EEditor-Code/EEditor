// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 北京格瑞特皮软件科技有限公司

#include "ESmartPtr.h"


Referenced::Referenced() : _refCount(0) {}

Referenced::Referenced(const Referenced&) : _refCount(0) {}

Referenced& Referenced::operator=(const Referenced&) {
    return *this;
}

void Referenced::ref() const {
    ++_refCount;
}

void Referenced::unref() const {
    if (--_refCount == 0) {
        delete this;
    }
}

int Referenced::getRefCount() const {
    return _refCount;
}

Referenced::~Referenced() = default;
    
