#include "Networking/MaintainedSocket.hpp"

MaintainedSocket::MaintainedSocket(const SocketAddress address):
	m_wsa_instance(),
	m_socket(nullptr),
	m_address(address)
{
}

void MaintainedSocket::maintain_connection() const
{
	m_socket = std::make_unique<Socket>(m_address);
}

Buffer MaintainedSocket::read(const uint32_t size) const
{
	return m_socket->read(size);
}

void MaintainedSocket::write(const Buffer& data) const
{
	try
	{
		if (m_socket == nullptr)
		{
			maintain_connection();
		}
		m_socket->write(data);
	}
	catch (const WsaException& ex)
	{
		if (ex.wsa_code() != WSAECONNABORTED)
		{
			throw;
		}
		maintain_connection();
		m_socket->write(data);
	}
}
