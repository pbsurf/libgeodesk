// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once
#include <geodesk/geom/index/RTree.h>

namespace geodesk {

class MonotoneChain;

// TODO: Idea
//  If a MC has only two vertexes, we don't need to store it -- its bbox is
//  its representation; we only need to mark whether it runs east or west
//  Could tag the item pointer
//  Bit 0 = 1: last item
//  Bit 1 = 1: item is a segment
//  If bit 1 = 1: Bit 2 indicates east/west
//  saves 20 bytes per short mc

/*
class CrossingCount
{
public:
	CrossingCount() : crossings_(0) {}

	bool isOdd() { return crossings_ & 2; }
		// we use 2 instead of 1 to differentiate full crossing vs. half-crossing

	CrossingCount& operator+=(int value) 
	{
		crossings_ += value;
		return *this;
	}

private:
	uint32_t crossings_;
};
*/

// TODO: to simplify checks, we could add a flag that indicates whether
// points on polygon boundary are considered "inside"
// setOnBoundary will then set crossings_ to boundaryIsConsideredInside_
class PointLocation
{
public:
	PointLocation() : crossings_(0), onBoundary_(false) {}

	bool isInside() const { return crossings_ & 1; }
	bool isOnBoundary() const { return onBoundary_; }

	/**
	 * -1 = outside
	 *  0 = on boundary
	 *  1 = inside
	 * 
	 * TODO: just count +2 for crossings? Saves multiplication and
	 *  also stays consistent with PointInPolygon class
	 */
	int location() const
	{
		return onBoundary_ ? 0 : ((crossings_ & 1) * 2 - 1);
	}
	void addCrossing() 
	{ 
		assert(!onBoundary_); 
		crossings_++;	
	}
	void setOnBoundary() 
	{ 
		onBoundary_ = true; 
		crossings_ = 0; 
	}

	

private:
	uint32_t crossings_;
	bool onBoundary_;
};

class MCIndex
{
public:
	MCIndex() : data_(nullptr) {}
	~MCIndex()
	{
		if (data_) delete[] data_;
	}

	MCIndex(const uint8_t* data, RTree<const MonotoneChain>&& index) : 
		data_(data), index_(std::move(index))
	{
		assert(data);
	}

	// Disable copy semantics as MCIndex owns resources
	MCIndex(const MCIndex& other) = delete;
	MCIndex& operator=(const MCIndex& other) = delete;
	MCIndex(MCIndex&& other) noexcept : 
		data_(other.data_), 
		index_(std::move(other.index_))
	{
		other.data_ = nullptr; // Prevent other from deallocating the memory
	}

	MCIndex& operator=(MCIndex&& other) noexcept
	{
		index_ = std::move(other.index_);
		if (this != &other && data_) delete data_; // Release currently owned memory (if any)
		data_ = other.data_;
		other.data_ = nullptr;
		return *this;
	}

	bool properlyContainsPoint(Coordinate c) const;
	bool containsPoint(Coordinate c) const;	
	bool pointOnBoundary(Coordinate c) const;
	bool intersects(const MonotoneChain* mc) const;

	/**
	 * Tests whether the boundary of the given box interacts with any
	 * of the monotone chains in the index.
	 */
	bool intersectsBoxBoundary(const Box& box) const
	{
		return index_.search(box, intersectsBoxBoundary, &box);
	}

	/**
	 * Tests where the given Box is located in respect to the indexed polygon.
	 * 
	 * @returns  -1 = Box lies fully outside
	 *            0 = Box boundary intersects with the polygon, or Box
	 *                contains the polygon
	 *            1 = Box lies fully inside the polygon
	 */
	int locateBox(const Box& box) const;

	/**
	 * A shortcut test that only considers the bounding boxes of monotone 
	 * chain.
	 *
	 * @returns  -1 = Box definitely lies fully outside 
	 *            0 = Box *may* interact with the monotone chains (bboxes intersect);
	 *                a full test is needed
	 *            1 = Box definitely lies fully inside the polygon
	 */
	int maybeLocateBox(const Box& box) const;

	// bool intersectsLineSegment(Coordinate start, Coordinate end) const;
	
	// -1 outside, 0 = boundary, 1 = inside
	int locatePoint(Coordinate c) const
	{
		PointLocationClosure closure(c);
		index_.search(
			Box(c.x, c.y, std::numeric_limits<int32_t>::max(), c.y),
			countCrossings, &closure);
		return closure.location.location();
	}

	// static void nextWayChain(Coordinate start, WayCoordinateIterator& iter, MonotoneChain* mc, int maxVertexes);

	template <typename QT>
	bool findChains(const Box& box, RTree<const MonotoneChain>::SearchFunction<QT> func, QT closure) const
	{
		return index_.search(box, func, closure);
	}
	
private:
	struct PointLocationClosure
	{
		PointLocationClosure(Coordinate pt) : point(pt) {}

		Coordinate point;
		PointLocation location;
	};

	static bool intersectsChain(const RTree<const MonotoneChain>::Node* node,
		const MonotoneChain* candidate);
	static bool intersectsBoxBoundary(const RTree<const MonotoneChain>::Node* node,
		const Box* bounds);
	static bool maybeIntersectsBoxBounds(const RTree<const MonotoneChain>::Node* node,
		const Box* bounds);

	/*
	static bool intersectsLineSegment(const RTree<const MonotoneChain>::Node* node,
		const Box* bounds);
	*/
	static bool countCrossings(const RTree<const MonotoneChain>::Node* node,
		PointLocationClosure* closure);

	RTree<const MonotoneChain> index_;
	const uint8_t* data_;
};



} // namespace geodesk
