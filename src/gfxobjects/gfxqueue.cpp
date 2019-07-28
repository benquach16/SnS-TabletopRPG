#include "gfxqueue.h"
#include "../game.h"

struct {
	bool operator()(GFXObject a, GFXObject b) const {
		return a.getZ() < b.getZ();
	}
} comparator;

void GFXQueue::render()
{
	//dumb brute force sort
	std::sort(m_queuedObjects.begin(), m_queuedObjects.end(), comparator);
	
	for(auto i : m_queuedObjects) {
		Game::getWindow().draw(*i.getDraw());
		i.cleanup();
	}

	m_queuedObjects.clear();
}
