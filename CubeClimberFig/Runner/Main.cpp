﻿#include "Exception.hpp"
#include "FigException.hpp"
#include "FigModule.hpp"
#include "FigOperation.hpp"
#include "Trace.hpp"
#include "Filesystem/File.hpp"

#include <ApricotException.hpp>

#include <Windows.h>

static void main_logic();

int WINAPI wWinMain([[maybe_unused]] HINSTANCE hInstance,
                    [[maybe_unused]] HINSTANCE hPrevInstance,
                    [[maybe_unused]] PWSTR pCmdLine,
                    [[maybe_unused]] int nCmdShow)
{
	try
	{
		main_logic();
		return EXIT_SUCCESS;
	}
	catch (const ApricotException& ex)
	{
		TRACE(
			"uncaught ApricotException with code ",
			ex.code(),
			" and ApricotCode ",
			static_cast<uint32_t>(ex.apricot_code())
		);
	}
	catch (const FigException& ex)
	{
		TRACE(
			"uncaught FigException with code ",
			ex.code(),
			" and FigCode ",
			static_cast<uint32_t>(ex.fig_code()),
			" and FigSpecificCode ",
			ex.fig_specific_code()
		);
	}
	CATCH_AND_TRACE()
	
	return EXIT_SUCCESS;
}

static void main_logic()
{
	static constexpr Fig::FigId FIG_ID = 1;
	const Buffer fig_buffer = File(L"../Debug/CubeClimberFig.dll", File::Mode::READ, File::Disposition::OPEN).read();
	const auto fig = std::make_shared<FigModule>(FIG_ID, fig_buffer);
	TRACE("fig id: ", fig->id(), " fig version: ", fig->major(), ".", fig->minor());
	const std::unique_ptr<FigOperation> fig_operation = fig->execute(1, {});
	bool is_over = false;
	File output(LR"(C:\Users\alper\OneDrive\Documents\output.txt)", File::Mode::WRITE, File::Disposition::OVERRIDE);
	while (!is_over)
	{
		const FigModule::StatusResult status = fig_operation->wait();
		switch (status.execution_status)
		{
		case Fig::ExecutionStatus::FAILED:
			TRACE(L"failed with fig code: ", status.fig_specific_code);
			throw Exception(ErrorCode::FAILED_FIG_EXECUTE);
		case Fig::ExecutionStatus::FINISHED:
			is_over = true;
			break;
		case Fig::ExecutionStatus::EXECUTING:
			continue;
		case Fig::ExecutionStatus::EXECUTING_CAN_TAKE:
			break;
		}
		Buffer data;
		do
		{
			data = fig_operation->take();
			output.write(data);
		}
		while (!data.empty());
	}
	TRACE("finished successfully");
}
