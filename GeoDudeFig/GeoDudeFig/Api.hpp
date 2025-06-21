#pragma once
#include "FigApi.hpp"

namespace Api
{
static constexpr Fig::FigId FIG_ID = 5;
static constexpr Fig::VersionMajor VERSION_MAJOR = 1;
static constexpr Fig::VersionMinor VERSION_MINOR = 0;

enum class OperationType : Fig::OperationType
{
	RESERVED = 0,
	DISCOVER_NETWORKS,
	FETCH_GEO_LOCATION,
};
}
