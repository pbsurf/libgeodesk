// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <clarisma/util/varint.h>
#include <geodesk/feature/NodePtr.h>

namespace geodesk {

/// \cond lowlevel

class WayPtr : public FeaturePtr
{
public:
	WayPtr() {}
	explicit WayPtr(FeaturePtr f) : FeaturePtr(f.ptr()) { assert(isNull() || isWay()); }
	explicit WayPtr(DataPtr p) : FeaturePtr(p) {}

	// TODO: remove in v2
	bool isPlaceholder() const { return maxY() < minY(); }

	bool hasFeatureNodes() const noexcept
	{
		return flags() & FeatureFlags::WAYNODE;
	}

	uint32_t nodeCount() const noexcept
	{
		const uint8_t* p = bodyptr();
		uint32_t rawCount = clarisma::readVarint32(p);
		return rawCount + isArea();
	}
};

// \endcond



} // namespace geodesk
