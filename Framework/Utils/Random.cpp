#include "Utils/Random.hpp"

#include <random>

uint32_t Random::uint32()
{
	std::random_device random_device;
	std::mt19937 generator(random_device());
	std::uniform_int_distribution<uint32_t> distribution(0, std::numeric_limits<uint32_t>::max());
	return distribution(generator);
}
