#pragma once
#include "Commands/ICommand.hpp"
#include "Utils/Buffer.hpp"

class LoadFigCommand final : public ICommand
{
public:
	explicit LoadFigCommand(Id id, Buffer fig_buffer);
	~LoadFigCommand() override = default;
	LoadFigCommand(const LoadFigCommand&) = delete;
	LoadFigCommand& operator=(const LoadFigCommand&) = delete;
	LoadFigCommand(LoadFigCommand&&) = delete;
	LoadFigCommand& operator=(LoadFigCommand&&) = delete;

	[[nodiscard]] Type type() const override;

	friend class LoadFigHandler;

private:
	Buffer m_fig_buffer;
};
