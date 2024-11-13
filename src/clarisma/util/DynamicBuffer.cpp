// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#include <clarisma/util/Buffer.h>
#include <cstring>
#include <clarisma/util/log.h>

namespace clarisma {

DynamicBuffer::DynamicBuffer(size_t initialCapacity)
{
	buf_ = new char[initialCapacity];
	p_ = buf_;
	end_ = buf_ + initialCapacity;
}


DynamicBuffer::~DynamicBuffer()
{
    if (buf_)
    {
        // LOG("Freeing DynamicBuffer %p", buf_);
        delete[] buf_;
    }
}


void DynamicBuffer::grow()
{
	size_t newCapacity = (end_ - buf_) * 2;
	size_t len = length();
	char* newBuf = new char[newCapacity];
	memcpy(newBuf, buf_, len);
	p_ += newBuf - buf_;
	buf_ = newBuf;
	end_ = newBuf + newCapacity;
}

void DynamicBuffer::filled(char* p)
{
    assert(p >= buf_);
    assert(p <= end_);
    p_ = p;
	char* oldBuf = buf_;
	grow();
	delete[] oldBuf;
}

void DynamicBuffer::flush(char* p)
{
    assert(p >= buf_);
    assert(p <= end_);
    p_ = p;
}


ByteBlock DynamicBuffer::takeBytes()
{
    ByteBlock block(reinterpret_cast<uint8_t*>(buf_), length());
    p_ = nullptr;
    buf_ = nullptr;
    end_ = nullptr;
    return block;
}

} // namespace clarisma
