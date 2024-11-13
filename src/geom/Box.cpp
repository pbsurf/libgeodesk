// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#include <geodesk/geom/Box.h>
#include <clarisma/text/Format.h>

namespace geodesk {

void Box::format(char* buf) const
{
	clarisma::Format::unsafe(buf, "[(%d,%d),(%d,%d)]", minX(), minY(), maxX(), maxY());
}

std::string Box::toString() const
{
	char buf[64];
	format(buf);
	return std::string(buf);
}

} // namespace geodesk
