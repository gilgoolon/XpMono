﻿#pragma once
#include "Processes/ILibrary.hpp"
#include "Synchronization/Event.hpp"
#include "Utils/Buffer.hpp"

#include "FigApi.hpp"
#include <filesystem>

class FigOperation;

class FigModule final : public std::enable_shared_from_this<FigModule>
{
public:
	explicit FigModule(std::unique_ptr<ILibrary> library);
	explicit FigModule(const std::filesystem::path& path);
	~FigModule() = default;
	FigModule(const FigModule&) = delete;
	FigModule& operator=(const FigModule&) = delete;
	FigModule(FigModule&&) = delete;
	FigModule& operator=(FigModule&&) = delete;

	[[nodiscard]] Fig::FigId id() const;
	[[nodiscard]] Fig::VersionMinor minor() const;
	[[nodiscard]] Fig::VersionMajor major() const;

	struct StatusResult final
	{
		Fig::ExecutionStatus execution_status;
		Fig::FigSpecificCode fig_specific_code;
	};

	[[nodiscard]] std::unique_ptr<FigOperation> execute(Fig::OperationType type, const Buffer& parameters);

private:
	std::unique_ptr<ILibrary> m_library;
	Fig::FigInterfaces m_interfaces;
	Fig::FigInformation m_information;
	Event m_fig_quit_event;

	[[nodiscard]] StatusResult status(Fig::OperationId id) const;

	[[nodiscard]] std::vector<uint8_t> take(Fig::OperationId id);

	friend class FigOperation;
};
