﻿#pragma once
#include "ICommunicator.hpp"
#include "Interfaces/IInputStream.hpp"
#include "Interfaces/IIOStream.hpp"
#include "Interfaces/IOutputStream.hpp"

class RawCommunicator final : public ICommunicator
{
public:
	explicit RawCommunicator(IInputStream::Ptr input, IOutputStream::Ptr output);
	~RawCommunicator() override = default;
	RawCommunicator(const RawCommunicator&) = delete;
	RawCommunicator& operator=(const RawCommunicator&) = delete;
	RawCommunicator(RawCommunicator&&) = delete;
	RawCommunicator& operator=(RawCommunicator&&) = delete;

	[[nodiscard]] IResponse::Ptr send(IRequest::Ptr request) override;

	[[nodiscard]] static std::unique_ptr<RawCommunicator> from_stream(const IIOStream::Ptr& stream);

private:
	IInputStream::Ptr m_input;
	IOutputStream::Ptr m_output;
};
