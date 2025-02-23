#pragma once
#include <WinSock2.h>

#include "Interfaces/IIOStream.hpp"

struct SocketAddress final
{
	uint32_t ip;
	uint16_t port;
};

class Socket final : public IIOStream
{
	explicit Socket(SOCKET socket);

public:
	explicit Socket(SocketAddress address);
	~Socket() override;
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;
	Socket(Socket&&) = delete;
	Socket& operator=(Socket&&) = delete;

private:
	[[nodiscard]] static SOCKET create();
	void connect(SocketAddress address);

	[[nodiscard]] Buffer read(uint32_t size) const override;
	void write(const Buffer& data) const override;

	SOCKET m_socket;
};
