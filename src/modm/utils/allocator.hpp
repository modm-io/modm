/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2011-2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <new>      // needed for placement new

namespace modm::allocator
{

/**
 * Base class for all allocator types
 *
 * @ingroup modm_utils
 * @author  Fabian Greif
 */
template <typename T>
class AllocatorBase
{
public:
    /**
     * Construct an object
     *
     * Constructs an object of type T (the template parameter) on the
     * location pointed by p using its copy constructor to initialize
     * its value to \p value.
     *
     * Notice that this does not allocate space for the element, it
     * should already be available at p.
     */
    static inline void
    construct(T* p, const T& value)
    {
        // placement new
        ::new((void *) p) T(value);
    }

    /**
     * Destroy an object
     *
     * Destroys the object of type T (the template parameter) pointed
     * by p.
     *
     * Notice that this does not deallocate space for the element.
     */
    static inline void
    destroy(T* p)
    {
        p->~T();
    }

protected:
    AllocatorBase()
    {
    }
};

/**
 * Dynamic memory allocator
 *
 * Wrapper for the underlying memory management. No additional
 * management is done.
 *
 * @ingroup modm_utils
 * @author  Fabian Greif
 */
template <typename T>
class Dynamic : public AllocatorBase<T>
{
public:
    template <typename U>
    struct rebind
    {
        typedef Dynamic<U> other;
    };

public:
    Dynamic() :
        AllocatorBase<T>()
    {
    }

    Dynamic(const Dynamic& other) :
        AllocatorBase<T>(other)
    {
    }

    template <typename U>
    Dynamic(const Dynamic<U>&) :
        AllocatorBase<T>()
    {
    }

    T*
    allocate(size_t n)
    {
        // allocate the memory without calling the constructor
        // of the associated data-type.
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void
    deallocate(T* p)
    {
        // it is important to use this form here, otherwise the
        // destructor of p will be called which is unwanted here.
        // The destructor can be called with the destroy()-method.
        ::operator delete(p);
    }
};

}
