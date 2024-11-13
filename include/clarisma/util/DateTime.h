// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <chrono>

namespace clarisma {

class DateTime
{
public:
	DateTime() : timestamp_(0) {}
	explicit DateTime(int64_t millisecondsSinceEpoch) : timestamp_(millisecondsSinceEpoch) {}

	static DateTime now()
	{
		auto now = std::chrono::system_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
		return DateTime(duration.count());
	}

	operator int64_t() const { return timestamp_; }
	
private:
	int64_t timestamp_;
};

} // namespace clarisma
