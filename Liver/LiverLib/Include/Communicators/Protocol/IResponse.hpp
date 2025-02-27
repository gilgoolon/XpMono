#pragma once
#include <cstdint>
#include <memory>

class IResponse
{
public:
	using Ptr = std::shared_ptr<IResponse>;

	explicit IResponse() = default;
	virtual ~IResponse() = default;
	IResponse(const IResponse&) = delete;
	IResponse& operator=(const IResponse&) = delete;
	IResponse(IResponse&&) = delete;
	IResponse& operator=(IResponse&&) = delete;

	enum class Type : uint32_t
	{
		KEEP_ALIVE = 0,
		EXECUTE_COMMANDS,
		SEND_RANDOM,
	};

	[[nodiscard]] virtual Type type() const = 0;
};
