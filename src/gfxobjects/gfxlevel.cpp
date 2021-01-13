#include "gfxlevel.h"
#include "../game.h"
#include "../level/level.h"
#include "../object/relationmanager.h"
#include "../ui/types.h"
#include "utils.h"

using namespace std;
sf::Texture fire;
// rounding
constexpr int cWallWidthOffset = static_cast<int>(cWidth * cCos45 + 1);
constexpr int cWallHeightOffset = static_cast<int>(cHeight * cCos45);

GFXLevel::GFXLevel()
    : m_processedLevel(nullptr)
{
    fire.loadFromFile("data/textures/fire.png");
    fire.setSmooth(true);
    m_texture.loadFromFile("data/textures/test.png");
    m_texture.setSmooth(true);
    m_stone.loadFromFile("data/textures/rough.png");
    m_stone.setRepeated(true);
    m_grass.loadFromFile("data/textures/grass.png");
    m_grass.setRepeated(true);

    auto windowSize = Game::getWindow().getSize();
    sf::RectangleShape rect(sf::Vector2f(windowSize.x, windowSize.y));
    m_bkg = rect;
    resize();
}

GFXLevel::~GFXLevel() {}

void GFXLevel::resize()
{
    auto windowSize = Game::getWindow().getSize();
    m_bkg.setSize(sf::Vector2f(windowSize.x, windowSize.y));
}

void GFXLevel::renderBkg(const Level* level)
{
    resize();
    switch (level->getLighting()) {
    case eLighting::Cave:
        m_bkg.setFillColor(sf::Color::Black);
        break;
    case eLighting::Dark:
    case eLighting::Sunny:
        m_bkg.setFillColor(sf::Color(140, 226, 255));
        break;
    }
    Game::getWindow().draw(m_bkg);
}

sf::RectangleShape* GFXLevel::createGround(sf::Texture* texture, sf::Vector2f pos, bool dark)
{
    sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(cWidth, cHeight));
    rect->setFillColor(sf::Color(77, 77, 77));
    if (dark) {
        rect->setFillColor(sf::Color(54, 54, 54));
    }
    rect->setTexture(texture);
    rect->setRotation(45.f);

    rect->setPosition(sf::Vector2f(pos.x, pos.y));
    return rect;
}

sf::RectangleShape* GFXLevel::createTop(sf::Texture* texture, sf::Vector2f pos, bool dark)
{
    sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(cWidth, cHeight));
    rect->setFillColor(sf::Color(33, 33, 33));
    if (dark) {
        rect->setFillColor(sf::Color(22, 22, 22));
    }
    rect->setTexture(texture);
    rect->setRotation(45.f);

    rect->setPosition(sf::Vector2f(pos.x, pos.y - (cWallHeightOffset - 1)));
    return rect;
}

sf::ConvexShape* GFXLevel::createRight(sf::Texture* texture, sf::Vector2f pos, bool dark)
{
    sf::ConvexShape* right = new sf::ConvexShape(4);
    right->setFillColor(sf::Color(66, 66, 66));
    if (dark) {
        right->setFillColor(sf::Color(43, 43, 43));
    }
    right->setTexture(texture);

    right->setPointCount(4);
    right->setPoint(0, sf::Vector2f(0, 0));
    right->setPoint(1, sf::Vector2f(cWallWidthOffset, -(cWallHeightOffset)));
    right->setPoint(2, sf::Vector2f(cWallWidthOffset, cHeight * 2 - (cWallHeightOffset)));
    right->setPoint(3, sf::Vector2f(0, cHeight * 2));

    sf::Vector2f rightPos(pos);
    // rightPos.x+=cWidth*cCos45;
    rightPos.y += cWallHeightOffset;
    right->setPosition(rightPos);
    return right;
}

sf::ConvexShape* GFXLevel::createBottom(sf::Texture* texture, sf::Vector2f pos, bool dark)
{
    sf::ConvexShape* bottom = new sf::ConvexShape(4);
    bottom->setFillColor(sf::Color(44, 44, 44));
    if (dark) {
        bottom->setFillColor(sf::Color(31, 31, 31));
    }
    bottom->setTexture(texture);
    bottom->setPoint(0, sf::Vector2f(0, 0));
    bottom->setPoint(1, sf::Vector2f(cWallWidthOffset, cWallHeightOffset));
    bottom->setPoint(2, sf::Vector2f(cWallWidthOffset, cHeight * 2 + (cWallHeightOffset)));
    bottom->setPoint(3, sf::Vector2f(0, cHeight * 2));

    sf::Vector2f bottomPos(pos);
    bottomPos.x -= cWallWidthOffset;
    bottomPos.y += cCos45;
    bottom->setPosition(bottomPos);
    return bottom;
}

void GFXLevel::regenerate(const Level* level)
{
    for (auto i : m_litData) {
        for (auto draw : i.objects) {
            draw.cleanup();
        }
    }
    for (auto i : m_darkData) {
        for (auto draw : i.objects) {
            draw.cleanup();
        }
    }
    m_visibleData.clear();
    m_darkData.clear();
    m_litData.clear();
    int width = level->getWidth();
    int height = level->getHeight();
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            Tile tile = (*level)(x, y);
            sf::Texture* texture = &m_stone;
            switch (tile.m_material) {
            case eTileMaterial::Grass:
                texture = &m_grass;
                break;
            }
            Container litContainer;
            Container darkContainer;
            if (tile.m_type == eTileType::Ground) {
                sf::Vector2f pos(x, y);
                pos = coordsToScreen(pos);
                sf::RectangleShape* rect = createGround(texture, pos, false);
                sf::RectangleShape* rect2 = createGround(texture, pos, true);
                litContainer.objects.push_back(GFXObject(rect, vector2d(x, y), true, -10));
                darkContainer.objects.push_back(GFXObject(rect2, vector2d(x, y), true, -10));
            } else if (tile.m_type == eTileType::Wall) {
                sf::Vector2f pos(x, y);
                pos = coordsToScreen(pos);
                pos.y -= cWallHeightOffset * 2 - 5;
                sf::RectangleShape* rect = createTop(texture, pos, false);
                sf::RectangleShape* rect2 = createTop(texture, pos, true);

                litContainer.objects.push_back(GFXObject(rect, vector2d(x, y), true, 1));
                darkContainer.objects.push_back(GFXObject(rect2, vector2d(x, y), true, 1));
                // culling
                // if (y == height - 1 || (*level)(x, y + 1).m_type != eTileType::Wall) {
                sf::ConvexShape* bottom = createBottom(texture, pos, false);
                sf::ConvexShape* bottom2 = createBottom(texture, pos, true);
                litContainer.objects.push_back(GFXObject(bottom, vector2d(x, y), true));
                darkContainer.objects.push_back(GFXObject(bottom2, vector2d(x, y), true));
                //}
                // if (x == width - 1 || (*level)(x + 1, y).m_type != eTileType::Wall) {
                sf::ConvexShape* right = createRight(texture, pos, false);
                sf::ConvexShape* right2 = createRight(texture, pos, true);

                litContainer.objects.push_back(GFXObject(right, vector2d(x, y), true));
                darkContainer.objects.push_back(GFXObject(right2, vector2d(x, y), true));
                //}
            }

            m_litData.push_back(litContainer);
            m_darkData.push_back(darkContainer);
            m_visibleData.push_back(false);
        }
    }
    assert(m_litData.size() == m_darkData.size());
    assert(m_litData.size() == m_visibleData.size());
}

void GFXLevel::run(const Level* level, vector2d center)
{
    if (level == nullptr) {
        return;
    }

    if (m_processedLevel != level) {
        regenerate(level);
        m_processedLevel = level;
    }

    for (unsigned i = 0; i < m_visibleData.size(); ++i) {
        m_visibleData[i] = false;
    }

    int width = level->getWidth();
    int height = level->getHeight();

    auto windowSize = Game::getWindow().getSize();

    sf::RectangleShape rect(sf::Vector2f(width * cWidth, height * cHeight));

    rect.setFillColor(sf::Color(11, 11, 11));
    sf::Vector2f pos(0, 0);
    rect.setTexture(&m_stone);
    rect.setTextureRect(sf::IntRect(0, 0, height * cWidth, height * cHeight));
    rect.setRotation(45.f);
    pos = coordsToScreen(pos);
    rect.setPosition(pos);
    m_ground.push(rect);

    unsigned range = 12;
    switch (level->getLighting()) {
    case eLighting::Cave:
        range = 12;
        break;
    case eLighting::Dark:
    case eLighting::Sunny:
        range = 20;
        break;
    }

    int minX = center.x - range;
    int minY = center.y - range;
    int maxX = center.x + range;
    int maxY = center.y + range;

    for (int i = 0; i < 360; i += 3) {
        float ox = center.x;
        float oy = center.y;
        float stepx = cos((float)i * 0.01745f);
        float stepy = sin((float)i * 0.01745f);
        for (int j = 0; j < range; j++) {
            int x = (int)ox;
            int y = (int)oy;
            Tile tile = (*level)(x, y);
            m_visibleData[x * height + y] = true;
            if (tile.m_type == eTileType::Wall) {
                break;
            }
            if (x > 0) {
                int tx = x - 1;
                m_visibleData[tx * height + y] = true;
            }
            if (x < width - 1) {
                int tx = x + 1;
                m_visibleData[tx * height + y] = true;
            }
            if (y > 0) {
                int ty = y - 1;
                m_visibleData[x * height + ty] = true;
            }
            if (y < height - 1) {
                int ty = y + 1;
                m_visibleData[x * height + ty] = true;
            }
            if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                break;
            }
            ox += stepx;
            oy += stepy;
        }
    }

    minX = max(0, minX);
    minY = max(0, minY);
    maxX = min(width, maxX);
    maxY = min(height, maxY);
    for (int x = minX; x < maxX; ++x) {
        for (int y = minY; y < maxY; ++y) {
            Tile tile = (*level)(x, y);
            int dist = (x - center.x) * (x - center.x) + (y - center.y) * (y - center.y);
            if (dist > range * range) {
                continue;
            }

            if (m_visibleData[x * height + y]) {
                for (auto object : m_litData[x * height + y].objects) {
                    object.getDraw();
                    m_queue.add(object);
                }
            } else {
                for (auto object : m_darkData[x * height + y].objects) {
                    object.getDraw();
                    m_queue.add(object);
                }
            }
        }
    }

    while (m_ground.empty() == false) {
        Game::getWindow().draw(m_ground.front());
        m_ground.pop();
    }

    const std::vector<Object*> rLevelObjs = level->getObjects();

    for (unsigned i = 0; i < rLevelObjs.size(); ++i) {
        vector2d position = rLevelObjs[i]->getPosition();
        int dist = (position.x - center.x) * (position.x - center.x)
            + (position.y - center.y) * (position.y - center.y);
        if (dist > 200) {
            continue;
        }

		// hide objects we can't see directly
		if (m_visibleData[position.x * height + position.y] == false) {
			continue;
		}
        sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(cWidth, cHeight));
        rect->setFillColor(sf::Color(99, 99, 99, 100));
        sf::RectangleShape* sprite = new sf::RectangleShape(sf::Vector2f(40, 100));
        sprite->setFillColor(sf::Color(190, 190, 190));
        sprite->setTexture(&m_texture);

        sf::Vector2f pos(position.x, position.y);
        pos = coordsToScreen(pos);
        rect->setPosition(pos);
        rect->setRotation(45.f);
        pos.y -= 60;
        pos.x -= 15;
        // please refactor me!
        // this code is not extensible and eosnt make sense
        if (rLevelObjs[i]->getObjectType() == eObjectTypes::Creature) {
            sf::Vector2f textPos(pos);
            sf::Text text;
            CreatureObject* creatureObject = static_cast<CreatureObject*>(rLevelObjs[i]);

            // don't color player
            if (creatureObject->isPlayer() == false) {
                int relationToPlayer = RelationManager::getSingleton()->getRelationship(
                    creatureObject->getFaction(), eCreatureFaction::Player);
                if (relationToPlayer <= RelationManager::cHostile) {
                    text.setFillColor(sf::Color(255, 100, 100));
                } else if (relationToPlayer >= RelationManager::cFriends) {
                    text.setFillColor(sf::Color(100, 255, 100));
                }
            }
            // hardcoded
            text.setCharacterSize(11);
            text.setFont(Game::getDefaultFont());
            text.setString(static_cast<const CreatureObject*>(rLevelObjs[i])->getName());
            textPos.x -= text.getLocalBounds().width / 2 - 15;
            text.setPosition(textPos);

            text.setScale(1, 2);
            m_texts.push(text);
        } else if (rLevelObjs[i]->getObjectType() == eObjectTypes::Campfire) {
            sprite->setTexture(&fire);
        }

        sprite->setPosition(pos);
        m_queue.add(GFXObject(rect, position, false, -1));

        m_queue.add(GFXObject(sprite, position));
    }

    m_queue.render();

    while (m_top.empty() == false) {
        Game::getWindow().draw(m_top.front());
        m_top.pop();
    }
}

void GFXLevel::renderText()
{
    while (m_texts.empty() == false) {
        Game::getWindow().draw(m_texts.front());
        m_texts.pop();
    }
}
