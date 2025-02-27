#pragma once
#include "Interfaces/ISerializable.hpp"

#include <cstdint>
#include <memory>

class IProduct : public ISerializable
{
public:
	using Ptr = std::unique_ptr<IProduct>;
	using Id = uint32_t;

private:
	explicit IProduct(uint32_t id);

public:
	explicit IProduct();
	~IProduct() override = default;
	IProduct(const IProduct&) = delete;
	IProduct& operator=(const IProduct&) = delete;
	IProduct(IProduct&&) = delete;
	IProduct& operator=(IProduct&&) = delete;

	[[nodiscard]] Id id() const;

private:
	Id m_id;
};
