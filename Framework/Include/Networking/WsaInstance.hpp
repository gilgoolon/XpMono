#pragma once

class WsaInstance final
{
public:
	explicit WsaInstance();
	~WsaInstance();
	WsaInstance(const WsaInstance&) = delete;
	WsaInstance& operator=(const WsaInstance&) = delete;
	WsaInstance(WsaInstance&&) = delete;
	WsaInstance& operator=(WsaInstance&&) = delete;
};
