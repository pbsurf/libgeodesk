// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#include <clarisma/thread/TaskStatus.h>

namespace clarisma {

const TaskStatus::Details TaskStatus::NORMAL_SHUTDOWN(TaskStatus::CODE_NORMAL_SHUTDOWN);
const TaskStatus::Details TaskStatus::ABORTED(TaskStatus::CODE_ABORTED);
const TaskStatus::Details TaskStatus::BAD_ALLOC(TaskStatus::CODE_BAD_ALLOC);

} // namespace clarisma
