#include "FigProduct.hpp"

#include "Utils/Strings.hpp"

FigProduct::FigProduct(const ICommand::Ptr& command,
                       const Fig::FigId fig_id,
                       const Fig::OperationId operation_id,
                       Buffer data):
	IProduct(command),
	m_fig_id(fig_id),
	m_operation_id(operation_id),
	m_data(std::move(data))
{
}

IProduct::Type FigProduct::type() const
{
	return Type::FIG_PRODUCT;
}

Buffer FigProduct::data() const
{
	return Strings::concat(
		ISerializable::serialize(m_fig_id),
		ISerializable::serialize(m_operation_id),
		m_data
	);
}
