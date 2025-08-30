#pragma once
#include "Json.hpp"
#include "Handlers/IOperationHandler.hpp"
#include "Processes/Process.hpp"

namespace HistoryType
{
inline const std::string TEXT = "Text";
inline const std::string SCREENSHOT = "Screenshot";
}

class GetHistoryHandler final : public IOperationHandler
{
public:
	using IOperationHandler::IOperationHandler;
	explicit GetHistoryHandler(std::unique_ptr<Event> operation_event, const Json& parameters);
	explicit GetHistoryHandler(std::unique_ptr<Event> operation_event, const Buffer& parameters);
	~GetHistoryHandler() override = default;
	GetHistoryHandler(const GetHistoryHandler&) = delete;
	GetHistoryHandler& operator=(const GetHistoryHandler&) = delete;
	GetHistoryHandler(GetHistoryHandler&&) = delete;
	GetHistoryHandler& operator=(GetHistoryHandler&&) = delete;

	void run() override;

private:
	std::string m_history_type;

	static uint32_t get_clipboard_service_process_pid();
	void handle_text_history(const Process& clipboard_service_process);
	void handle_screenshot_history(const Process& clipboard_service_process);
	static bool is_text_pattern(const Process& process, uintptr_t address);
	static bool is_screenshot_pattern(const Process& process, uintptr_t address);
	static std::wstring get_text_data_from_text_pattern(const Process& process, uintptr_t address);
	static Buffer get_screenshot_bitmap_from_screenshot_pattern(const Process& process, uintptr_t address);
};
