#pragma once
#include <memory>
#include <Wbemidl.h>

struct WmiReleaserDeleter final
{
	void operator()(IUnknown* object) const;
};

using WmiReleaser = std::unique_ptr<IUnknown, WmiReleaserDeleter>;
