#pragma once
#include "Protocol/IRequest.hpp"
#include "Protocol/IResponse.hpp"

class ICommunicator
{
public:
	using Ptr = std::unique_ptr<ICommunicator>;

	explicit ICommunicator() = default;
	virtual ~ICommunicator() = default;
	ICommunicator(const ICommunicator&) = delete;
	ICommunicator& operator=(const ICommunicator&) = delete;
	ICommunicator(ICommunicator&&) = delete;
	ICommunicator& operator=(ICommunicator&&) = delete;

	[[nodiscard]] virtual IResponse::Ptr send(IRequest::Ptr request) = 0;
};
