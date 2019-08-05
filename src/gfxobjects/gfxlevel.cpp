#include "gfxlevel.h"
#include "../game.h"
#include "../level/level.h"
#include "utils.h"

using namespace std;

sf::Texture test;

// rounding
constexpr int cWallWidthOffset = cWidth * cCos45 + 1;
constexpr int cWallHeightOffset = cHeight * cCos45;

GFXLevel::GFXLevel()
{
    m_texture.loadFromFile("data/textures/test.png");
    m_texture.setSmooth(true);
    test.loadFromFile("data/textures/rough.png");
    test.setRepeated(true);
}

void GFXLevel::run(const Level* level, vector2d center)
{
    if (level == nullptr) {
        return;
    }

    int width = level->getWidth();
    int height = level->getHeight();

    auto windowSize = Game::getWindow().getSize();

    std::queue<sf::RectangleShape> m_ground;
    std::queue<sf::RectangleShape> m_top;

    sf::RectangleShape rect(sf::Vector2f(width * cWidth, height * cHeight));
    rect.setFillColor(sf::Color(11, 11, 11));
    sf::Vector2f pos(0, 0);

    rect.setTexture(&test);
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

            int x0 = center.x;
            int y0 = center.y;
            int x1 = x;
            int y1 = y;

            // at some point calc fov
            /*

                        bool steep = (abs(y1 -y0) > abs(x1 - x0));
                        if(steep == true) {
                                swap(x0, y0);
                                swap(x1, y1);
                        }

                        int dx = x1 - x0;
                        int dy = abs(y1 - y0);

                        float error = dx / 2.0f;
                        int ystep = (y0 < y1) ? 1 : -1;
                        int yprime = y0;
                        bool render = true;
                        for(int xprime = x0; xprime < x1; ++xprime) {
                                if(steep) {
                                        sf::RectangleShape
               rect(sf::Vector2f(cWidth,cHeight));
                                        rect.setFillColor(sf::Color(55, 155,
               55)); sf::Vector2f pos(yprime, xprime); if(xprime > x0 &&
               (*level)(yprime - ystep, xprime-1).m_type == eTileType::Wall) {
                                                render = false;
                                        }
                                        rect.setRotation(45.f);
                                        pos = coordsToScreen(pos);
                                        rect.setPosition(pos);
                                        Game::getWindow().draw(rect);
                                } else {
                                        sf::RectangleShape
               rect(sf::Vector2f(cWidth,cHeight));
                                        rect.setFillColor(sf::Color(55, 155,
               55)); sf::Vector2f pos(xprime, yprime); if(xprime > x0 &&
               (*level)(xprime - 1, yprime).m_type == eTileType::Wall) { render
               =false;
                                        }
                                        rect.setRotation(45.f);
                                        pos = coordsToScreen(pos);
                                        rect.setPosition(pos);
                                        Game::getWindow().draw(rect);
                                }
                                error -= dy;
                                if(error < 0) {
                                        yprime += ystep;
                                        error += dx;
                                }
                        }
                        if(render == false) {
                                continue;
                        }
                        */

            if (tile.m_type == eTileType::Ground) {
                // this code is really slow and unncessary

                sf::RectangleShape rect(sf::Vector2f(cWidth, cHeight));
                rect.setFillColor(sf::Color(55, 55, 55));
                sf::Vector2f pos(x, y);

                rect.setTexture(&test);
                rect.setRotation(45.f);
                pos = coordsToScreen(pos);
                rect.setPosition(pos);
                m_ground.push(rect);

            } else if (tile.m_type == eTileType::Wall) {
                sf::RectangleShape rect(sf::Vector2f(cWidth, cWidth));
                rect.setFillColor(sf::Color(22, 22, 22));
                sf::Vector2f pos(x, y);
                rect.setTexture(&test);
                rect.setRotation(45.f);
                pos = coordsToScreen(pos);
                pos.y -= cWallHeightOffset * 2 - 5;
                rect.setPosition(sf::Vector2f(pos.x, pos.y - (cWallHeightOffset - 1)));
                m_top.push(rect);
                /*
                                sf::ConvexShape left;
                                left.setFillColor(sf::Color(22, 22, 22));
                                left.setTexture(&test);
                                sf::Vector2f leftPos(pos);
                                leftPos.x-=cWidth*cCos45;
                                left.setPosition(leftPos);
                                left.setPointCount(4);
                                left.setPoint(0, sf::Vector2f(0, 0));
                                left.setPoint(1, sf::Vector2f(cWidth * cCos45,
                   -(cHeight*cCos45))); left.setPoint(2, sf::Vector2f(cWidth *
                   cCos45, cHeight*2 - (cHeight*cCos45))); left.setPoint(3,
                   sf::Vector2f(0, cHeight*2)); m_wall.push(left);

                                sf::ConvexShape top;
                                top.setFillColor(sf::Color(33, 33, 33, 150));
                                top.setTexture(&test);
                                sf::Vector2f topPos(pos);
                                //pos.x-=cWidth*cCos45;
                                topPos.y-=40 * cCos45;
                                top.setPosition(topPos);
                                top.setPointCount(4);
                                top.setPoint(0, sf::Vector2f(0, 0));
                                top.setPoint(1, sf::Vector2f(cWidth * cCos45,
                   cHeight*cCos45)); top.setPoint(2, sf::Vector2f(cWidth *
                   cCos45, cHeight*2 + (cHeight*cCos45))); top.setPoint(3,
                   sf::Vector2f(0, cHeight*2)); m_wall.push(top);
*/
                sf::ConvexShape* bottom = new sf::ConvexShape(4);
                bottom->setFillColor(sf::Color(33, 33, 33));
                bottom->setTexture(&test);
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
                right->setTexture(&test);
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

    std::queue<sf::RectangleShape> m_chars;
    for (int i = 0; i < rLevelObjs.size(); ++i) {
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
