// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#include <geodesk/filter/CrossesFilter.h>
#include <geodesk/feature/FastMemberIterator.h>

namespace geodesk {


int CrossesFilter::acceptTile(Tile tile) const
{
	return index_.locateBox(tile.bounds()) == 0 ? 0 : -1;
}

bool CrossesFilter::acceptWay(WayPtr way) const
{
	// If the way's bounding box does not intersect any MC bboxes,
	// then it is impossible for its geometry to cross 
	if (!index_.intersectsBoxBoundary(way.bounds())) return false;
	return anySegmentsCross(way);
}

bool CrossesFilter::acceptNode(NodePtr node) const
{
	return false;
}

bool CrossesFilter::acceptAreaRelation(FeatureStore* store, RelationPtr relation) const
{
	return acceptMembers(store, relation, nullptr);
}


bool CrossesFilter::accept(FeatureStore* store, FeaturePtr feature, FastFilterHint fast) const
{
	// "crosses" has no turbo-mode; only tiles that interact with segments
	// are examined, all others are rejected
	return acceptFeature(store, feature);
}


} // namespace geodesk
