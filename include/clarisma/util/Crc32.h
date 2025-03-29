// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once
#include <cstdint>
#include <stddef.h>
//#include <boost/crc.hpp>
#include <array>

static constexpr auto generate_crc32_table(uint32_t polynomial = 0xEDB88320) {
  std::array<uint32_t, 256> table{};
  for (uint32_t ii = 0; ii < 256; ++ii) {
    uint32_t c = ii;
    for (size_t jj = 0; jj < 8; ++jj) {
      c = (c & 1) ? polynomial ^ (c >> 1) : c >> 1;
    }
    table[ii] = c;
  }
  return table;
}

namespace clarisma {

class Crc32
{
public:
    void update(const void* data, size_t size)
    {
        //crc_.process_bytes(data, size);
        newcrc_ = calc_crc32(static_cast<const uint8_t*>(data), size, newcrc_);
    }

    uint32_t get() const noexcept
    {
        //auto oldcrc = crc_.checksum();
        //assert(newcrc_ == oldcrc);
        return newcrc_;
    }

private:
    //boost::crc_32_type crc_;

    uint32_t newcrc_ = 0;

    static constexpr auto crc32_table = generate_crc32_table();

    static constexpr uint32_t calc_crc32(const uint8_t* buffer, size_t length, uint32_t seed = 0) {
      uint32_t c = ~seed;
      for (size_t ii = 0; ii < length; ++ii) {
        c = crc32_table[(c ^ buffer[ii]) & 0xFF] ^ (c >> 8);
      }
      return ~c;
    }
};


} // namespace clarisma
