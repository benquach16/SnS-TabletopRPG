#pragma once

#include <queue>
#include <vector>
#include <unordered_map>

#include "../object/object.h"
#include "gfxobject.h"
#include "gfxqueue.h"

class Level;

class GFXLevel {
public:
	enum class ePart {
		Ground,
		Left,
		Right,
		Top
	};
    GFXLevel();
	~GFXLevel();
    void renderBkg(const Level* level);
    void run(const Level* level, vector2d center);
    void renderText();
    void resize();
	void regenerate(const Level* level);
private:
	struct Container {
		std::vector<GFXObject> objects;
	};
    sf::Texture m_texture;
    sf::Texture m_grass;
    sf::Texture m_stone;
    std::queue<sf::Text> m_texts;
    std::queue<sf::RectangleShape> m_ground;
    std::queue<sf::RectangleShape> m_top;

	std::unordered_map<ePart, sf::Shape*> m_shapes;

    GFXQueue m_queue;

	const Level* m_processedLevel;

	std::vector<Container> m_data;

    sf::RectangleShape m_bkg;
};
