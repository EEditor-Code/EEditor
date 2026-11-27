// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#include "ESmartPtr.h"


Referenced::Referenced() : _refCount(0) {}

Referenced::Referenced(const Referenced&) : _refCount(0) {}

Referenced& Referenced::operator=(const Referenced&) {
    return *this;
}

void Referenced::ref() const {
    _refCount.fetch_add(1, std::memory_order_relaxed);
}

void Referenced::unref() const {
    // acq_rel: 最后一个引用释放前的写操作对析构线程可见
    if (_refCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
        delete this;
    }
}

int Referenced::getRefCount() const {
    return _refCount.load(std::memory_order_relaxed);
}

Referenced::~Referenced() = default;
    
