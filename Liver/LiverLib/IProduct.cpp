#include "IProduct.hpp"

#include "Utils/Random.hpp"

IProduct::IProduct():
	m_id(Random::generate<Id>())
{
}

IProduct::Id IProduct::id() const
{
	return m_id;
}
