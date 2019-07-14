#pragma once

#include "gfxobject.h"

class Level;

class GFXLevel : public GFXObject
{
public:
	void setLevel(Level* level) { m_level = level; }
	void run() override;

private:
	Level* m_level;
};
