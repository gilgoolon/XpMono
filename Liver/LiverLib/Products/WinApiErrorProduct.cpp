#include "Products/WinApiErrorProduct.hpp"

#include "Utils/Strings.hpp"

WinApiErrorProduct::WinApiErrorProduct(const ICommand::Ptr& command,
                                       const ErrorCode error_code,
                                       const uint32_t win_api_error):
	ErrorProduct(command, error_code),
	m_win_api_error(win_api_error)
{
}

IProduct::Type WinApiErrorProduct::type() const
{
	return Type::WIN_API_ERROR_PRODUCT;
}

Buffer WinApiErrorProduct::data() const
{
	return Strings::concat(ErrorProduct::data(), as_buffer(m_win_api_error));
}
