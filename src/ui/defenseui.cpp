#include "defenseui.h"
#include "common.h"
#include "game.h"
#include "items/utils.h"
#include "types.h"

using namespace std;

void DefenseUI::run(bool hasKeyEvents, sf::Event event, Player* player, CombatInstance* instance)
{
    switch (m_currentState) {
    case eUiState::ChooseWeapon:
        doChooseWeapon(hasKeyEvents, event, player);
        break;
    case eUiState::ChooseManuever:
        doManuever(hasKeyEvents, event, player, instance);
        break;
    case eUiState::ChooseDice:
        doChooseDice(hasKeyEvents, event, player);
        break;
    case eUiState::Finished:
        break;
    default:
        assert(true);
        break;
    }
}

void DefenseUI::doChooseWeapon(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    UiCommon::initializeText(text);

    string str = "Choose weapon to defend with:\n";

    const Weapon* primaryWeapon = player->getPrimaryWeapon();
    const Weapon* secondaryWeapon = player->getSecondaryWeapon();

    str += "a - ";
    str += primaryWeapon->getName();
    if (player->primaryWeaponDisabled()) {
        str += " [Disabled!]";
    }
    str += '\n';
    str += "b - ";
    str += secondaryWeapon->getName();
    if (player->secondaryWeaponDisabled()) {
        str += " [Disabled!]";
    }

    str += '\n';
    text.setString(str);
    Game::getWindow().draw(text);

    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case 'a':
            if (player->primaryWeaponDisabled()) {
                Log::push("You cannot use a disabled weapon this tempo!");
            } else {
                player->setDefenseWeapon(true);
                m_currentState = eUiState::ChooseManuever;
            }

            break;
        case 'b':
            if (player->secondaryWeaponDisabled()) {
                Log::push("You cannot use a disabled weapon this tempo!");
            } else {
                player->setDefenseWeapon(false);
                m_currentState = eUiState::ChooseManuever;
            }
            break;
        }
    }
}

void DefenseUI::doManuever(
    bool hasKeyEvents, sf::Event event, Player* player, CombatInstance* instance)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());

    string str = "Choose defense:\n";

    map<eDefensiveManuevers, int> manuevers
        = getAvailableDefManuevers(player, player->getQueuedDefense().withPrimaryWeapon,
            instance->getLastTempo(), instance->getCurrentReach(), instance->getInGrapple());
    map<char, std::pair<eDefensiveManuevers, int>> indices;
    char idx = 'a';
    for (auto manuever : manuevers) {
        str += idx;
        str += " - ";
        str += defensiveManueverToString(manuever.first);
        if (manuever.second > 0) {
            str += " (" + to_string(manuever.second) + " AP)";
        }
        str += '\n';
        indices[idx] = pair<eDefensiveManuevers, int>(manuever.first, manuever.second);
        idx++;
    }

    text.setString(str);
    Game::getWindow().draw(text);
    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        auto iter = indices.find(c);

        if (iter != indices.end()) {
            auto cost = iter->second;
            if (player->getCombatPool() < cost.second && cost.second > 0) {
                Log::push(to_string(cost.second) + " AP needed.");
                return;
            }
            player->reduceCombatPool(cost.second);
            player->setDefenseManuever(cost.first);

            switch (cost.first) {
            case eDefensiveManuevers::StealInitiative:
            case eDefensiveManuevers::Parry:
            case eDefensiveManuevers::Dodge:
            case eDefensiveManuevers::ParryLinked:
            case eDefensiveManuevers::Expulsion:
            case eDefensiveManuevers::DodgeTakeInitiative:
            case eDefensiveManuevers::Counter:
            case eDefensiveManuevers::Reverse:
            case eDefensiveManuevers::Resist:
            case eDefensiveManuevers::Break:
                m_currentState = eUiState::ChooseDice;
                break;
            case eDefensiveManuevers::AttackFromDef:
                player->setDefenseReady();
                m_currentState = eUiState::Finished;
                break;
            default:
                m_currentState = eUiState::ChooseManuever;
                break;
            }
        }
    }
}

void DefenseUI::doChooseDice(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Allocate action points (" + std::to_string(player->getCombatPool())
        + " action points left):");
    Game::getWindow().draw(text);

    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {

        player->setDefenseDice(m_numberInput.getNumber());
        player->reduceCombatPool(m_numberInput.getNumber());
        m_currentState = eUiState::Finished;
        // last one so set flag
        player->setDefenseReady();
        m_numberInput.reset();
    }

    m_numberInput.setMax(player->getCombatPool());
    m_numberInput.run(hasKeyEvents, event);
    m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
}
