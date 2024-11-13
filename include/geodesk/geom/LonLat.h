// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <geodesk/geom/Mercator.h>
#include <clarisma/text/Format.h>

namespace geodesk {

class LonLat
{
public:
	LonLat(Coordinate c) :
		lon(Mercator::lonFromX(c.x)),
		lat(Mercator::latFromY(c.y)) {}

	void format(char* buf) const
	{
		Format::unsafe(buf, "%.7f,%.7f", lon, lat);
	}

	std::string toString() const
	{
		char buf[32];
		format(buf);
		return std::string(buf);
	}

	double lon;
	double lat;
};

template<typename Stream>
Stream& operator<<(Stream& out, const LonLat& lonlat)
{
	char buf[32];
	lonlat.format(buf);
	out.write(buf, strlen(buf));
	return out;
}


} // namespace geodesk
