// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <geodesk/feature/FeatureBase.h>
#include <geodesk/feature/Features.h>
#include <geodesk/feature/Nodes.h>

// \cond

namespace geodesk {

// TODO: Add type constraints to nodes/members

template<typename T>
Nodes FeatureBase<T>::nodes() const
{
    return nodes(nullptr);
}

template<typename T>
Nodes FeatureBase<T>::nodes(const char* query) const
{
    if(isWay())
    {
        return Nodes(View::nodesOf(store(), ptr(), query));
    }
    return Nodes::empty(store());
}

template<typename T>
Features FeatureBase<T>::members() const
{
    return members(nullptr);
}

template<typename T>
Features FeatureBase<T>::members(const char* query) const
{
    if(isRelation())
    {
        return Features(View::membersOf(store(), ptr(), query));
    }
    if(isWay())
    {
        return Features(View::nodesOf(store(), ptr(), query));
    }
    return Features::empty(store());
}

} // namespace geodesk

// \endcond