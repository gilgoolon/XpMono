#include "GetHistoryHandler.hpp"
#include "FigException.hpp"
#include "Trace.hpp"
#include "Media/BitmapFormat.hpp"
#include "Processes/MemoryPatternScanner.hpp"
#include "Processes/Process.hpp"
#include "Products/ImageTypedProduct.hpp"
#include "Products/TextTypedProduct.hpp"
#include "Services/Service.hpp"
#include "Services/ServiceIterator.hpp"

namespace Parameters
{
static constexpr auto HISTORY_TYPE = "history_type";
}

GetHistoryHandler::GetHistoryHandler(std::unique_ptr<Event> operation_event, const Json& parameters) :
	IOperationHandler(std::move(operation_event)),
	m_history_type(parameters[Parameters::HISTORY_TYPE].get<std::string>())
{
	if (m_history_type != HistoryType::TEXT && m_history_type != HistoryType::SCREENSHOT)
	{
		throw FigException(ErrorCode::INVALID_ARGUMENT, Fig::FigCode::FAILED_RUN);
	}
}

GetHistoryHandler::GetHistoryHandler(std::unique_ptr<Event> operation_event, const Buffer& parameters) :
	GetHistoryHandler(std::move(operation_event), Json::parse(Strings::to_string(parameters)))
{
}

void GetHistoryHandler::run()
{
	const uint32_t pid = get_clipboard_service_process_pid();
	if (pid == 0)
	{
		throw FigException(ErrorCode::PROCESS_NOT_FOUND, Fig::FigCode::FAILED_RUN);
	}

	const Process process(pid);

	if (!process.is_running())
	{
		throw FigException(ErrorCode::PROCESS_NOT_RUNNING, Fig::FigCode::FAILED_RUN);
	}

	if (m_history_type == HistoryType::TEXT)
	{
		handle_text_history(process);
	}
	else
	{
		handle_screenshot_history(process);
	}
}

void GetHistoryHandler::handle_text_history(const Process& clipboard_service_process)
{
	const MemoryPatternScanner scanner(
		clipboard_service_process,
		{
			[this](const Process& process, const uintptr_t address)
			{
				return is_text_pattern(process, address);
			}
		}
	);

	std::wstring total_text_history;
	while (scanner.has_next())
	{
		const void* address = scanner.next();
		std::wstring text_data = get_text_data_from_text_pattern(
			clipboard_service_process,
			reinterpret_cast<uintptr_t>(address)
		);
		if (text_data.empty())
		{
			continue;
		}

		total_text_history += L"======================== Clipboard Content ========================\n" + text_data +
			L"\n=================================================================\n\n";
	}

	auto product = std::make_unique<TextTypedProduct>(std::move(total_text_history));
	append(std::move(product));
	finished();
}

void GetHistoryHandler::handle_screenshot_history(const Process& clipboard_service_process)
{
	const MemoryPatternScanner scanner(
		clipboard_service_process,
		{
			[this](const Process& process, const uintptr_t address)
			{
				return is_screenshot_pattern(process, address);
			}
		}
	);

	while (scanner.has_next())
	{
		const void* address = scanner.next();
		Buffer screenshot_bitmap = get_screenshot_bitmap_from_screenshot_pattern(
			clipboard_service_process,
			reinterpret_cast<uintptr_t>(address)
		);
		if (screenshot_bitmap.empty())
		{
			continue;
		}

		auto product = std::make_unique<ImageTypedProduct>(std::move(screenshot_bitmap));
		append(std::move(product));
	}
	finished();
}

bool GetHistoryHandler::is_text_pattern(const Process& process, const uintptr_t address)
{
	static constexpr size_t END_BYTE_OFFSET = 0x20;
	static constexpr size_t RDATA_ADDRESS_OFFSET = 0x00;
	static constexpr size_t DATA_TYPE_ADDRESS_OFFSET = 0x08;
	static constexpr size_t DATA_TYPE_VALUE_OFFSET = 0x1c;

	static constexpr BYTE EXPECTED_END_BYTE = 0x01;
	static constexpr wchar_t WINDOWS_DATA_TRANSFER_DLL[] = L"windows.applicationmodel.datatransfer.dll";
	static constexpr size_t MAX_DATA_TYPE_LENGTH = 20;
	static const std::wstring TEXT_TYPE_VALUE = L"Text";

	const HMODULE windows_data_transfer_module = process.get_module(WINDOWS_DATA_TRANSFER_DLL);

	const auto end_byte = process.read_memory(
		reinterpret_cast<void*>(address + END_BYTE_OFFSET),
		sizeof(BYTE)
	);
	if (!end_byte || end_byte.value().empty() || end_byte.value()[0] != EXPECTED_END_BYTE)
	{
		return false;
	}

	const auto rdata_address_buffer = process.read_memory(
		reinterpret_cast<void*>(address + RDATA_ADDRESS_OFFSET),
		sizeof(DWORD_PTR)
	);
	if (!rdata_address_buffer)
	{
		return false;
	}
	const uintptr_t rdata_address = *reinterpret_cast<const DWORD_PTR*>(rdata_address_buffer.value().data());
	if (!process.is_address_in_rdata_section_of_module(windows_data_transfer_module, rdata_address))
	{
		return false;
	}

	const auto data_type_address_buffer = process.read_memory(
		reinterpret_cast<void*>(address + DATA_TYPE_ADDRESS_OFFSET),
		sizeof(DWORD_PTR)
	);
	if (!data_type_address_buffer)
	{
		return false;
	}
	const uintptr_t data_type_address = *reinterpret_cast<const DWORD_PTR*>(data_type_address_buffer.value().data());

	const auto data_type_buffer = process.read_memory(
		reinterpret_cast<void*>(data_type_address + DATA_TYPE_VALUE_OFFSET),
		MAX_DATA_TYPE_LENGTH * sizeof(wchar_t)
	);
	if (!data_type_buffer)
	{
		return false;
	}

	const auto data_type_raw = reinterpret_cast<const wchar_t*>(data_type_buffer.value().data());
	std::wstring data_type(data_type_raw, std::wcslen(data_type_raw));
	if (data_type.empty() || data_type != TEXT_TYPE_VALUE)
	{
		return false;
	}

	return true;
}

bool GetHistoryHandler::is_screenshot_pattern(const Process& process, const uintptr_t address)
{
	static constexpr size_t REQUIRED_HEADER_SIZE = 40;
	static constexpr long MIN_WIDTH = 1;
	static constexpr long MAX_WIDTH = 4096;
	static constexpr long MIN_HEIGHT = -4096;
	static constexpr long MAX_HEIGHT = 4096;
	static constexpr unsigned short REQUIRED_PLANES = 1;
	static constexpr unsigned short REQUIRED_BIT_COUNT = 32;

	const auto bmp_header_buffer = process.read_memory(reinterpret_cast<void*>(address), sizeof(BITMAPINFOHEADER));
	if (!bmp_header_buffer || bmp_header_buffer.value().size() < sizeof(BITMAPINFOHEADER))
	{
		return false;
	}
	const auto bmp_header = reinterpret_cast<const BITMAPINFOHEADER*>(bmp_header_buffer.value().data());

	if (bmp_header->biSize != REQUIRED_HEADER_SIZE || bmp_header->biWidth < MIN_WIDTH || bmp_header->biWidth > MAX_WIDTH
		|| bmp_header->biHeight < MIN_HEIGHT || bmp_header->biHeight > MAX_HEIGHT || bmp_header->biHeight == 0 ||
		bmp_header->biPlanes != REQUIRED_PLANES || bmp_header->biBitCount != REQUIRED_BIT_COUNT)
	{
		return false;
	}

	return true;
}

std::wstring GetHistoryHandler::get_text_data_from_text_pattern(const Process& process, const uintptr_t address)
{
	static constexpr size_t TEXT_DATA_ADDRESS_OFFSET = 0x18;
	static constexpr size_t ADD_ONCE = 1;

	const auto text_data_address_buffer = process.read_memory(
		reinterpret_cast<void*>(address + TEXT_DATA_ADDRESS_OFFSET),
		sizeof(uintptr_t)
	);
	if (!text_data_address_buffer)
	{
		return L"";
	}
	const uintptr_t text_data_address = *reinterpret_cast<const uintptr_t*>(text_data_address_buffer.value().data());

	std::wstring text_data;
	size_t current_offset = 0;
	while (true)
	{
		auto chunk_buffer = process.read_memory(
			reinterpret_cast<void*>(text_data_address + current_offset),
			sizeof(wchar_t)
		);
		if (!chunk_buffer)
		{
			return L"";
		}
		const wchar_t chunk = *reinterpret_cast<const wchar_t*>(chunk_buffer.value().data());

		if (chunk == L'\0')
		{
			break;
		}

		text_data.append(ADD_ONCE, chunk);
		current_offset += sizeof(wchar_t);
	}

	return text_data;
}

Buffer GetHistoryHandler::get_screenshot_bitmap_from_screenshot_pattern(const Process& process, const uintptr_t address)
{
	const auto bmp_header_buffer = process.read_memory(reinterpret_cast<void*>(address), sizeof(BITMAPINFOHEADER));
	if (!bmp_header_buffer)
	{
		return Buffer{};
	}
	const auto bmp_header = reinterpret_cast<const BITMAPINFOHEADER*>(bmp_header_buffer.value().data());

	const size_t pixel_data_size = bmp_header->biWidth * abs(bmp_header->biHeight) * (bmp_header->biBitCount / 8);
	auto pixel_data = process.read_memory(
		reinterpret_cast<void*>(address + bmp_header->biSize),
		pixel_data_size
	);
	if (!pixel_data)
	{
		return Buffer{};
	}

	BITMAP bitmap = {};
	bitmap.bmType = 0;
	bitmap.bmWidth = bmp_header->biWidth;
	bitmap.bmHeight = bmp_header->biHeight;
	bitmap.bmPlanes = bmp_header->biPlanes;
	bitmap.bmBitsPixel = bmp_header->biBitCount;
	bitmap.bmBits = pixel_data.value().data();

	return Formats::Bitmap::create(bitmap, pixel_data.value());
}

uint32_t GetHistoryHandler::get_clipboard_service_process_pid()
{
	static constexpr wchar_t CLIPBOARD_SERVICE_NAME[] = L"cbdhsvc";

	const ServiceIterator service_iterator;
	while (service_iterator.has_next())
	{
		const auto service = service_iterator.next();
		if (service->get_name().starts_with(CLIPBOARD_SERVICE_NAME))
		{
			return service->open()->get_pid();
		}
	}

	return 0;
}
