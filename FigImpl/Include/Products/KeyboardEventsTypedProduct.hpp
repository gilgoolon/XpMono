#pragma once
#include "KeyboardEvent.hpp"
#include "TypedProduct.hpp"

class KeyboardEventsTypedProduct final : public TypedProduct
{
public:
	explicit KeyboardEventsTypedProduct(KeyboardEvents keyboard_events);
	~KeyboardEventsTypedProduct() override = default;
	KeyboardEventsTypedProduct(const KeyboardEventsTypedProduct&) = delete;
	KeyboardEventsTypedProduct& operator=(const KeyboardEventsTypedProduct&) = delete;
	KeyboardEventsTypedProduct(KeyboardEventsTypedProduct&&) = delete;
	KeyboardEventsTypedProduct& operator=(KeyboardEventsTypedProduct&&) = delete;

	[[nodiscard]] Type type() const override;
	[[nodiscard]] Buffer data() const override;

private:
	KeyboardEvents m_keyboard_events;
};
