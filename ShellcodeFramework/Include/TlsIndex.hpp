#pragma once
#include <cstdint>

class TlsIndex final
{
public:
	explicit TlsIndex(bool& result);
	~TlsIndex();
	TlsIndex(const TlsIndex&) = delete;
	TlsIndex& operator=(const TlsIndex&) = delete;
	TlsIndex(TlsIndex&&) = delete;
	TlsIndex& operator=(TlsIndex&&) = delete;

	bool set_value(uint8_t* value);

private:
	uint32_t m_index;
	bool m_initialized;

	[[nodiscard]] static bool allocate_index(uint32_t& index);
};
