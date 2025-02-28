#pragma once
#include "IProduct.hpp"
#include "IRequest.hpp"

class ReturnProductsRequest final : public IRequest
{
public:
	explicit ReturnProductsRequest(uint32_t liver_id, std::vector<IProduct::Ptr> products);
	~ReturnProductsRequest() override = default;
	ReturnProductsRequest(const ReturnProductsRequest&) = delete;
	ReturnProductsRequest& operator=(const ReturnProductsRequest&) = delete;
	ReturnProductsRequest(ReturnProductsRequest&&) = delete;
	ReturnProductsRequest& operator=(ReturnProductsRequest&&) = delete;

	[[nodiscard]] Buffer data() const override;

private:
	std::vector<IProduct::Ptr> m_products;
};
