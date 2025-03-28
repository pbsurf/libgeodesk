// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <geodesk/geom/Coordinate.h>
#include <geodesk/feature/WayPtr.h>
#include <geodesk/feature/RelationPtr.h>

namespace geodesk {

/// Classes and functions for calculating a feature's centroid
///
/// @ingroup lowlevel
/// \cond lowlevel
///
class GEODESK_API Centroid
{
public:
	static Coordinate ofWay(WayPtr way);
	static Coordinate ofRelation(FeatureStore* store, RelationPtr relation);
	static Coordinate ofFeature(FeatureStore* store, FeaturePtr feature);

private:
	class Areal
	{
	public:
		Areal() : areaSum_(0), areaCentroidX_(0), areaCentroidY_(0) {}

		template<typename Iter>
		void addRing(Iter& iter, bool isShell)
		{
			double ringSum = 0;
			double ringCentroidX = 0;
			double ringCentroidY = 0;

			Coordinate c = iter.next();
			double x1 = c.x;
			double y1 = c.y;
			for (int count = iter.coordinatesRemaining(); count > 0; count--)
			{
				c = iter.next();
				double x2 = c.x;
				double y2 = c.y;
				double a = x1 * y2 - x2 * y1;
				ringSum += a;
				ringCentroidX += (x1 + x2) * a;
				ringCentroidY += (y1 + y2) * a;
				x1 = x2;
				y1 = y2;
			}
			double sign = (ringSum >= 0 && isShell) ? 1.0 : -1.0;
			areaSum_ += ringSum * sign;
			areaCentroidX_ += ringCentroidX * sign;
			areaCentroidY_ += ringCentroidY * sign;
		}

		void addAreaRelation(FeatureStore* store, RelationPtr relation);
		bool isEmpty() const { return areaSum_ == 0; }
		Coordinate centroid() const
		{
			return Coordinate(
				static_cast<int32_t>(round(areaCentroidX_ / (3.0 * areaSum_))),
				static_cast<int32_t>(round(areaCentroidY_ / (3.0 * areaSum_))));
		}

	private:
		double areaSum_;
		double areaCentroidX_;
		double areaCentroidY_;
	};

	class Lineal
	{
	public:
		Lineal() : totalLength_(0), lineCentroidX_(0), lineCentroidY_(0) {}

		void addLineSegments(WayPtr way);

		bool isEmpty() const { return totalLength_ == 0; }
		Coordinate centroid() const
		{
			return Coordinate(
				static_cast<int32_t>(round(lineCentroidX_ / (totalLength_ * 2.0))),
				static_cast<int32_t>(round(lineCentroidY_ / (totalLength_ * 2.0))));
		}

	private:
		double totalLength_;
		double lineCentroidX_;
		double lineCentroidY_;
	};

	class Puntal
	{
	public:
		Puntal() : pointCount_(0), pointCentroidX_(0), pointCentroidY_(0) {}

		void addPoint(Coordinate point);
		bool isEmpty() const { return pointCount_ == 0; }
		Coordinate centroid() const
		{
			return Coordinate(
				static_cast<int32_t>(round(pointCentroidX_ / pointCount_)),
				static_cast<int32_t>(round(pointCentroidY_ / pointCount_)));
		}

	private:
		double pointCentroidX_;
		double pointCentroidY_;
		size_t pointCount_;
	};

private:
	void addWay(WayPtr way);
	void addRelation(FeatureStore* store, RelationPtr rel, RecursionGuard& guard);
	
	Areal areal_;
	Lineal lineal_;
	Puntal puntal_;
};

// \endcond



} // namespace geodesk
