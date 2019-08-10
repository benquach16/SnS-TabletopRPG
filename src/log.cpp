#include "log.h"
#include "game.h"
#include "ui/types.h"
#include <algorithm>
#include <iostream>

using namespace std;

std::deque<Log::message> Log::m_queue = std::deque<Log::message>();

void Log::push(const std::string& str, eMessageTypes type)
{
    // split if the string is too long
    auto windowSize = Game::getWindow().getSize();
    int width = str.size() * cCharWidth;
    if (width > windowSize.x) {
        int diff = width - windowSize.x;
        int count = diff / cCharWidth;
        m_queue.push_back({ str.substr(0, count), type });
        m_queue.push_back({ str.substr(count, str.size() - count), type });
    } else {
        m_queue.push_back({ str, type });
    }
}

void Log::run()
{
    auto windowSize = Game::getWindow().getSize();
    const unsigned maxHistory = (windowSize.y - 100) / cCharSize - 2;
    unsigned rectHeight = cCharSize * (cLinesDisplayed + 1);
    // magic numbers
    static sf::RectangleShape logBkg(sf::Vector2f(windowSize.x - 6, rectHeight - 3));
    logBkg.setPosition(3, windowSize.y - rectHeight);

    if (m_queue.size() > maxHistory) {
        unsigned difference = m_queue.size() - maxHistory;
        for (unsigned i = 0; i < difference; ++i) {
            m_queue.pop_front();
        }
    }

    logBkg.setFillColor(sf::Color(12, 12, 23));
    logBkg.setOutlineThickness(3);
    logBkg.setOutlineColor(sf::Color(22, 22, 33));

    Game::getWindow().draw(logBkg);

    // only display last cLinesDisplayed elements of m_queue
    unsigned size = min(static_cast<unsigned>(m_queue.size()), cLinesDisplayed);
    for (unsigned i = 0; i < size; ++i) {
        unsigned index
            = m_queue.size() > cLinesDisplayed ? (i + (m_queue.size() - cLinesDisplayed)) : i;

        sf::Text text = createLogText(m_queue[index].text, m_queue[index].type);

        // i needs to be from 0 - cLinesDisplayed to format properly
        text.setPosition(
            cBorder, windowSize.y - ((cCharSize * (cLinesDisplayed + 1)) - i * cCharSize));
        Game::getWindow().draw(text);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
        // lots of magic numbers
        constexpr int cSpace = 50;
        sf::RectangleShape historyBkg(sf::Vector2f(windowSize.x - cSpace, windowSize.y - cSpace));
        historyBkg.setPosition(cSpace / 2, cSpace / 2);
        historyBkg.setFillColor(sf::Color(12, 12, 23));
        historyBkg.setOutlineThickness(3);
        historyBkg.setOutlineColor(sf::Color(22, 22, 33));

        Game::getWindow().draw(historyBkg);

        for (int i = 0; i < m_queue.size(); ++i) {
            sf::Text text = createLogText(m_queue[i].text, m_queue[i].type);
            text.setPosition(cSpace / 2, cSpace / 2 + (i * cCharSize));
            Game::getWindow().draw(text);
        }
    }
}

sf::Text Log::createLogText(const std::string& str, eMessageTypes type)
{
    sf::Text text;
    text.setString(str);

    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    switch (type) {
    case eMessageTypes::Announcement:
        text.setFillColor(sf::Color::Yellow);
        break;
    case eMessageTypes::Standard:
        text.setFillColor(sf::Color::White);
        break;
    case eMessageTypes::Alert:
        text.setFillColor(sf::Color::Cyan);
        break;
    case eMessageTypes::Damage:
        text.setFillColor(sf::Color::Red);
        text.setStyle(sf::Text::Bold);
        break;
    case eMessageTypes::Dialogue:
        text.setFillColor(sf::Color::Green);
        break;
    }
    return text;
}
