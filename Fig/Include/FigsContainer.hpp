#pragma once
#include "FigModule.hpp"

#include <unordered_map>

class FigsContainer final
{
public:
	explicit FigsContainer();
	~FigsContainer() = default;
	FigsContainer(const FigsContainer&) = delete;
	FigsContainer& operator=(const FigsContainer&) = delete;
	FigsContainer(FigsContainer&&) = delete;
	FigsContainer& operator=(FigsContainer&&) = delete;

	void load(Fig::FigId id, const Buffer& buffer);

	void unload(Fig::FigId id);

private:
	std::unordered_map<Fig::FigId, std::unique_ptr<FigModule>> m_loaded_figs;
};
