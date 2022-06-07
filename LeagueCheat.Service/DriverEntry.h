#pragma once

#include <ntifs.h>
#include <intrin.h>

#include "ntos.h"

#pragma warning (disable: 4022 4100 4311 4047 4024 6273 4146 4113 4189 6011 4201 4091)

#define LOG(x, ...) DbgPrintEx(0, 0, x, __VA_ARGS__)