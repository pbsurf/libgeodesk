// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <cstdint>
#include <iostream>
#include <type_traits>

namespace clarisma {

template <typename T, int N>
class TaggedPtr
{
    static_assert(N > 0 && N < 4, "Flags bits must be 1,2 or 3");
    /*
    static_assert(alignof(T) >= (1 << N),
        "Alignment of pointer does not support the desired number of flag bits");
    // fails for TagTablePtr 
    */


public:
    // Constructor
    TaggedPtr(T* ptr = nullptr, uintptr_t flags = 0) noexcept :
        data_(reinterpret_cast<uintptr_t>(ptr) | (flags & flagMask()))
    {
    }

    // Get the pointer without flags
    T* ptr() const noexcept
    {
        return reinterpret_cast<T*>(data_ & ~flagMask());
    }

    // Get the flags
    int flags() const noexcept
    {
        return static_cast<int>(data_ & flagMask());
    }

    // Implicit conversion to a regular pointer
    operator T* () const noexcept
    {
        return ptr();
    }

    // Get the pointer, including flags
    T* rawPtr() const noexcept
    {
        return reinterpret_cast<T*>(data_);
    }

    uintptr_t raw() const noexcept { return data_; }

    // Set the flags
    void setFlags(uintptr_t flags) noexcept
    {
        data_ = reinterpret_cast<uintptr_t>(ptr()) | (flags & flagMask());
    }

private:
    uintptr_t data_;

    // Calculate the flag mask
    static constexpr uintptr_t flagMask() noexcept
    {
        return (1UL << N) - 1;
    }
};


} // namespace clarisma
