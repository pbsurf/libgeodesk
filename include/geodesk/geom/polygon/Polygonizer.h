// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <geodesk/feature/WayPtr.h>
#include <geodesk/feature/RelationPtr.h>
#include <clarisma/alloc/Arena.h>
#ifdef GEODESK_WITH_GEOS
#include <geos_c.h>
#endif

namespace geodesk {

// TODO: Use a borrowed arena?
// No, needs to be reset after each polygon is built, or else arena will
// just keep growing

class Polygonizer
{
public:
    class Ring;

    Polygonizer();
    const Ring* outerRings() const { return outerRings_; };
    const Ring* innerRings() const { return innerRings_; };

    /**
     * Creates only the raw outer and inner rings, without assigning
     * the inner rings to outer, and without merging inner rings 
     * whose edges touch. (This is sufficient for area calculation)
     */
    void createRings(FeatureStore* store, RelationPtr relation);

    /**
     * Assigns inner rings to outer, and merges any inner rings whose
     * edges touch. The createRings() method must have been called.
     */
    void assignAndMergeHoles();
    #ifdef GEODESK_WITH_GEOS
    GEOSGeometry* createPolygonal(GEOSContextHandle_t context);
    #endif

private:
    class Segment;
    class RingBuilder;
    class RingAssigner;
    class RingMerger;

    Segment* createSegment(WayPtr way, Segment* next);
    Ring* buildRings(int segmentCount, Segment* firstSegment);

    static Ring* createRing(int vertexCount, Segment* firstSegment, 
        Ring* next, clarisma::Arena& arena);
    
    clarisma::Arena arena_;
    Ring* outerRings_;
    Ring* innerRings_;

    friend class RingCoordinateIterator;
};



} // namespace geodesk
