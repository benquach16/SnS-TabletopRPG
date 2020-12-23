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
{
    fire.loadFromFile("data/textures/fire.png");
    fire.setSmooth(true);
    m_texture.loadFromFile("data/textures/test.png");
    m_texture.setSmooth(true);
    m_stone.loadFromFile("data/textures/rough.png");
    m_stone.setRepeated(true);
    m_grass.loadFromFile("data/textures/grass.png");
    m_grass.setRepeated(true);
}

void GFXLevel::run(const Level* level, vector2d center)
{
    if (level == nullptr) {
        return;
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

    constexpr unsigned cRange = 12;
    int minX = center.x - cRange;
    int minY = center.y - cRange;
    int maxX = center.x + cRange;
    int maxY = center.y + cRange;
    minX = max(0, minX);
    minY = max(0, minY);
    maxX = min(width, maxX);
    maxY = min(height, maxY);
    for (int x = minX; x < maxX; ++x) {
        for (int y = minY; y < maxY; ++y) {
            Tile tile = (*level)(x, y);
            int dist = (x - center.x) * (x - center.x) + (y - center.y) * (y - center.y);
            if (dist > cRange * cRange) {
                continue;
            }
            sf::Texture* texture = &m_stone;
            switch (tile.m_material) {
            case eTileMaterial::Grass:
                texture = &m_grass;
                break;
            }
            int x0 = center.x;
            int y0 = center.y;
            int x1 = x;
            int y1 = y;

            if (tile.m_type == eTileType::Ground) {
                // this code is really slow and unncessary

                sf::RectangleShape rect(sf::Vector2f(cWidth, cHeight));
                rect.setFillColor(sf::Color(55, 55, 55));
                sf::Vector2f pos(x, y);

                rect.setTexture(texture);
                rect.setRotation(45.f);
                pos = coordsToScreen(pos);
                rect.setPosition(pos);
                m_ground.push(rect);

            } else if (tile.m_type == eTileType::Wall) {
                sf::RectangleShape rect(sf::Vector2f(cWidth, cWidth));
                rect.setFillColor(sf::Color(22, 22, 22));
                sf::Vector2f pos(x, y);
                rect.setTexture(texture);
                rect.setRotation(45.f);
                pos = coordsToScreen(pos);
                pos.y -= cWallHeightOffset * 2 - 5;
                rect.setPosition(sf::Vector2f(pos.x, pos.y - (cWallHeightOffset - 1)));
                m_top.push(rect);

                sf::ConvexShape* bottom = new sf::ConvexShape(4);
                bottom->setFillColor(sf::Color(33, 33, 33));
                bottom->setTexture(texture);
                sf::Vector2f bottomPos(pos);
                bottomPos.x -= cWallWidthOffset;
                bottomPos.y += cCos45;
                bottom->setPosition(bottomPos);
                bottom->setPoint(0, sf::Vector2f(0, 0));
                bottom->setPoint(1, sf::Vector2f(cWallWidthOffset, cWallHeightOffset));
                bottom->setPoint(
                    2, sf::Vector2f(cWallWidthOffset, cHeight * 2 + (cWallHeightOffset)));
                bottom->setPoint(3, sf::Vector2f(0, cHeight * 2));
                m_queue.add(GFXObject(bottom, vector2d(x, y)));

                sf::ConvexShape* right = new sf::ConvexShape(4);
                right->setFillColor(sf::Color(66, 66, 66));
                right->setTexture(texture);
                sf::Vector2f rightPos(pos);
                // rightPos.x+=cWidth*cCos45;
                rightPos.y += cWallHeightOffset;
                right->setPosition(rightPos);
                right->setPointCount(4);
                right->setPoint(0, sf::Vector2f(0, 0));
                right->setPoint(1, sf::Vector2f(cWallWidthOffset - 1, -(cWallHeightOffset)));
                right->setPoint(
                    2, sf::Vector2f(cWallWidthOffset - 1, cHeight * 2 - (cWallHeightOffset)));
                right->setPoint(3, sf::Vector2f(0, cHeight * 2));
                m_queue.add(GFXObject(right, vector2d(x, y)));
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

        sf::RectangleShape rect(sf::Vector2f(cWidth, cHeight));
        rect.setFillColor(sf::Color(99, 99, 99, 100));
        sf::RectangleShape* sprite = new sf::RectangleShape(sf::Vector2f(40, 100));
        sprite->setFillColor(sf::Color(190, 190, 190));
        sprite->setTexture(&m_texture);

        sf::Vector2f pos(position.x, position.y);
        pos = coordsToScreen(pos);
        rect.setPosition(pos);
        rect.setRotation(45.f);
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
        Game::getWindow().draw(rect);

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
