#include "KeyboardEventsTypedProduct.hpp"

KeyboardEventsTypedProduct::KeyboardEventsTypedProduct(KeyboardEvents keyboard_events):
	m_keyboard_events(std::move(keyboard_events))
{
}

TypedProduct::Type KeyboardEventsTypedProduct::type() const
{
	return Type::KEYBOARD_EVENTS;
}

Buffer KeyboardEventsTypedProduct::data() const
{
	const auto* const start = reinterpret_cast<const uint8_t*>(m_keyboard_events.data());
	return Buffer{start, start + m_keyboard_events.size() * sizeof(KeyboardEvent)};
}
