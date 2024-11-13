// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#ifdef GEODESK_WITH_GEOS
#include <geos_c.h>
#endif
#include <geodesk/geom/index/MonotoneChain.h>
#include <geodesk/geom/index/MCIndex.h>
#include <geodesk/feature/WayPtr.h>
#include <geodesk/feature/RelationPtr.h>
#include <clarisma/alloc/Arena.h>

namespace geodesk {

class MCIndexBuilder
{
public:
	MCIndexBuilder();
	void addLineSegment(Coordinate start, Coordinate end);
	void segmentizeWay(WayPtr way);
	#ifdef GEODESK_WITH_GEOS
	void segmentizeCoords(GEOSContextHandle_t context, const GEOSCoordSequence* coords);
	void segmentizePolygon(GEOSContextHandle_t context, const GEOSGeometry* polygon);
	#endif
	void segmentizeAreaRelation(FeatureStore* store, RelationPtr rel);
	void segmentizeMembers(FeatureStore* store, RelationPtr rel, RecursionGuard& guard);
	MCIndex build(Box bounds);
	static MCIndex buildFromAreaRelation(FeatureStore* store, RelationPtr rel)
	{
		MCIndexBuilder builder;
		builder.segmentizeAreaRelation(store, rel);
		return builder.build(rel.bounds());
	}

private:
	// static const size_t CHUNK_SIZE = 32 * 1024;
	static const int MAX_VERTEX_COUNT = 256;

	class MCHolder 
	{
	public:
		static size_t storageSize(int vertexCount)
		{
			return sizeof(MCHolder) - sizeof(MonotoneChain) + 
				MonotoneChain::storageSize(vertexCount);
		}

		const MCHolder* next;
		uint32_t padding;     // explicit padding (MC always has an odd number of ints)
		MonotoneChain chain;
	};

	size_t chainCount_;
	size_t totalChainSize_;
	const MCHolder* first_;
	clarisma::Arena arena_;
};

} // namespace geodesk
