#pragma once
#include "Wmi/WmiReleaser.hpp"

#include <mfobjects.h>

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

private:
	WmiReleaser m_device;

	[[nodiscard]] IMFActivate* get() const;
};
}
