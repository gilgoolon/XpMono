#include "Products/TypedProduct.hpp"

#include "Utils/Strings.hpp"

Buffer TypedProduct::serialize() const
{
	const Buffer data = data();
	return Strings::concat()
}
