#pragma once
#include "Socket.hpp"
#include "WsaInstance.hpp"

class MaintainedSocket final : public IIOStream
{
public:
	explicit MaintainedSocket(SocketAddress address);
	~MaintainedSocket() override = default;
	MaintainedSocket(const MaintainedSocket&) = delete;
	MaintainedSocket& operator=(const MaintainedSocket&) = delete;
	MaintainedSocket(MaintainedSocket&&) = delete;
	MaintainedSocket& operator=(MaintainedSocket&&) = delete;

private:
	void maintain_connection() const;

public:
	[[nodiscard]] Buffer read(uint32_t size) const override;
	void write(const Buffer& data) const override;

private:
	WsaInstance m_wsa_instance;
	mutable std::unique_ptr<Socket> m_socket;
	SocketAddress m_address;
};
