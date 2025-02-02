#include "LoadedFigsContainer.hpp"

LoadedFigs& LoadedFigsContainer::instance()
{
	static LoadedFigs instance;
	return instance;
}
