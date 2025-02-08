#pragma once
#include "IOperationHandler.hpp"

class DirlistHandler final : public IOperationHandler
{
public:
	static constexpr Fig::FigId TYPE = 2;

	using IOperationHandler::IOperationHandler;
	~DirlistHandler() override = default;
	DirlistHandler(const DirlistHandler&) = delete;
	DirlistHandler& operator=(const DirlistHandler&) = delete;
	DirlistHandler(DirlistHandler&&) = delete;
	DirlistHandler& operator=(DirlistHandler&&) = delete;

	void run() override;
};
