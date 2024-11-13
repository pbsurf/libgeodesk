// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#include <geodesk/filter/Filter.h>
#include <geodesk/filter/SpatialFilter.h>

namespace geodesk {


Box Filter::getBounds() const
{
    return (flags_ & FilterFlags::USES_BBOX) ?
        reinterpret_cast<const SpatialFilter*>(this)->bounds() : Box::ofWorld();
}





} // namespace geodesk
