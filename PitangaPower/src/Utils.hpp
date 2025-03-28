#pragma once
#include <vector>
#include <utility>

namespace Utils
{

    template <typename T>
    std::vector<T> concat(std::vector<T> &&first)
    {
        return std::move(first); // Base case: move the last vector
    }
    
    template <typename T, typename... Vectors>
    std::vector<T> concat(std::vector<T> first, Vectors ...others)
    {
        std::vector<T> result = std::move(first); // Move first vector into result
        (result.insert(result.end(),
        std::make_move_iterator(others.begin()),
        std::make_move_iterator(others.end())),
        ...);
        return result;
    }
}
