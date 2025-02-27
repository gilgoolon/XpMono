#include "ProductsContainer.hpp"

void ProductsContainer::insert_all(std::vector<IProduct::Ptr> products)
{
	CriticalSection::Acquired acquired = m_lock.acquire();

	for (IProduct::Ptr& product : products)
	{
		m_products.push_back(std::move(product));
	}
}

std::vector<IProduct::Ptr> ProductsContainer::pop_all()
{
	CriticalSection::Acquired acquired = m_lock.acquire();

	decltype(m_products) empty;
	std::swap(empty, m_products);
	return empty;
}
