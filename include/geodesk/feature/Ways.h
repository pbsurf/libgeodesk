// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include "FeaturesBase.h"

namespace geodesk {

class Ways : public FeaturesBase<Way>
{
public:
	using FeaturesBase::FeaturesBase;
	Ways(const FeaturesBase<Feature>& other) :
		FeaturesBase(View(other.view_& FeatureTypes::WAYS)) {}
	Ways(const FeaturesBase<Node>& other) : FeaturesBase(empty()) {}
	Ways(const FeaturesBase<Way>& other) : FeaturesBase(other.view_) {}
	Ways(const FeaturesBase<Relation>& other) : FeaturesBase(empty()) {}

	template<typename P>
	Ways parentsOf(FeatureBase<P> feature) const;
};

} // namespace geodesk
