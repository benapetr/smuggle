//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2015

#include "collectable.hpp"
#include "collectable_smartptr.hpp"
#include "apiquery.hpp"
#include "exception.hpp"
#include "gc.hpp"

using namespace Smuggle;

template <class T>
Collectable_SmartPtr<T>::Collectable_SmartPtr()
{
    this->ptr = NULL;
}

template <class T>
Collectable_SmartPtr<T>::Collectable_SmartPtr(Collectable_SmartPtr *smart_ptr)
{
    // we must not just copy the bare pointer but also increment the reference count
    if (smart_ptr->ptr)
        smart_ptr->ptr->IncRef();
    this->ptr = smart_ptr->ptr;
}

template <class T>
Collectable_SmartPtr<T>::Collectable_SmartPtr(const Collectable_SmartPtr &sp_)
{
    if (sp_.ptr)
        sp_.ptr->IncRef();
    this->ptr = sp_.ptr;
}

template <class T>
Collectable_SmartPtr<T>::Collectable_SmartPtr(T *pt)
{
    if (!pt)
    {
        this->ptr = NULL;
        return;
    }
    this->ptr = pt;
    this->ptr->IncRef();
}

template <class T>
Collectable_SmartPtr<T>::~Collectable_SmartPtr()
{
    this->FreeAcqRsrPtr();
}

template <class T>
void Collectable_SmartPtr<T>::SetPtr(T *pt)
{
    this->FreeAcqRsrPtr();
    if (pt)
    {
        pt->IncRef();
        this->ptr = pt;
    }
}

template <class T>
T *Collectable_SmartPtr<T>::operator->()
{
    return this->ptr;
}

template <class T>
T *Collectable_SmartPtr<T>::GetPtr() const
{
    return this->ptr;
}

template <class T>
Collectable_SmartPtr<T>::operator void *() const
{
    return this->ptr;
}

template <class T>
Collectable_SmartPtr<T>::operator T*() const
{
    return this->ptr;
}

template <class T>
void Collectable_SmartPtr<T>::FreeAcqRsrPtr()
{
    GC_DECREF(this->ptr);
}

namespace Smuggle
{
    template class Collectable_SmartPtr<Query>;
    template class Collectable_SmartPtr<ApiQuery>;
}
