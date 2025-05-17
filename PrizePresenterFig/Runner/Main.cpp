#include "Exception.hpp"
#include "FigException.hpp"
#include "FigModule.hpp"
#include "FigOperation.hpp"
#include "Json.hpp"
#include "Trace.hpp"
#include "Filesystem/File.hpp"
#include "Protections/ProgramProtector.hpp"
#include "Utils/Strings.hpp"

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
	catch ([[maybe_unused]] const FigException& ex)
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
	std::filesystem::path path = L"../"
#ifdef _DEBUG
		"Debug"
#else
		"Release"
#endif
		"/PrizePresenterFig.dll";
	const auto fig = std::make_shared<FigModule>(path);
	TRACE("fig id: ", fig->id(), " fig version: ", fig->major(), ".", fig->minor());
	const Json parameters = {
		{"path", R"(C:\Users\alper\OneDrive\Documents\new file.txt)"}
	};
	TRACE(L"json value: ", parameters.dump().c_str());
	const std::unique_ptr<FigOperation> fig_operation = fig->execute(3, Strings::to_buffer(parameters.dump()));
	bool is_over = false;
	File output(
		LR"(C:\Users\alper\OneDrive\Documents\more_output.txt)",
		File::Mode::WRITE,
		File::Disposition::OVERRIDE
	);
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
