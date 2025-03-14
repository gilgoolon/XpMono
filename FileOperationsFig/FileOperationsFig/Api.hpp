﻿#pragma once
#include "FigApi.hpp"

namespace Api
{
static constexpr Fig::FigId FIG_ID = 2;
static constexpr Fig::VersionMajor VERSION_MAJOR = 1;
static constexpr Fig::VersionMinor VERSION_MINOR = 0;

enum class OperationType : Fig::OperationType
{
	RESERVED = 0,
	DIRLIST,
	DROP_FILE,
	GET_FILE,
};
}
