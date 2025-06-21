#pragma once
#include "Interfaces/ISerializableStruct.hpp"
#include "Utils/Time.hpp"

#include <string>
#include <vector>

struct Credential final : ISerializableStruct
{
	explicit Credential(std::wstring origin,
	                    std::wstring username,
	                    std::wstring password,
	                    std::optional<Time::Datetime> created_time,
	                    std::optional<Time::Datetime> accessed_time,
	                    std::optional<Time::Datetime> modified_time);
	~Credential() override = default;
	Credential(const Credential&) = delete;
	Credential& operator=(const Credential&) = delete;
	Credential(Credential&&) = delete;
	Credential& operator=(Credential&&) = delete;

	[[nodiscard]] std::wstring type() const override;
	[[nodiscard]] Fields fields() const override;

	std::wstring origin;
	std::wstring username;
	std::wstring password;
	std::optional<Time::Datetime> created_time;
	std::optional<Time::Datetime> accessed_time;
	std::optional<Time::Datetime> modified_time;
};

struct NotSupported final : ISerializableStruct
{
	explicit NotSupported() = default;
	~NotSupported() override = default;
	NotSupported(const NotSupported&) = delete;
	NotSupported& operator=(const NotSupported&) = delete;
	NotSupported(NotSupported&&) = delete;
	NotSupported& operator=(NotSupported&&) = delete;

	[[nodiscard]] std::wstring type() const override;
	[[nodiscard]] Fields fields() const override;
};

using Credentials = std::vector<std::unique_ptr<Credential>>;

class CredentialsGrabber
{
public:
	using Ptr = std::unique_ptr<CredentialsGrabber>;

	explicit CredentialsGrabber() = default;
	virtual ~CredentialsGrabber() = default;
	CredentialsGrabber(const CredentialsGrabber&) = delete;
	CredentialsGrabber& operator=(const CredentialsGrabber&) = delete;
	CredentialsGrabber(CredentialsGrabber&&) = delete;
	CredentialsGrabber& operator=(CredentialsGrabber&&) = delete;

	[[nodiscard]] virtual std::optional<Credentials> grab_credentials() const = 0;
	[[nodiscard]] virtual std::wstring source() const = 0;
};
