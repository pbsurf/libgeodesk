// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once
#include <random>
#include <array>
#include <cstdint>

namespace clarisma {

using std::byte;

class UUID
{
public:
    explicit UUID(const uint8_t* bytes)
    {
        memcpy(guid_, bytes, 16);
    }

    explicit UUID(const UUID& other)
    {
        memcpy(guid_, other.guid_, 16);
    }

    static UUID create()
    {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);
        uint8_t guid[16];

        // Generate two 64-bit random numbers
        uint64_t r1 = dis(gen);
        uint64_t r2 = dis(gen);

        // Fill the first 8 bytes with r1
        for (int i = 0; i < 8; ++i) 
        {
            guid[i] = static_cast<uint8_t>(r1 >> (i * 8));
        }

        // Fill the last 8 bytes with r2
        for (int i = 0; i < 8; ++i) 
        {
            guid[i + 8] = static_cast<uint8_t>(r2 >> (i * 8));
        }

        // Set the version (4) and variant (RFC 4122)
        guid[6] = (guid[6] & 0x0F) | 0x40;  // version 4
        guid[8] = (guid[8] & 0x3F) | 0x80;  // variant 1
        return UUID(guid);
    }

private:
    uint8_t guid_[16];
};


} // namespace clarisma
