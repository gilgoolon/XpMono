#include "Exception.hpp"
#include "FigException.hpp"
#include "FigModule.hpp"
#include "FigOperation.hpp"
#include "Json.hpp"
#include "Trace.hpp"
#include "Filesystem/File.hpp"
#include "Protections/ProgramProtector.hpp"

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
		"/ClipboardInformationFig.dll";
	const auto fig = std::make_shared<FigModule>(path);
	TRACE("fig id: ", fig->id(), " fig version: ", fig->major(), ".", fig->minor());

	static constexpr bool IS_SCREENSHOT_HISTORY = true;
	Json parameters;
	if (IS_SCREENSHOT_HISTORY)
	{
		parameters = {
			{"history_type", "Screenshot"},
		};
	}
	else
	{
		parameters = {
			{"history_type", "Text"},
		};
	}

	const std::unique_ptr<FigOperation> fig_operation = fig->execute(4, Strings::to_buffer(parameters.dump()));
	std::filesystem::path output_path;
	bool is_over = false;
	size_t screenshot_counter = 1;

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
			static constexpr size_t DATA_METADATA_SIZE = 12;

			data = fig_operation->take();
			if (data.empty())
			{
				continue;
			}

			if (IS_SCREENSHOT_HISTORY)
			{
				output_path = LR"(C:\Users\97254\Desktop\tmp\screenshot_)" +
					std::to_wstring(screenshot_counter++) + L".bmp";
			}
			else
			{
				output_path = LR"(C:\Users\97254\Desktop\tmp\clipboard_text_history.txt)";
			}
			auto output = File(
				output_path,
				File::Mode::WRITE,
				File::Disposition::OVERRIDE
			);

			data.erase(data.begin(), data.begin() + DATA_METADATA_SIZE);
			output.write(data);
		}
		while (!data.empty());
	}
	TRACE("finished successfully");
}
