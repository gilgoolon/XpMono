#include "ProductsContainer.hpp"

void ProductsContainer::insert_all(std::vector<IProduct::Ptr> products)
{
	CriticalSection::Acquired acquired = m_lock.acquire();

	m_products.insert(
		m_products.end(),
		std::make_move_iterator(products.begin()),
		std::make_move_iterator(products.end())
	);
}

std::vector<IProduct::Ptr> ProductsContainer::pop_all()
{
	CriticalSection::Acquired acquired = m_lock.acquire();

	decltype(m_products) empty;
	std::swap(empty, m_products);
	return empty;
}

bool ProductsContainer::has_new() const
{
	return !m_products.empty();
}
