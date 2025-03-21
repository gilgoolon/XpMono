﻿#pragma once
#include "FigModule.hpp"
#include "FigOperation.hpp"
#include "Synchronization/CriticalSection.hpp"

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

	void load(std::unique_ptr<ILibrary> fig);

	void unload(Fig::FigId id);

	[[nodiscard]] std::unique_ptr<FigOperation> execute(Fig::FigId id,
	                                                    Fig::OperationType operation_type,
	                                                    const Buffer& operation_parameters);

private:
	CriticalSection m_lock;
	std::unordered_map<Fig::FigId, std::shared_ptr<FigModule>> m_loaded_figs;
};
