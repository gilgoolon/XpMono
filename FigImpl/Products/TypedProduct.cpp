#include "Products/TypedProduct.hpp"

#include "Utils/Strings.hpp"

Buffer TypedProduct::serialize() const
{
	const Buffer contents = data();
	return Strings::concat(ISerializable::serialize(type()), ISerializable::serialize(contents.size()), contents);
}
