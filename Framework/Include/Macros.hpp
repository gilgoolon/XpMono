#pragma once
#include "Exception.hpp"

#include <string>

#define MACRO_START() do {
#define MACRO_END() } while(0)

#define DEFINE_ENUM_FORMAT_CASE(x) case x: return L#x
