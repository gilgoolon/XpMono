#pragma once
#include <random>

namespace Random
{
template <typename T>
[[nodiscard]] T generate();

template <typename T>
T generate()
{
	std::random_device random_device;
	std::mt19937 generator(random_device());
	std::uniform_int_distribution<T> distribution(0, (std::numeric_limits<T>::max)());
	return distribution(generator);
}
};
