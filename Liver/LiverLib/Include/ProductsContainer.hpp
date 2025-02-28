#pragma once
#include "IProduct.hpp"
#include "Synchronization/CriticalSection.hpp"

class ProductsContainer final
{
public:
	explicit ProductsContainer() = default;
	~ProductsContainer() = default;
	ProductsContainer(const ProductsContainer&) = delete;
	ProductsContainer& operator=(const ProductsContainer&) = delete;
	ProductsContainer(ProductsContainer&&) = delete;
	ProductsContainer& operator=(ProductsContainer&&) = delete;

	void insert_all(std::vector<IProduct::Ptr> products);
	[[nodiscard]] std::vector<IProduct::Ptr> pop_all();
	[[nodiscard]] bool has_new() const;

private:
	CriticalSection m_lock;
	std::vector<IProduct::Ptr> m_products;
};
