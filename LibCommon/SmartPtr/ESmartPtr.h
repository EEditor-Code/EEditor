// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#ifndef SMART_PTR_H
#define SMART_PTR_H
#include <DefineLibCommon.h>
#include <type_traits>

class COMMON_DLL Referenced 
{
public:
    // Constructor, initial reference count is 0
    Referenced();
    
    // Copy constructor, reference count of new object is 0
    Referenced(const Referenced&);
    
    // Assignment operator, does not change reference count
    Referenced& operator=(const Referenced&);
    
    // Increase reference count
    void ref() const;
    
    // Decrease reference count, delete object when count reaches 0
    void unref() const;
    
    // Get current reference count
    int getRefCount() const;

protected:
    // Virtual destructor to ensure proper destruction of subclasses
    virtual ~Referenced();

private:
    mutable int _refCount; // Reference count, mutable allows modification in const member functions
};

template <typename T>
class ESmartPtr 
{
public:
    // Constructor: accepts pointer of type T*
    ESmartPtr(T* ptr = nullptr);
    
    // Copy constructor
    ESmartPtr(const ESmartPtr& rhs);
    
    // Template conversion constructor: allows ESmartPtr<Derived> to convert to ESmartPtr<Base>
    // This enables implicit conversion from derived class smart pointers to base class smart pointers
    template <typename U>
    ESmartPtr(const ESmartPtr<U>& rhs, 
              typename std::enable_if<std::is_convertible<U*, T*>::value>::type* = nullptr)
        : _ptr(rhs.get()) 
    {
        if (_ptr) _ptr->ref();
    }
    
    // Private constructor, used for dynamicCast, does not increase reference count
    ESmartPtr(T* ptr, bool addRef);
    
    // Destructor
    ~ESmartPtr();
    
    // New: Assignment operator, accepts raw pointer
    ESmartPtr& operator=(T* ptr);
    
    // Assignment operator
    ESmartPtr& operator=(const ESmartPtr& rhs);
    
    // Template assignment operator: allows assignment from ESmartPtr<Derived> to ESmartPtr<Base>
    template <typename U>
    typename std::enable_if<std::is_convertible<U*, T*>::value, ESmartPtr&>::type
    operator=(const ESmartPtr<U>& rhs) 
    {
        if (_ptr != rhs.get()) {
            if (rhs.get()) rhs.get()->ref();
            if (_ptr) _ptr->unref();
            _ptr = rhs.get();
        }
        return *this;
    }
    
    // Pointer access operator
    T* operator->() const;
    
    // Dereference operator
    T& operator*() const;
    
    // Conversion to bool type, used to check if pointer is valid
    operator bool() const;
    
    // Comparison operators for ref_ptr.
    bool operator == (const ESmartPtr& rp) const { return (_ptr==rp._ptr); }
    bool operator == (const T* ptr) const { return (_ptr==ptr); }

    bool operator != (const ESmartPtr& rp) const { return (_ptr!=rp._ptr); }
    bool operator != (const T* ptr) const { return (_ptr!=ptr); }
    
    // Get raw pointer
    T* get() const;
    
    // Swap contents of two smart pointers
    void swap(ESmartPtr& rhs);
    
    // Dynamic cast from base class pointer to derived class pointer
    template <typename Derived>
    static ESmartPtr<Derived> dynamicCast(const ESmartPtr& base);

private:
    
    T* _ptr; // Pointer to the actual object
};

// Implementations of template class member functions (template classes usually need to be implemented in headers)
template <typename T>
ESmartPtr<T>::ESmartPtr(T* ptr) : _ptr(ptr) {
    if (_ptr) _ptr->ref();
}

template <typename T>
ESmartPtr<T>::ESmartPtr(const ESmartPtr& rhs) : _ptr(rhs._ptr) {
    if (_ptr) _ptr->ref();
}

template <typename T>
ESmartPtr<T>::~ESmartPtr() {
    if (_ptr) _ptr->unref();
}

template <typename T>
ESmartPtr<T>& ESmartPtr<T>::operator=(T* ptr) 
{
    // Avoid self-assignment
    if (_ptr != ptr) {
        // Process new pointer first: increase reference count if not null
        if (ptr) ptr->ref();
        // Process old pointer: decrease reference count
        if (_ptr) _ptr->unref();
        // Point to new object
        _ptr = ptr;
    }
    return *this;
}

template <typename T>
ESmartPtr<T>& ESmartPtr<T>::operator=(const ESmartPtr& rhs) {
    if (_ptr != rhs._ptr) {
        // First increase reference count of rhs to prevent premature destruction during self-assignment
        if (rhs._ptr) rhs._ptr->ref();
        // Decrease reference count of current pointer
        if (_ptr) _ptr->unref();
        // Point to new object
        _ptr = rhs._ptr;
    }
    return *this;
}

template <typename T>
T* ESmartPtr<T>::operator->() const {
    return _ptr;
}

template <typename T>
T& ESmartPtr<T>::operator*() const {
    return *_ptr;
}

template <typename T>
ESmartPtr<T>::operator bool() const {
    return _ptr != nullptr;
}

template <typename T>
T* ESmartPtr<T>::get() const {
    return _ptr;
}

template <typename T>
void ESmartPtr<T>::swap(ESmartPtr& rhs) {
    T* temp = _ptr;
    _ptr = rhs._ptr;
    rhs._ptr = temp;
}

template <typename T>
template <typename Derived>
ESmartPtr<Derived> ESmartPtr<T>::dynamicCast(const ESmartPtr& base) {
    Derived* derived = dynamic_cast<Derived*>(base.get());
    return ESmartPtr<Derived>(derived, true); // Share ownership: add a reference
}

template <typename T>
ESmartPtr<T>::ESmartPtr(T* ptr, bool addRef) : _ptr(ptr) {
    if (_ptr && addRef) _ptr->ref();
}

#endif // SMART_PTR_H
