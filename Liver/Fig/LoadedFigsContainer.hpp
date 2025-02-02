#pragma once
#include "LoadedFigs.hpp"

class LoadedFigsContainer final
{
public:
	explicit LoadedFigsContainer() = delete;
	~LoadedFigsContainer() = delete;
	LoadedFigsContainer(const LoadedFigsContainer&) = delete;
	LoadedFigsContainer& operator=(const LoadedFigsContainer&) = delete;
	LoadedFigsContainer(LoadedFigsContainer&&) = delete;
	LoadedFigsContainer& operator=(LoadedFigsContainer&&) = delete;

	[[nodiscard]] static LoadedFigs& instance();
};
