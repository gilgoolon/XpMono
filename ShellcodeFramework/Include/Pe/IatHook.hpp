#pragma once
#include "ImportedFunctionsIterator.hpp"

class IatHook final
{
public:
	explicit IatHook(const Pe::ImportedFunctionsIterator::ImportedFunction& function, void* hook);
	~IatHook();
	IatHook(const IatHook&) = delete;
	IatHook& operator=(const IatHook&) = delete;
	IatHook(IatHook&&) = delete;
	IatHook& operator=(IatHook&&) = delete;

private:
	void** m_function_pointer;
	void* m_original_value;
	bool m_is_initialized;
};
