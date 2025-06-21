#pragma once
#include "MediaSource.hpp"
#include "Wmi/WmiReleaser.hpp"

#include <mfobjects.h>
#include <string>

namespace MediaFoundation
{
class Device final
{
	explicit Device(IMFActivate* device);

public:
	~Device() = default;
	Device(const Device&) = delete;
	Device& operator=(const Device&) = delete;
	Device(Device&&) = default;
	Device& operator=(Device&&) = default;

	friend class Attributes;

	[[nodiscard]] MediaSource activate();
	[[nodiscard]] std::wstring get_friendly_name() const;
	[[nodiscard]] std::wstring get_symbolik_link() const;

private:
	WmiReleaser m_device;

	[[nodiscard]] IMFActivate* get() const;

	[[nodiscard]] std::wstring get_allocated_string(const GUID& property_guid) const;
};
}
