#pragma once
#include "KeyboardEvent.hpp"

#include <Windows.h>

class ActiveKeySniff final
{
public:
	explicit ActiveKeySniff() = delete;
	~ActiveKeySniff() = delete;
	ActiveKeySniff(const ActiveKeySniff&) = delete;
	ActiveKeySniff& operator=(const ActiveKeySniff&) = delete;
	ActiveKeySniff(ActiveKeySniff&&) = delete;
	ActiveKeySniff& operator=(ActiveKeySniff&&) = delete;

	static void start();
	static void stop();

private:
	static void callback(int action_type, WPARAM message_type, LPARAM message_data);

public:
	static KeyboardEvents g_keyboard_events;
};
