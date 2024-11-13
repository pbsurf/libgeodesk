// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#include <geodesk/feature/FeatureBase.h>

namespace geodesk {

Tags::Tags(const Feature& feature) :
    Tags(feature.store(), feature.ptr())
{
}

bool Tags::operator==(const Tags& other) const
{
    // TODO
    return false;
}


} // namespace geodesk