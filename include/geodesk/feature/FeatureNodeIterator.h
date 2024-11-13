// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <geodesk/feature/WayPtr.h>

namespace geodesk {

/// \cond lowlevel
///
class FeatureNodeIterator
{
public:
    explicit FeatureNodeIterator(FeatureStore* store);
    FeatureStore* store() const { return store_; }
    void start(DataPtr pBody, int flags, const MatcherHolder* matcher, const Filter* filter);
    NodePtr next();

private:
    FeatureStore* store_;
    const MatcherHolder* matcher_;
    const Filter* filter_;
    Tip currentTip_;
    int32_t currentNode_;
    DataPtr p_;
    DataPtr pForeignTile_;
};

// \endcond
} // namespace geodesk
