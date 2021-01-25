#include "preresolveui.h"
#include "common.h"
#include "creatures/utils.h"
#include "game.h"
#include "items/utils.h"
#include "types.h"

using namespace std;

void PreresolveUI::run(bool hasKeyEvents, sf::Event event, Player* player, const Creature* target,
    const CombatInstance* instance)
{
    switch (m_currentState) {
    case eUiState::ChooseFeint:
        doFeint(hasKeyEvents, event, player);
        break;
    case eUiState::ChooseDice:
        doDice(hasKeyEvents, event, player);
        break;
    case eUiState::ChooseManuever:
        doManuever(hasKeyEvents, event, player, instance);
        break;
    case eUiState::ChooseTarget:
        doTarget(hasKeyEvents, event, player, target);
        break;
    case eUiState::ChooseComponent:
        doComponent(hasKeyEvents, event, player);
        break;
    case eUiState::Finished:
        break;
    }
}

void PreresolveUI::doFeint(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Feint attack? (2AP)\na - No\nb - Yes");
    Game::getWindow().draw(text);

    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        if (c == 'a') {
            player->setPreResolutionReady();
            m_currentState = eUiState::Finished;
        }
        if (c == 'b') {
            // TODO : REPLACE ME WITH CODE IN CREATURE TO REDUCE COMBAT POOL
            player->setOffenseFeint();
            player->reduceCombatPool(2);

            m_currentState = eUiState::ChooseManuever;
        }
    }
}

void PreresolveUI::doDice(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Choose amount of AP to invest in the feint:\n");
    Game::getWindow().draw(text);

    if (hasKeyEvents && event.type == sf::Event::KeyReleased
        && event.key.code == sf::Keyboard::Enter) {
        int dice = m_numberInput.getNumber();
        player->setOffenseFeintDice(dice);
        player->reduceCombatPool(dice);
        m_currentState = eUiState::Finished;
        // last one so set flag
        player->setPreResolutionReady();
        m_numberInput.reset();
    }

    m_numberInput.setMax(player->getCombatPool());
    m_numberInput.run(hasKeyEvents, event);
    m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
}

void PreresolveUI::doManuever(
    bool hasKeyEvents, sf::Event event, Player* player, const CombatInstance* instance)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    bool withPrimaryWeapon = player->getQueuedOffense().withPrimaryWeapon;
    map<eOffensiveManuevers, int> manuevers
        = getAvailableOffManuevers(player, withPrimaryWeapon, instance->getCurrentReach(),
            instance->getInGrapple(), false, true, instance->getLastTempo() == false);

    string str = "Choose new attack:\n";
    map<char, std::pair<eOffensiveManuevers, int>> indices;
    char idx = 'a';
    for (auto manuever : manuevers) {
        str += idx;
        str += " - ";
        str += offensiveManueverToString(manuever.first);
        if (manuever.second > 0) {
            str += " (" + to_string(manuever.second) + " AP)";
        }
        str += '\n';
        indices[idx] = pair<eOffensiveManuevers, int>(manuever.first, manuever.second);
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
            player->setOffenseManuever(cost.first);

            switch (cost.first) {
            default:
                m_currentState = eUiState::ChooseComponent;
                break;
            }
        }
    }
}

void PreresolveUI::doTarget(
    bool hasKeyEvents, sf::Event event, Player* player, const Creature* target)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());

    std::string str = "Choose new target location:\n";
    map<eHitLocations, int> locations
        = getHitLocationCost(target, true, player->getQueuedOffense().target);

    map<char, std::pair<eHitLocations, int>> indices;
    char idx = 'a';
    for (auto location : locations) {
        str += idx;
        str += " - ";
        str += hitLocationToString(location.first);
        if (location.second > 0) {
            str += " (" + to_string(location.second) + " AP)";
        }
        str += '\n';
        indices[idx] = pair<eHitLocations, int>(location.first, location.second);
        idx++;
    }

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
            player->setOffenseTarget(cost.first);
            m_currentState = eUiState::ChooseDice;
        }
    }
    text.setString(str);
    Game::getWindow().draw(text);
}

// shared code with offense ui, needs to be abstracted and refactored
void PreresolveUI::doComponent(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    const Weapon* weapon = player->getQueuedOffense().withPrimaryWeapon == true
        ? player->getPrimaryWeapon()
        : player->getSecondaryWeapon();
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    std::string str("Choose weapon component:\n");
    switch (player->getQueuedOffense().manuever) {
    case eOffensiveManuevers::Swing:
    case eOffensiveManuevers::Draw:
    case eOffensiveManuevers::HeavyBlow:
    case eOffensiveManuevers::Beat:
    case eOffensiveManuevers::Hook: {
        for (int i = 0; i < weapon->getSwingComponents().size(); ++i) {
            char idx = ('a' + i);

            str += idx;
            str += " - " + weapon->getSwingComponents()[i]->getName() + '\n';

            if (hasKeyEvents && event.type == sf::Event::TextEntered) {
                char c = event.text.unicode;
                if (c == idx) {
                    player->setOffenseComponent(weapon->getSwingComponents()[i]);
                    m_currentState = eUiState::ChooseTarget;
                }
            }
        }
    } break;

    default: {
        for (int i = 0; i < weapon->getThrustComponents().size(); ++i) {
            char idx = ('a' + i);

            str += idx;
            str += " - " + weapon->getThrustComponents()[i]->getName() + '\n';

            if (hasKeyEvents && event.type == sf::Event::TextEntered) {
                char c = event.text.unicode;
                if (c == idx) {
                    player->setOffenseComponent(weapon->getThrustComponents()[i]);
                    m_currentState = eUiState::ChooseTarget;
                }
            }
        }
    } break;
    }

    text.setString(str);
    Game::getWindow().draw(text);
}
