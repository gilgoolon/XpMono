﻿#include <WinSock2.h>

#include "Networking/WsaInstance.hpp"

#include "Exception.hpp"
#include "Trace.hpp"

WsaInstance::WsaInstance()
{
	WSADATA wsa_data{};
	static constexpr WORD VERSION = MAKEWORD(2, 2);
	static constexpr int FAILED = 0;
	if (WSAStartup(VERSION, &wsa_data) != FAILED)
	{
		throw WsaException(ErrorCode::FAILED_WSA_START);
	}
}

WsaInstance::~WsaInstance()
{
	try
	{
		static constexpr int SUCCEEDED = 0;
		if (WSACleanup() != SUCCEEDED)
		{
			TRACE(L"failed to close Wsa runtime");
		}
	}
	catch (...)
	{
		TRACE(L"failed to close Wsa runtime");
	}
}
