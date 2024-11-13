// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <unordered_set>
#include <geodesk/feature/FeatureStore.h>
#include <geodesk/feature/Tip.h>
#include <geodesk/geom/Box.h>
#include <geodesk/geom/Tile.h>

namespace geodesk {

class Filter;

/// \cond lowlevel

class TileIndexWalker
{
public:
    TileIndexWalker(DataPtr pIndex, uint32_t zoomLevels,
        const Box& box, const Filter* filter);

    bool next();
    Tip currentTip() const { return Tip(currentTip_); }
    Tile currentTile() const { return currentTile_; }
    uint32_t northwestFlags() const { return northwestFlags_; }
    uint32_t turboFlags() const { return turboFlags_; }
    const Box& bounds() const { return box_; }

private:
    static const int MAX_LEVELS = 13;   // currently 0 - 12
        // TODO: GOL 2.0 has max 8 levels

    // TODO: uint16 supports max level 15 (not 16, because of sign;
    //  we start columns at -1)
	struct Level
	{
		uint64_t childTileMask;
        int32_t pChildEntries;
        Tile topLeftChildTile;
        uint16_t step;       // difference in zoom level to parent
        int16_t startCol;
        int16_t endCol;
        int16_t endRow;
        int16_t currentCol;
        int16_t currentRow;
        uint32_t turboFlags;
	};

    void startLevel(Level* level, int tip);
    void startRoot();
    
    Box box_;
    const Filter* filter_;
    DataPtr pIndex_;
    int currentLevel_;
    Tile currentTile_;
    uint32_t currentTip_;
    uint32_t northwestFlags_;
    uint32_t turboFlags_;
    bool tileBasedAcceleration_;
    bool trackAcceptedTiles_;
    std::unordered_set<Tile> acceptedTiles_;
    Level levels_[MAX_LEVELS];
};

// \endcond
} // namespace geodesk
