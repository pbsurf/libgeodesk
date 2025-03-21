// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <cassert>
#ifdef GEODESK_PYTHON
#include <Python.h>
#endif
#include <geodesk/export.h>
#include <geodesk/feature/Key.h>
#include <geodesk/feature/StringTable.h>
#include <geodesk/feature/TagValue.h>
#include <clarisma/compile/unreachable.h>
#include <clarisma/util/DataPtr.h>
#include <clarisma/util/TaggedPtr.h>
#ifdef GEODESK_PYTHON
#include "python/util/util.h"
#endif

// TODO: Migrate Python functionality to PyTags

class PyTagIterator;

namespace geodesk {

/// \cond lowlevel

/*
* Bit 0 : type (0 = number, 1 = string)
* Bit 1 : size flag (0 = narrow, 1 = wide)
* Bit 2 - 15 : unused
* Bits 16 - 31 : narrow value (if size flag = 0)
* Bits 32 - 63 : relative pointer to wide value (if size flag = 1)
*
* 0 = no tag value found
*/
typedef int64_t TagBits;

using geodesk::TagValue;
using clarisma::DataPtr;
using clarisma::TaggedPtr;

enum TagValueType
{
	NARROW_NUMBER = 0,
	GLOBAL_STRING = 1,
	WIDE_NUMBER = 2,
	LOCAL_STRING = 3,
};

class GEODESK_API TagTablePtr final
{
public:
	TagTablePtr(DataPtr p, bool hasLocalTags) :
		TagTablePtr(TaggedPtr<const uint8_t, 1>(p, hasLocalTags)) {}

	static TagTablePtr empty()
	{
		return readFrom(reinterpret_cast<const uint8_t*>(&TagValues::EMPTY_TABLE_STRUCT));
	}

	static TagTablePtr readFrom(DataPtr ppTags)
	{
		return TagTablePtr(TaggedPtr<const uint8_t, 1>(ppTags + ppTags.getInt()));
	}

	uint32_t count() const;
	TagBits getKeyValue(const char* key, size_t len,
		const StringTable& strings) const;
	TagBits getKeyValue(const std::string_view& sv, const StringTable& strings) const
	{
		return getKeyValue(sv.data(), sv.size(), strings);
	}
	TagBits getKeyValue(Key key) const;
	TagBits getGlobalKeyValue(int keyCode) const;
	TagBits getLocalKeyValue(const char* key, size_t len) const;
	bool hasLocalKeys() const
	{
		return taggedPtr_.flags();
	}

	#ifdef GEODESK_PYTHON
	TagBits getKeyValue(PyObject* key, const StringTable& strings) const;
	PyObject* valueAsString(TagBits value, StringTable& strings) const;
	PyObject* valueAsObject(TagBits value, StringTable& strings) const;
	PyObject* valueAsNumber(TagBits value, StringTable& strings) const;
	PyObject* getValue(PyObject* key, StringTable& strings) const;
	#endif

	TagValue tagValue(TagBits value, StringTable& strings) const
	{
		// TODO: if strings were 0/2 instead of 1/3,
		//  we would not need this check
		//  Tag-bits value of 0 would instead translate to global-string #0 (empty string),
		//  which is the default if a key is not found
		//  Or, getGlobalKeyValue() could return `1` to signify "tag not found",
		//  which is less intuitive

		if (value == 0) return {};

		int typeAndSize = (int)value & 3;
		switch(typeAndSize)
		{
		case 0:	// narrow number
			return TagValue((rawNarrowValue(value) << 2) | TagValueType::NARROW_NUMBER);
		case 1:	// global string
			return TagValue(TagValueType::GLOBAL_STRING, globalString(value, strings));
		case 2: // wide number
		{
			DataPtr pValue = valuePtr(value);
			return TagValue((pValue.getUnsignedIntUnaligned() << 2) | TagValueType::WIDE_NUMBER);
		}
		case 3: // local string
		{
			return TagValue(TagValueType::LOCAL_STRING, localString(value));
		}
		default:
			UNREACHABLE_CASE
		}
		// Explicit return to silence warning
		return {};
	}

	DataPtr ptr() const { return taggedPtr_.ptr(); }
	TaggedPtr<const uint8_t, 1> taggedPtr() const { return taggedPtr_; }
	DataPtr alignedBasePtr() const 
	{ 
		return taggedPtr_.raw() & 0xffff'ffff'ffff'fffcULL; 
	}

	int32_t pointerOffset(DataPtr p) const noexcept
	{
		return static_cast<int32_t>(p.ptr() - taggedPtr_.rawPtr());
	}
	// This is always based off the raw tagged pointer (i.e. with the local_key bit,
	// if any), not the actual pointer

private:
	explicit TagTablePtr(TaggedPtr<const uint8_t,1> taggedPtr) : taggedPtr_(taggedPtr) {}

	static int valueType(TagBits value) noexcept
	{
		return static_cast<int>(value) & 3;
	}

	DataPtr valuePtr(TagBits value) const noexcept
	{
		return taggedPtr_.rawPtr() + (value >> 32);
	}

	static uint32_t rawNarrowValue(TagBits value) noexcept
	{
		return static_cast<uint32_t>(value) >> 16;
	}

	// Note: wideNumber / localString rely on tagPtr_, hence cannot be static
	static int32_t narrowNumber(int64_t value) noexcept;
	clarisma::Decimal wideNumber(TagBits value) const noexcept;

	static const clarisma::ShortVarString* globalString(TagBits value, StringTable& strings) noexcept
	{
		assert((value & 3) == TagValueType::GLOBAL_STRING);
		return strings.getGlobalString(rawNarrowValue(value));
	}

	const clarisma::ShortVarString* localString(TagBits value) const noexcept
	{
		assert((value & 3) == TagValueType::LOCAL_STRING);
		DataPtr ppValue = valuePtr(value);
		return reinterpret_cast<const clarisma::ShortVarString*>(ppValue.ptr() + ppValue.getIntUnaligned());
	}

	#ifdef GEODESK_PYTHON
	static PyObject* getGlobalStringObject(TagBits value, StringTable& strings)
	{
		assert((value & 3) == TagValueType::GLOBAL_STRING);
		return strings.getStringObject(static_cast<uint32_t>(value) >> 16);
	}

	PyObject* getLocalStringObject(TagBits value) const
	{
		return Python::toStringObject(*localString(value));
	}
	#endif

	TaggedPtr<const uint8_t, 1> taggedPtr_;

	friend class TagIterator;
	friend class ::PyTagIterator;
	friend class FeatureWriter;
};

// \endcond
} // namespace geodesk
