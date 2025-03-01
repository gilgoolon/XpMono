#pragma once
#include "FigApi.hpp"
#include "Commands/ICommand.hpp"

class UnloadFigCommand final : public ICommand
{
public:
	explicit UnloadFigCommand(Id id, Fig::FigId fig_id);
	~UnloadFigCommand() override = default;
	UnloadFigCommand(const UnloadFigCommand&) = delete;
	UnloadFigCommand& operator=(const UnloadFigCommand&) = delete;
	UnloadFigCommand(UnloadFigCommand&&) = delete;
	UnloadFigCommand& operator=(UnloadFigCommand&&) = delete;

	[[nodiscard]] Type type() const override;

	friend class UnloadFigHandler;

private:
	Fig::FigId m_fig_id;
};
