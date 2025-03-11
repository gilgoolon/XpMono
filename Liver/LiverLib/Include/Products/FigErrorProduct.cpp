#include "FigErrorProduct.hpp"

#include "Utils/Strings.hpp"

FigErrorProduct::FigErrorProduct(const ICommand::Ptr& command,
                                 const Fig::FigId fig_id,
                                 const Fig::OperationId operation_id,
                                 const Fig::FigSpecificCode code):
	IProduct(command),
	m_fig_id(fig_id),
	m_operation_id(operation_id),
	m_code(code)
{
}

IProduct::Type FigErrorProduct::type() const
{
	return Type::FIG_ERROR_PRODUCT;
}

Buffer FigErrorProduct::data() const
{
	return Strings::concat(ISerializable::serialize(m_fig_id), ISerializable::serialize(m_code));
}
