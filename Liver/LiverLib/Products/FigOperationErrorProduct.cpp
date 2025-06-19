#include "Products/FigOperationErrorProduct.hpp"

#include "Utils/Strings.hpp"

FigOperationErrorProduct::FigOperationErrorProduct(const ICommand::Ptr& command,
                                                   const Fig::FigId fig_id,
                                                   const Fig::OperationId operation_id,
                                                   const Fig::OperationType operation_type,
                                                   const Fig::FigCode fig_code,
                                                   const Fig::FigSpecificCode specific_code):
	IProduct(command),
	m_fig_id(fig_id),
	m_operation_id(operation_id),
	m_operation_type(operation_type),
	m_fig_code(fig_code),
	m_specific_code(specific_code)
{
}

IProduct::Type FigOperationErrorProduct::type() const
{
	return Type::FIG_OPERATION_ERROR_PRODUCT;
}

Buffer FigOperationErrorProduct::data() const
{
	return Strings::concat(
		ISerializable::serialize(m_fig_id),
		ISerializable::serialize(m_operation_id),
		ISerializable::serialize(m_operation_type),
		ISerializable::serialize(m_fig_code),
		ISerializable::serialize(m_specific_code)
	);
}
