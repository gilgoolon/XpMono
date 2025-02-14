#include "Exception.hpp"
#include "FigException.hpp"
#include "FigModule.hpp"
#include "FigOperation.hpp"
#include "Json.hpp"
#include "Trace.hpp"
#include "Filesystem/File.hpp"
#include "Protections/ProgramProtector.hpp"
#include "Utils/Strings.hpp"

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
		Protections::ProgramProtector protector;
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
		)
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
		)
	}
	catch (const WinApiException& ex)
	{
		TRACE("uncaught WinApiException with code ", ex.code(), " and error ", ex.error())
	}
	catch (const Exception& ex)
	{
		TRACE("uncaught Exception with code ", ex.code())
	}
	catch (const std::exception& ex)
	{
		TRACE("uncaught std::exception: ", ex.what())
	}
	catch (...)
	{
		TRACE("uncaught unknown or critical exception")
	}
	return EXIT_SUCCESS;
}

static void main_logic()
{
	static constexpr Fig::FigId FIG_ID = 1;
	std::filesystem::path path = L"../Debug/InfoIngestorFig.dll";
#ifndef _DEBUG
	const Buffer fig_buffer = File(path, File::Mode::READ, File::Disposition::OPEN).read();
	const auto fig = std::make_shared<FigModule>(FIG_ID, fig_buffer);
#else
	const auto fig = std::make_shared<FigModule>(FIG_ID, path);
#endif
	TRACE("fig id: ", fig->id(), " fig version: ", fig->major(), ".", fig->minor());
	const Json parameters = {
		{"path", "C:\\Users"},
		{"depth", 1}
	};
	TRACE(L"json value: ", parameters.dump().c_str())
	const std::unique_ptr<FigOperation> fig_operation = fig->execute(2, Json::to_bson(parameters));
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
	TRACE("finished successfully")
}
