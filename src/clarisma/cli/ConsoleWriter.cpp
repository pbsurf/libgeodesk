// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#include <clarisma/cli/ConsoleWriter.h>
#include <clarisma/cli/Console.h>

namespace clarisma {

ConsoleWriter::ConsoleWriter(int mode) :
	console_(Console::get()),
	indent_(0),
	timestampSeconds_(-1)
{
	setBuffer(&buf_);
	switch(mode)
	{
	case NONE:
		break;
	case SUCCESS:
		success();
		break;
	case FAILED:
		failed();
		break;
	case LOGGED:
		timestamp();
		writeConstString("  ");
		break;
	}
}

void ConsoleWriter::color(int color)
{
	writeConstString("\033[38;5;");
	formatInt(color);
	writeByte('m');
}

void ConsoleWriter::normal()
{
	writeConstString("\033[0m");
}


void ConsoleWriter::flush()
{
	if(console_->consoleState_ == Console::ConsoleState::PROGRESS)
	{
		ensureCapacityUnsafe(256);
		if(timestampSeconds_ < 0)
		{
			auto elapsed = std::chrono::steady_clock::now() - console_->startTime();
			timestampSeconds_ = static_cast<int>(
				std::chrono::duration_cast<std::chrono::seconds>(elapsed).count());
		}
		p_ = console_->formatStatus(p_, timestampSeconds_,
			console_->currentPercentage_, console_->currentTask_);
	}
	else
	{
		writeConstString("\033[K");
	}
	console_->print(data(), length());
	clear();
}

ConsoleWriter& ConsoleWriter::timestamp()
{
	ensureCapacityUnsafe(64);
	auto elapsed = std::chrono::steady_clock::now() - console_->startTime();
	int ms = static_cast<int>(
		std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count());
	div_t d;
	d = div(ms, 1000);
	int s = d.quot;
	ms = d.rem;

	bool color = console_->hasColor();
	if(color) writeConstString("\033[38;5;242m");
	p_ = Format::timer(p_, s, ms);
	if(color) writeConstString("\033[0m");
	timestampSeconds_ = s;
	indent_ = 14;
	return *this;
}

void ConsoleWriter::success()
{
	bool color = console_->hasColor();
	ensureCapacityUnsafe(64);
	putStringUnsafe("\033[2K");	// clear current line
	if(color) putStringUnsafe("\033[97;48;5;28m");
		// Or use 64 for apple green
	auto elapsed = std::chrono::steady_clock::now() - console_->startTime();
	int secs = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count());
	p_ = Format::timer(p_, secs, -1);
	if(color)
	{
		putStringUnsafe("\033[0m ");
	}
	else
	{
		putStringUnsafe(" ");
	}
}

void ConsoleWriter::failed()
{
	ensureCapacityUnsafe(64);
	putStringUnsafe("\033[2K");	// clear current line
	if(console_->hasColor())
	{
		putStringUnsafe("\033[38;5;9m ─────── \033[0m");
	}
	else
	{
		putStringUnsafe(" ------- ");
	}
}

} // namespace clarisma
