#include <WinSock2.h>

#include "Networking/Socket.hpp"

#include "Trace.hpp"

Socket::Socket(const SOCKET socket):
	m_socket(socket)
{
}

Socket::Socket(const SocketAddress address):
	Socket(create())
{
	connect(address);
}

Socket::~Socket()
{
	try
	{
		static constexpr int SUCCESS = 0;
		if (closesocket(m_socket) != SUCCESS)
		{
			TRACE(L"failed to close socket");
		}
	}
	catch (...)
	{
		TRACE(L"failed to close socket");
	}
}

SOCKET Socket::create()
{
	const SOCKET result = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (result == INVALID_SOCKET)
	{
		throw WsaException(ErrorCode::FAILED_SOCKET_CREATE);
	}
	return result;
}

void Socket::connect(const SocketAddress address)
{
	sockaddr_in add{};
	add.sin_family = AF_INET;
	add.sin_addr.s_addr = htonl(address.ip);
	add.sin_port = htons(address.port);

	static constexpr int SUCCESS = 0;
	if (::connect(m_socket, reinterpret_cast<const sockaddr*>(&add), sizeof(add)) != SUCCESS)
	{
		throw WsaException(ErrorCode::FAILED_SOCKET_CONNECT);
	}
}

Buffer Socket::read(const uint32_t size) const
{
	Buffer data(size);
	static constexpr int BLOCK_UNTIL_COMPLETE = MSG_WAITALL;
	const int read_bytes = recv(
		m_socket,
		reinterpret_cast<char*>(data.data()),
		static_cast<int>(size),
		BLOCK_UNTIL_COMPLETE
	);
	if (static_cast<uint32_t>(read_bytes) != size)
	{
		throw WsaException(ErrorCode::FAILED_SOCKET_RECEIVE);
	}
	data.resize(read_bytes);
	return data;
}

void Socket::write(const Buffer& data) const
{
	static constexpr int REGULAR_SEND = 0;
	const int bytes_written = send(m_socket, reinterpret_cast<const char*>(data.data()), data.size(), REGULAR_SEND);
	if (static_cast<uint32_t>(bytes_written) != data.size())
	{
		throw WsaException(ErrorCode::FAILED_SOCKET_SEND);
	}
}
