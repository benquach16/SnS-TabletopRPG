#pragma once

#include <vector>

#include "gfxobject.h"

class GFXQueue
{
public:
	void render();
	void add(GFXObject object) { m_queuedObjects.push_back(object); }
private:
	std::vector<GFXObject> m_queuedObjects;
};
