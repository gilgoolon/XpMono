#pragma once

namespace MediaFoundation
{
class Instance final
{
public:
	explicit Instance();
	~Instance();
	Instance(const Instance&) = delete;
	Instance& operator=(const Instance&) = delete;
	Instance(Instance&&) = delete;
	Instance& operator=(Instance&&) = delete;
};
}
