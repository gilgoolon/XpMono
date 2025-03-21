﻿#pragma once
#include <cstdint>
#include <memory>

class ICommand
{
public:
	enum class Type: uint32_t
	{
		LOAD_DLL = 0,
		UNLOAD_DLL,
		CALL_DLL_PROCEDURE,
		CALL_DLL_GENERIC_PROCEDURE,
		LOAD_FIG,
		UNLOAD_FIG,
		EXECUTE_FIG_OPERATION,
	};

	using Ptr = std::shared_ptr<ICommand>;
	using Id = uint32_t;

	explicit ICommand(Id command_id);
	virtual ~ICommand() = default;
	ICommand(const ICommand&) = delete;
	ICommand& operator=(const ICommand&) = delete;
	ICommand(ICommand&&) = delete;
	ICommand& operator=(ICommand&&) = delete;

	[[nodiscard]] Id id() const;
	[[nodiscard]] virtual Type type() const = 0;

private:
	Id m_id;
};
