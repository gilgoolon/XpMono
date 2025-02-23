#include "Networking/MaintainedSocket.hpp"

MaintainedSocket::MaintainedSocket(SocketAddress address):
	m_socket(std::make_unique<Socket>(address)),
	m_address(address)
{
}

void MaintainedSocket::maintain_connection() const
{
	if (m_socket != nullptr && m_socket->is_connected())
	{
		return;
	}
	m_socket = std::make_unique<Socket>(m_address);
}

Buffer MaintainedSocket::read(const uint32_t size) const
{
	maintain_connection();
	return m_socket->read(size);
}

void MaintainedSocket::write(const Buffer& data) const
{
	maintain_connection();
	m_socket->write(data);
}
