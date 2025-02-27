#include "IProduct.hpp"

#include "Utils/Random.hpp"

IProduct::IProduct(const uint32_t id):
	m_id(id)

{
}

IProduct::IProduct():
	IProduct(Random::generate<Id>())
{
}

IProduct::Id IProduct::id() const
{
	return m_id;
}
