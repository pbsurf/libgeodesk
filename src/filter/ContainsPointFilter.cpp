// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#include <geodesk/filter/ContainsPointFilter.h>
#include <geodesk/geom/polygon/PointInPolygon.h>

namespace geodesk {

bool ContainsPointFilter::accept(FeatureStore* store, FeaturePtr feature, FastFilterHint /* ignored */) const
{
	if (feature.isArea())
	{
		if (feature.isWay())
		{
			PointInPolygon tester(point_);
			// point is on boundary or inside (odd number of crossings)
			return tester.testAgainstWay(WayPtr(feature)) || tester.isInside();
		}
		else
		{
			PointInPolygon tester(point_);
			// point is on boundary or inside (odd number of crossings)
			return tester.testAgainstRelation(store, RelationPtr(feature)) || tester.isInside();
		}
	}
	else if (feature.isWay())
	{
		WayPtr way(feature);
		WayCoordinateIterator iter(way);
		Coordinate start = iter.next();
		for (;;)
		{
			Coordinate end = iter.next();
			if (end.isNull()) break;
			if (LineSegment::orientation(start, end, point_) == 0)
			{
				// test point lies on line segment, which means
				// the linear way "contains" the point
				return true;
			}
			start = end;
		}
		return false;
	}
	else if (feature.isNode())
	{
		// A node contains another node only if their coordinates are the same
		NodePtr node(feature);
		return node.xy() == point_;
	}
	// TODO: non-area relations
	return false;
}

} // namespace geodesk
