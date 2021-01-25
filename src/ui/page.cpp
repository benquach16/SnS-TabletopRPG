#include "page.h"
#include "common.h"
#include "creatures/creature.h"
#include "game.h"
#include "types.h"
#include "utils.h"
#include "items/utils.h"

using namespace std;

constexpr unsigned cMaxDisplay = 25;

Page::Page()
    : m_begin(0)
    , m_end(cMaxDisplay)
{
}

int Page::run(bool hasKeyEvents, sf::Event event, std::map<int, int>& inventory,
    sf::Vector2f position, bool readOnly, Creature* creature, bool useFilter, eItemType filter)
{
    sf::Text txt;
    txt.setPosition(position);
    UiCommon::initializeText(txt);
    string str;
    int count = 0;
    int current = 0;

    if (m_begin > 0) {
        str += "<Less>\n";
    }
    for (auto it = inventory.begin(); it != inventory.end();) {
        const Item* item = ItemTable::getSingleton()->get(it->first);
        eItemType type = item->getItemType();
        if (useFilter && type != filter) {
            // special case to ignore weapons / armor
            if (filter != eItemType::Misc) {
                it++;
                continue;
            } else if (type == eItemType::Weapon || type == eItemType::Armor) {
                it++;
                continue;
            }
        }
        if (current >= m_begin && current < m_end) {
            if (it->second <= 0) {
                it = inventory.erase(it);
            } else {
                char idx = ('a' + count);
                if (readOnly == false) {
                    str += idx;
                    str += " - ";
                }
                // str += " - [" + itemTypeToString(item->getItemType()) + "] ";
                str += item->getName() + " x" + to_string(it->second);

                if (creature != nullptr) {
                    bool equipped = creature->getNumEquipped(it->first) > 0;
                    if (equipped) {
                        str += " - Equipped";
                        if (creature->getPrimaryWeaponId() == it->first) {
                            str += " as Primary Weapon";
                        }
                        if (creature->getSecondaryWeaponId() == it->first) {
                            str += " as Secondary Weapon";
                        }
                        if (creature->findInQuickdraw(it->first)) {
                            str += " on belt";
                        }
                    }
                }

                str += '\n';

                count++;

                if (readOnly == false && hasKeyEvents && event.type == sf::Event::TextEntered) {
                    char c = event.text.unicode;
                    if (c == idx) {
                        return it->first;
                    }
                }
                it++;
            }
        } else {
            it++;
        }
        current++;
    }
    if (m_end < current) {
        str += "<More>\n";
    }
    txt.setString(str);
    Game::getWindow().draw(txt);
    if (hasKeyEvents && event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::Down) {
            if (m_end < current) {
                m_begin++;
                m_end++;
            }
        }
        if (event.key.code == sf::Keyboard::Up) {
            if (m_begin > 0) {
                m_begin--;
                m_end--;
            }
        }
    }
    return -1;
}
