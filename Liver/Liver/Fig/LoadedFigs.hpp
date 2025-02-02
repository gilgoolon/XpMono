#pragma once
#include "FigModule.hpp"

#include <unordered_map>

class LoadedFigs final
{
public:
	explicit LoadedFigs();
	~LoadedFigs() = default;
	LoadedFigs(const LoadedFigs&) = delete;
	LoadedFigs& operator=(const LoadedFigs&) = delete;
	LoadedFigs(LoadedFigs&&) = delete;
	LoadedFigs& operator=(LoadedFigs&&) = delete;

	void load(Fig::FigId id, const Buffer& buffer);

	void unload(Fig::FigId id);

private:
	std::unordered_map<Fig::FigId, std::unique_ptr<FigModule>> m_loaded_figs;
};
