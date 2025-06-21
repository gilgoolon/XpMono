#include "Media/MediaFoundation/Device.hpp"

MediaFoundation::Device::Device(IMFActivate* const device):
	m_device(device)
{
}

IMFActivate* MediaFoundation::Device::get() const
{
	return reinterpret_cast<IMFActivate*>(m_device.get());
}
