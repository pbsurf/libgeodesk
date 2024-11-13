// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#include <geodesk/filter/Filters.h>
#include <geodesk/feature/FeatureBase.h>
#include <geodesk/feature/QueryException.h>
#include <geodesk/filter/CrossesFilter.h>
#include <geodesk/filter/IntersectsFilter.h>
#include <geodesk/filter/PointDistanceFilter.h>
#include <geodesk/filter/WithinFilter.h>

namespace geodesk {

const Filter* filter(PreparedFilterFactory&& factory, Feature feature)
{
    const Filter* filter;
    if(feature.isAnonymousNode())
    {
        filter = factory.forCoordinate(feature.xy());
    }
    else
    {
        filter = factory.forFeature(feature.store(), feature.ptr());
    }
    if(filter == nullptr)
    {
        throw QueryException("Filter not implemented");
    }
    return filter;
}


const Filter* Filters::intersects(Feature feature)
{
    return filter(IntersectsFilterFactory(), feature);
}

const Filter* Filters::within(Feature feature)
{
    return filter(WithinFilterFactory(), feature);
}

const Filter* Filters::containsPoint(Coordinate xy)
{
    return new ContainsPointFilter(xy);
}

const Filter* Filters::crossing(Feature feature)
{
    return filter(CrossesFilterFactory(), feature);
}

const Filter* Filters::maxMetersFrom(double meters, Coordinate xy)
{
    return new PointDistanceFilter(meters, xy);
}

} // namespace geodesk
