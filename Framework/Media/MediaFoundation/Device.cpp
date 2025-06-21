#include "Media/MediaFoundation/Device.hpp"

#include "Wmi/WmiException.hpp"

MediaFoundation::Device::Device(IMFActivate* const device):
	m_device(device)
{
}

MediaFoundation::MediaSource MediaFoundation::Device::activate()
{
	IMFMediaSource* media_source = nullptr;

	const HRESULT result = get()->ActivateObject(IID_PPV_ARGS(&media_source));
	if (FAILED(result))
	{
		throw WmiException(ErrorCode::FAILED_MEDIA_FOUNDATION_ACTIVATE_DEVICE, result);
	}

	return MediaSource(media_source);
}

IMFActivate* MediaFoundation::Device::get() const
{
	return reinterpret_cast<IMFActivate*>(m_device.get());
}
