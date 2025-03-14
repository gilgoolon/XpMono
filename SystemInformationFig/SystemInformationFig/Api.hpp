﻿#pragma once
#include "FigApi.hpp"

namespace Api
{
static constexpr Fig::FigId FIG_ID = 1;
static constexpr Fig::VersionMajor VERSION_MAJOR = 1;
static constexpr Fig::VersionMinor VERSION_MINOR = 0;

enum class OperationType : Fig::OperationType
{
	RESERVED = 0,
	OS_INFO,
	USERS_INFO,
	PHYSICAL_DRIVES_INFO,
	BIOS_INFO,
};
}
