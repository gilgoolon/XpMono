#pragma once
#include "FigApi.hpp"
#include "Commands/ICommand.hpp"
#include "Utils/Buffer.hpp"

class LoadFigCommand final : public ICommand
{
public:
	explicit LoadFigCommand(Id id, Fig::FigId fig_id, Buffer fig_buffer);
	~LoadFigCommand() override = default;
	LoadFigCommand(const LoadFigCommand&) = delete;
	LoadFigCommand& operator=(const LoadFigCommand&) = delete;
	LoadFigCommand(LoadFigCommand&&) = delete;
	LoadFigCommand& operator=(LoadFigCommand&&) = delete;

	[[nodiscard]] Type type() const override;

	friend class LoadFigHandler;

private:
	Fig::FigId m_fig_id;
	Buffer m_fig_buffer;
};
