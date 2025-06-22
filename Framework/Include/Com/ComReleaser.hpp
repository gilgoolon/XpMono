#pragma once
#include <memory>
#include <Wbemidl.h>

struct ComReleaserDeleter final
{
	void operator()(IUnknown* object) const;
};

using ComReleaser = std::unique_ptr<IUnknown, ComReleaserDeleter>;
