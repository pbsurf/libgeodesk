// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <geodesk/geom/Coordinate.h>

namespace geodesk {

/// \cond lowlevel
///
class Quadrant
{
public:
	static const int NE = 0;
	static const int NW = 1;
	static const int SW = 2;
	static const int SE = 3;

	static int quadrant(Coordinate a, Coordinate b)
	{
		int xDelta = (int)(b.x < a.x);
		int yDelta = (int)(b.y < a.y); 
		return (yDelta << 1) | xDelta;
	}
};

// \endcond

} // namespace geodesk
