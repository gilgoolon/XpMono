#pragma once
#include "Device.hpp"
#include "Com/Releaser.hpp"

#include <mfobjects.h>
#include <vector>

namespace MediaFoundation
{
class Attributes final
{
public:
	explicit Attributes();
	~Attributes() = default;
	Attributes(const Attributes&) = delete;
	Attributes& operator=(const Attributes&) = delete;
	Attributes(Attributes&&) = delete;
	Attributes& operator=(Attributes&&) = delete;

	void set_source_type(MediaType::Type source_type);
	[[nodiscard]] std::vector<std::unique_ptr<Device>> enumerate_devices();

private:
	Com::Releaser m_attributes;
	MediaType::Type m_source_type;

	[[nodiscard]] IMFAttributes* get() const;

	[[nodiscard]] static IMFAttributes* create_attributes();
};
}
