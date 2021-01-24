#include <vector>

#include "../creatures/creature.h"
#include "../creatures/utils.h"
#include "../game.h"
#include "common.h"
#include "types.h"

constexpr int cHeight = 14;

using namespace std;

namespace UiCommon {

void drawTopPanel()
{
    auto windowSize = Game::getWindow().getSize();
    constexpr unsigned cBorderWidth = 2;
    static sf::RectangleShape bkg(
        sf::Vector2f(windowSize.x - (cBorderWidth * 2), cCharSize * cHeight));
    bkg.setFillColor(sf::Color(12, 12, 23));
    bkg.setOutlineThickness(cBorderWidth);
    bkg.setOutlineColor(sf::Color(22, 22, 33));
    bkg.setPosition(sf::Vector2f(cBorderWidth, 0));
    Game::getWindow().draw(bkg);
}

void initializeText(sf::Text& text)
{
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
}

string drawPaperdoll(const Creature* creature)
{
    string str;

    vector<eHitLocations> hitlocations = creature->getUnmodifiedHitLocations();
    for (auto location : hitlocations) {

        vector<eBodyParts> parts = WoundTable::getSingleton()->getUniqueParts(location);
        str += "[" + hitLocationToString(location) + "]\n";

        for (auto part : parts) {
            // ignore the secondpart arm/head
            if (part != eBodyParts::SecondLocationArm && part != eBodyParts::SecondLocationHead) {
                ArmorSegment segment = creature->getArmorAtPart(part);
                str += bodyPartToString(part);
                str += ": ";
                str += to_string(segment.AV);
                if (segment.isMetal) {
                    str += "M";
                }

                switch (segment.type) {
                case eArmorTypes::Maille:
                    str += " Maille";
                    break;
                case eArmorTypes::Plate:
                    str += " Plate";
                    break;
                }
            }
            str += ", ";
        }

        str += "\n";
    }
    return str;
}

} // namespace UiCommon
