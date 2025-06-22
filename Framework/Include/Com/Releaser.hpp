#pragma once
#include <memory>
#include <Wbemidl.h>

namespace Com
{
struct ReleaserDeleter final
{
	void operator()(IUnknown* object) const;
};

using Releaser = std::unique_ptr<IUnknown, ReleaserDeleter>;
}
