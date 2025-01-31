#pragma once

namespace Shellcode
{
template <typename T>
class Array final
{
public:
	explicit Array();
	~Array();
	Array(const Array&) = delete;
	Array& operator=(const Array&) = delete;
	Array(Array&&) = delete;
	Array& operator=(Array&&) = delete;

private:
	T*
};

template <typename T>
Array<T>::Array()
{
}
}
