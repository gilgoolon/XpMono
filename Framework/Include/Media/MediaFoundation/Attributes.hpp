#pragma once
#include "Device.hpp"
#include "Wmi/WmiReleaser.hpp"

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

	enum class SourceType : uint32_t
	{
		VIDEO = 0,
		AUDIO,
	};

	void set_source_type(SourceType source_type);
	[[nodiscard]] std::vector<std::unique_ptr<Device>> enumerate_devices();

private:
	WmiReleaser m_attributes;

	[[nodiscard]] IMFAttributes* get() const;

	[[nodiscard]] static IMFAttributes* create_attributes();
};
}
