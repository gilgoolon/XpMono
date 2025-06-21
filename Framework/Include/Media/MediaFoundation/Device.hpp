#pragma once
#include "MediaSource.hpp"
#include "Interfaces/ISerializableStruct.hpp"
#include "Wmi/WmiReleaser.hpp"

#include <mfobjects.h>
#include <string>

namespace MediaFoundation
{
class Device : public ISerializableStruct
{
	explicit Device(uint32_t index, IMFActivate* device);

public:
	~Device() override = default;
	Device(const Device&) = delete;
	Device& operator=(const Device&) = delete;
	Device(Device&&) = delete;
	Device& operator=(Device&&) = delete;

	friend class Attributes;

	[[nodiscard]] MediaSource activate();

	[[nodiscard]] uint32_t get_index() const;
	[[nodiscard]] std::wstring get_friendly_name() const;
	[[nodiscard]] std::wstring get_symbolic_link() const;

private:
	uint32_t m_index;
	WmiReleaser m_device;

	[[nodiscard]] IMFActivate* get() const;

	[[nodiscard]] std::wstring get_allocated_string(const GUID& property_guid) const;

public:
	[[nodiscard]] std::wstring type() const override;
	[[nodiscard]] Fields fields() const override;
};
}
