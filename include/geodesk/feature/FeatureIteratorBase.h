// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <geodesk/feature/FeatureBase.h>
#include <geodesk/feature/MemberIterator.h>
#include <geodesk/feature/FeatureNodeIterator.h>
#include <geodesk/feature/WayCoordinateIterator.h>
#include <geodesk/filter/FeatureNodeFilter.h>
#include <geodesk/filter/WayNodeFilter.h>
#include <geodesk/query/Query.h>

namespace geodesk {

/// \cond

class GEODESK_API FeatureIteratorBase
{
public:
    explicit FeatureIteratorBase(const View& view);
    ~FeatureIteratorBase();

protected:
	const Feature& currentFeature() const { return current_; }
	void fetchNext();

private:
	void initNodeIterator(const View& view);

	uint_fast8_t type_;
    Feature current_;
	union Storage
	{
		Query worldQuery;
		MemberIterator members;
		struct
		{
			WayCoordinateIterator coords;
			FeatureNodeIterator featureNodes;
			NodePtr nextFeatureNode;
		} nodes;

	    // Default constructor
	    Storage() {}
	    // Destructor does nothing as we handle destruction manually
	    ~Storage() {}
	}
	storage_;

	union ParentWaysFilters
	{
		FeatureNodeFilter featureNodes;
		WayNodeFilter anonNodes;

		ParentWaysFilters() {}
		~ParentWaysFilters() {}
	}
	filter_;
};


// \endcond

} // namespace geodesk


// Possible iterators:
// - empty -> nothing
// - world view -> Node,Way,Relation
// - purely coordinate nodes -> AnonymousNode
// - purely feature nodes -> Node
// - mix of feature nodes and coordinate nodes -> Node, AnonymousNode
// - members -> Node,Way,Relation
// - parent relations -> Relation
// - parent ways (type of world view) -> Way
// - both parent relations & parent ways -> Way,Relation

