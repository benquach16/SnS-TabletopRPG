#include "offenseui.h"
#include "common.h"
#include "creatures/utils.h"
#include "game.h"
#include "items/utils.h"
#include "types.h"

using namespace std;

void OffenseUI::run(bool hasKeyEvents, sf::Event event, Player* player, Creature* target,
    const CombatInstance* instance, bool allowStealInitiative, bool payCosts)
{
    if (player->getHasOffense()) {
        return;
    }
    switch (m_currentState) {
    case eUiState::ChooseWeapon:
        doChooseWeapon(hasKeyEvents, event, player);
        break;
    case eUiState::ChooseManuever:
        doManuever(hasKeyEvents, event, player, instance);
        break;
    case eUiState::ChooseFeint:
        doFeint(hasKeyEvents, event, player);
        break;
    case eUiState::ChooseComponent:
        doComponent(hasKeyEvents, event, player);
        break;
    case eUiState::ChooseDice:
        doDice(hasKeyEvents, event, player);
        break;
    case eUiState::ChooseTarget:
        doTarget(hasKeyEvents, event, player, target, payCosts);
        break;
    case eUiState::InspectTarget:
        doInspect(hasKeyEvents, event, target);
        break;
    case eUiState::PinpointThrust:
        doPinpointThrust(hasKeyEvents, event, player);
        break;
    case eUiState::ChooseHeavyBlow:
        doHeavyBlow(hasKeyEvents, event, player);
        break;
    case eUiState::Finished:
        break;
    }
}

void OffenseUI::doChooseWeapon(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    UiCommon::initializeText(text);

    string str = "Choose weapon to attack with:\n";

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
                player->setOffenseWeapon(true);
                m_currentState = eUiState::ChooseManuever;
            }

            break;
        case 'b':
            if (player->secondaryWeaponDisabled()) {
                Log::push("You cannot use a disabled weapon this tempo!");
            } else {
                player->setOffenseWeapon(false);
                m_currentState = eUiState::ChooseManuever;
            }
            break;
        }
    }
}

void OffenseUI::doManuever(
    bool hasKeyEvents, sf::Event event, Player* player, const CombatInstance* instance)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    bool withPrimaryWeapon = player->getQueuedOffense().withPrimaryWeapon;
    map<eOffensiveManuevers, int> manuevers = getAvailableOffManuevers(player, withPrimaryWeapon,
        instance->getCurrentReach(), instance->getInGrapple(), true, false);

    string str = "Choose attack:\n";
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
    str += idx;
    str += " - Inspect Target";
    text.setString(str);
    Game::getWindow().draw(text);

    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        if (c == idx) {
            m_currentState = eUiState::InspectTarget;
            return;
        }
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
            case eOffensiveManuevers::Swing:
            case eOffensiveManuevers::PinpointThrust:
            case eOffensiveManuevers::Thrust:
            case eOffensiveManuevers::Beat:
            case eOffensiveManuevers::Hook:
            case eOffensiveManuevers::HeavyBlow:
            case eOffensiveManuevers::Disarm:
            case eOffensiveManuevers::Draw:
            case eOffensiveManuevers::VisorThrust:
            case eOffensiveManuevers::Throw:
            case eOffensiveManuevers::Snap:
                m_currentState = eUiState::ChooseComponent;
                break;
            case eOffensiveManuevers::Grab:
                m_currentState = eUiState::ChooseComponent;
                break;
            case eOffensiveManuevers::Mordhau: {
                const Weapon* weapon = player->getPrimaryWeapon();
                player->setOffenseManuever(eOffensiveManuevers::Mordhau);
                player->setOffenseComponent(weapon->getPommelStrike());

                m_currentState = eUiState::ChooseTarget;

                break;
            }
            case eOffensiveManuevers::NoOffense:
                player->setOffenseDice(0);
                player->setOffenseReady();
                m_currentState = eUiState::ChooseManuever;
                break;
            default:
                m_currentState = eUiState::ChooseManuever;
                break;
            }
        }
    }
}

void OffenseUI::doFeint(bool hasKeyEvents, sf::Event event, Player* player)
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
            m_currentState = eUiState::ChooseComponent;
        }
        if (c == 'b') {
            // TODO : REPLACE ME WITH CODE IN CREATURE TO REDUCE COMBAT POOL
            player->setOffenseFeint();
            player->reduceCombatPool(2);
            m_currentState = eUiState::ChooseComponent;
        }
    }
}

void OffenseUI::doHeavyBlow(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Heavy blow? (Can spend 0-2 AP)");
    Game::getWindow().draw(text);

    if (hasKeyEvents && event.type == sf::Event::KeyReleased
        && event.key.code == sf::Keyboard::Enter) {
        int dice = m_numberInput.getNumber();
        player->reduceCombatPool(dice);
        player->setOffenseHeavyDice(dice);
        m_numberInput.reset();
        m_currentState = eUiState::ChooseComponent;
    }

    constexpr int cMaxHeavyDice = 2;
    m_numberInput.setMax(min(player->getCombatPool(), cMaxHeavyDice));
    m_numberInput.run(hasKeyEvents, event);
    m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
}

void OffenseUI::doInspect(bool hasKeyEvents, sf::Event event, Creature* target)
{
    UiCommon::drawTopPanel();

    std::vector<const Armor*> armor = target->getArmor();
    string str;
    str += target->getName() + " is has armor coverage on \n";

    str += UiCommon::drawPaperdoll(target);
    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString(str);
    Game::getWindow().draw(text);
    if (hasKeyEvents && event.type == sf::Event::TextEntered) {
        m_currentState = eUiState::ChooseManuever;
    }
}

void OffenseUI::doComponent(bool hasKeyEvents, sf::Event event, Player* player)
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

void OffenseUI::doDice(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    text.setString("Allocate action points (" + std::to_string(player->getCombatPool())
        + " action points left):");

    Game::getWindow().draw(text);

    if (hasKeyEvents && event.type == sf::Event::KeyReleased
        && event.key.code == sf::Keyboard::Enter) {
        int dice = m_numberInput.getNumber();
        player->setOffenseDice(dice);
        player->reduceCombatPool(dice);
        m_currentState = eUiState::Finished;
        // last one so set flag
        player->setOffenseReady();
        m_numberInput.reset();
    }

    m_numberInput.setMax(player->getCombatPool());
    m_numberInput.run(hasKeyEvents, event);
    m_numberInput.setPosition(sf::Vector2f(0, cCharSize));
}

void OffenseUI::doTarget(bool hasKeyEvents, sf::Event event, Player* player, Creature* target, bool payCosts)
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

        if (hasKeyEvents && event.type == sf::Event::TextEntered) {
            char c = event.text.unicode;
            if (c == idx) {
                player->setOffenseTarget(locations[i]);
                if (payCosts) {
                    m_currentState = eUiState::ChooseDice;
                }
                else {
                    player->setOffenseReady();
                    m_currentState = eUiState::Finished;
                }
                
            }
            // if player chose pinpoit thrust allow them to pick a specific location
            if (player->getQueuedOffense().manuever == eOffensiveManuevers::PinpointThrust
                && m_currentState == eUiState::ChooseDice) {
                m_currentState = eUiState::PinpointThrust;
                return;
            }
        }
    }
    text.setString(str);
    Game::getWindow().draw(text);
}

void OffenseUI::doPinpointThrust(bool hasKeyEvents, sf::Event event, Player* player)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());

    string str = "Choose body part to pinpoint:\n";
    eHitLocations location = player->getQueuedOffense().target;
    bool canHitX = (player->getGrip() == eGrips::Staff || player->getGrip() == eGrips::HalfSword
        || player->getCurrentReach() == eLength::Hand);
    std::vector<eBodyParts> parts
        = WoundTable::getSingleton()->getPinpointThrustTargets(location, canHitX);

    for (int i = 0; i < parts.size(); ++i) {
        char idx = ('a' + i);

        str += idx;
        str += " - " + bodyPartToString(parts[i]) + '\n';

        if (hasKeyEvents && event.type == sf::Event::TextEntered) {
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
