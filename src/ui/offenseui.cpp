#include "offenseui.h"
#include "../creatures/utils.h"
#include "../game.h"
#include "common.h"
#include "types.h"

using namespace std;

void OffenseUI::run(
    sf::Event event, Player* player, Creature* target, bool allowStealInitiative, bool linkedParry)
{
    switch (m_currentState) {
    case eUiState::ChooseManuever:
        doManuever(event, player, linkedParry);
        break;
    case eUiState::ChooseFeint:
        doFeint(event, player);
        break;
    case eUiState::ChooseComponent:
        doComponent(event, player);
        break;
    case eUiState::ChooseDice:
        doDice(event, player);
        break;
    case eUiState::ChooseTarget:
        doTarget(event, player, linkedParry, target);
        break;
    case eUiState::InspectTarget:
        doInspect(event, target);
        break;
    case eUiState::PinpointThrust:
        doPinpointThrust(event, player);
        break;
    case eUiState::ChooseHeavyBlow:
        doHeavyBlow(event, player);
        break;
    case eUiState::Finished:
        break;
    }
}

void OffenseUI::doManuever(sf::Event event, Player* player, bool linkedParry)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    bool halfPrice = player->getGrip() == eGrips::Staff || player->getGrip() == eGrips::HalfSword;
    string str = "Choose attack:\na - Swing\nb - Thrust\nc - Pinpoint Thrust (";
    int pinpointCost = offenseManueverCost(eOffensiveManuevers::PinpointThrust);
    if (halfPrice == true) {
        str += "1";
        pinpointCost = 1;
    } else {
        str += to_string(pinpointCost);
    }

    str += "AP)\nd - Beat (1AP)\ne - Hook\nf - Wrap (1AP)\ng - Slam\nh - Mordhau(";

    int mordhauCost = offenseManueverCost(eOffensiveManuevers::Mordhau);
    bool freeMordhau = player->getGrip() == eGrips::HalfSword;
    if (freeMordhau == true) {
        str += "0";
        mordhauCost = 0;
    } else {
        str += to_string(mordhauCost);
    }
    str += "AP)\ni - Inspect Target";
    text.setString(str);
    Game::getWindow().draw(text);

    if (event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        switch (c) {
        case 'a':
            player->setOffenseManuever(eOffensiveManuevers::Swing);
            m_currentState = eUiState::ChooseHeavyBlow;
            break;
        case 'b':
            player->setOffenseManuever(eOffensiveManuevers::Thrust);
            m_currentState = eUiState::ChooseFeint;
            break;
        case 'c':
            if (player->getCombatPool() >= pinpointCost) {
                player->setOffenseManuever(eOffensiveManuevers::PinpointThrust);
                m_currentState = eUiState::ChooseFeint;
                player->reduceCombatPool(pinpointCost);
            } else {
                // need 2 dice
                Log::push(to_string(pinpointCost) + " AP needed.");
            }
            break;
        case 'e':
            player->setOffenseManuever(eOffensiveManuevers::Hook);
            m_currentState = eUiState::ChooseFeint;
            break;
        case 'h':
            if (player->getPrimaryWeapon()->getType() == eWeaponTypes::Swords
                || player->getPrimaryWeapon()->getType() == eWeaponTypes::Longswords) {

                if (player->getCombatPool() >= mordhauCost) {
                    player->reduceCombatPool(mordhauCost);
                    const Weapon* weapon = player->getPrimaryWeapon();
                    player->setOffenseManuever(eOffensiveManuevers::Mordhau);
                    player->setOffenseComponent(weapon->getPommelStrike());
                    player->setOffenseTarget(eHitLocations::Head);
                    if (linkedParry == true) {
                        player->setOffenseReady();
                        m_currentState = eUiState::Finished;
                    } else {
                        m_currentState = eUiState::ChooseDice;
                    }
                } else {
                    Log::push(to_string(mordhauCost) + " AP needed.");
                }
            } else {
                Log::push("You need a sword to use this manuever");
            }
            break;
        case 'i':
            m_currentState = eUiState::InspectTarget;
            break;
        default:
            break;
        }
    }
}

void OffenseUI::doFeint(sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Feint attack? (2AP)\na - No\nb - Yes");
    Game::getWindow().draw(text);

    if (event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        if (c == 'a') {
            m_currentState = eUiState::ChooseComponent;
        }
        if (c == 'b') {
            player->setOffenseFeint();
            player->reduceCombatPool(2);
            m_currentState = eUiState::ChooseComponent;
        }
    }
}

void OffenseUI::doHeavyBlow(sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Heavy blow? (Can spend 0-3 AP)");
    Game::getWindow().draw(text);

    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
        int dice = m_numberInput.getNumber();
        player->reduceCombatPool(dice);
        player->setOffenseHeavyDice(dice);
        m_numberInput.reset();
        m_currentState = eUiState::ChooseFeint;
    }

    constexpr int cMaxHeavyDice = 3;
    m_numberInput.setMax(min(player->getCombatPool(), cMaxHeavyDice));
    m_numberInput.run(event);
    m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
}

void OffenseUI::doInspect(sf::Event event, Creature* target)
{
    UiCommon::drawTopPanel();

    std::vector<const Armor*> armor = target->getArmor();
    string str;
    str += target->getName() + " is wearing ";
    for (int i = 0; i < armor.size(); ++i) {
        str += armor[i]->getName() + ", ";
    }
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString(str);
    Game::getWindow().draw(text);
    if (event.type == sf::Event::TextEntered) {
        m_currentState = eUiState::ChooseManuever;
    }
}

void OffenseUI::doComponent(sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    const Weapon* weapon = player->getPrimaryWeapon();
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    std::string str("Choose weapon component:\n");
    eGrips grip = player->getGrip();
    if (player->getQueuedOffense().manuever == eOffensiveManuevers::Swing
        || player->getQueuedOffense().manuever == eOffensiveManuevers::Hook) {
        for (int i = 0; i < weapon->getSwingComponents(grip).size(); ++i) {
            char idx = ('a' + i);

            str += idx;
            str += " - " + weapon->getSwingComponents(grip)[i]->getName() + '\n';

            if (event.type == sf::Event::TextEntered) {
                char c = event.text.unicode;
                if (c == idx) {
                    player->setOffenseComponent(weapon->getSwingComponents(grip)[i]);
                    m_currentState = eUiState::ChooseTarget;
                }
            }
        }
    } else {
        for (int i = 0; i < weapon->getThrustComponents(grip).size(); ++i) {
            char idx = ('a' + i);

            str += idx;
            str += " - " + weapon->getThrustComponents(grip)[i]->getName() + '\n';

            if (event.type == sf::Event::TextEntered) {
                char c = event.text.unicode;
                if (c == idx) {
                    player->setOffenseComponent(weapon->getThrustComponents(grip)[i]);
                    m_currentState = eUiState::ChooseTarget;
                }
            }
        }
    }

    text.setString(str);
    Game::getWindow().draw(text);
}

void OffenseUI::doDice(sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Allocate action points (" + std::to_string(player->getCombatPool())
        + " action points left):");

    Game::getWindow().draw(text);

    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
        int dice = m_numberInput.getNumber();
        player->setOffenseDice(dice);
        player->reduceCombatPool(dice);
        m_currentState = eUiState::Finished;
        // last one so set flag
        player->setOffenseReady();
        m_numberInput.reset();
    }

    m_numberInput.setMax(player->getCombatPool());
    m_numberInput.run(event);
    m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
}

void OffenseUI::doTarget(sf::Event event, Player* player, bool linkedParry, Creature* target)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());

    std::string str = "Choose target location:\n";
    const std::vector<eHitLocations> locations = target->getHitLocations();

    for (int i = 0; i < locations.size(); ++i) {
        char idx = ('a' + i);

        str += idx;
        str += " - " + hitLocationToString(locations[i]) + '\n';

        if (event.type == sf::Event::TextEntered) {
            char c = event.text.unicode;
            if (c == idx) {
                player->setOffenseTarget(locations[i]);
                m_currentState = eUiState::ChooseDice;
            }
            // if player chose pinpoit thrust allow them to pick a specific location
            if (player->getQueuedOffense().manuever == eOffensiveManuevers::PinpointThrust
                && m_currentState == eUiState::ChooseDice) {
                m_currentState = eUiState::PinpointThrust;
                return;
            }
            // the uistate comparision is a hacky way to repurpose it
            if (linkedParry == true && m_currentState == eUiState::ChooseDice) {
                player->setOffenseDice(0);
                m_currentState = eUiState::Finished;
                // linked parry so set flag
                player->setOffenseReady();
                return;
            }
        }
    }
    text.setString(str);
    Game::getWindow().draw(text);
}

void OffenseUI::doPinpointThrust(sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());

    string str = "Choose body part to pinpoint:\n";
    eHitLocations location = player->getQueuedOffense().target;

    std::vector<eBodyParts> parts = WoundTable::getSingleton()->getUniqueParts(location);

    for (int i = 0; i < parts.size(); ++i) {
        char idx = ('a' + i);

        str += idx;
        str += " - " + bodyPartToString(parts[i]) + '\n';

        if (event.type == sf::Event::TextEntered) {
            char c = event.text.unicode;
            if (c == idx) {
                player->setOffensePinpointTarget(parts[i]);
                m_currentState = eUiState::ChooseDice;
            }
        }
    }

    text.setString(str);
    Game::getWindow().draw(text);
}
