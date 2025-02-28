#include "IProduct.hpp"

#include "Utils/Random.hpp"
#include "Utils/Strings.hpp"

IProduct::IProduct(const uint32_t id, const ICommand::Id command_id):
	m_id(id),
	m_command_id(command_id)

{
}

IProduct::IProduct(const ICommand::Ptr& command):
	IProduct(Random::generate<Id>(), command->id())
{
}

IProduct::Id IProduct::id() const
{
	return m_id;
}

Buffer IProduct::serialize() const
{
	const Buffer data_buffer = data();
	return Strings::concat(
		ISerializable::serialize(m_id),
		ISerializable::serialize(m_command_id),
		ISerializable::serialize(type()),
		ISerializable::serialize(data_buffer.size()),
		data_buffer
	);
}
