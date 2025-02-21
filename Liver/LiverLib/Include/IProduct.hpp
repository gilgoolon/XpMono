#pragma once
#include <cstdint>
#include <memory>

class IProduct
{
public:
	using Ptr = std::unique_ptr<IProduct>;
	using Id = uint32_t;

	explicit IProduct();
	virtual ~IProduct() = default;
	IProduct(const IProduct&) = delete;
	IProduct& operator=(const IProduct&) = delete;
	IProduct(IProduct&&) = delete;
	IProduct& operator=(IProduct&&) = delete;

	[[nodiscard]] Id id() const;

private:
	Id m_id;
};
