#pragma once

#include <memory>
#include <type_traits>
#include <Windows.h>

struct ServiceHandleCloser
{
	void operator()(SC_HANDLE handle) const;
};

using ScopedServiceHandle = std::unique_ptr<std::remove_pointer_t<SC_HANDLE>, ServiceHandleCloser>;
