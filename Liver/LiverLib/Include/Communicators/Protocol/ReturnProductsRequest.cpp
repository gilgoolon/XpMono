#include "ReturnProductsRequest.hpp"

ReturnProductsRequest::ReturnProductsRequest(const uint32_t liver_id, std::vector<IProduct::Ptr> products):
	IRequest(Type::SEND_PRODUCTS, liver_id),
	m_products(std::move(products))
{
}

Buffer ReturnProductsRequest::data() const
{
	Buffer result;
	for (const IProduct::Ptr& product : m_products)
	{
		const Buffer serialized = product->serialize();
		result.insert(result.end(), serialized.begin(), serialized.end());
	}
	return result;
}
