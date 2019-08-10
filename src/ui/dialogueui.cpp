#include "dialogueui.h"
#include "../game.h"
#include "../log.h"
#include "../object/creatureobject.h"
#include "../object/playerobject.h"
#include "common.h"
#include "types.h"

using namespace std;

DialogueUI::DialogueUI() { resetState(); }

void DialogueUI::run(sf::Event event, PlayerObject* player, CreatureObject* creature)
{
    switch (m_currentState) {
    case eUiState::TalkingNPC:
        doTalkingNPC(event, player, creature);
        break;
    case eUiState::TalkingPlayer:
        doTalkingPlayer(event, player, creature);
        break;
    case eUiState::Finished:
        break;
    }
}

void DialogueUI::doTalkingNPC(sf::Event event, PlayerObject* player, CreatureObject* creature)
{
    // UiCommon::drawTopPanel();

    Log::push(
        creature->getCreatureComponent()->getName() + ": Hello", Log::eMessageTypes::Dialogue);
    m_currentState = eUiState::TalkingPlayer;
}

void DialogueUI::doTalkingPlayer(sf::Event event, PlayerObject* player, CreatureObject* creature)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());
    string str = "Dialogue with " + creature->getCreatureComponent()->getName();
    str += "\na - I must leave now.";
    text.setString(str);
    Game::getWindow().draw(text);
    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::A) {
        Log::push(player->getCreatureComponent()->getName() + ": I must leave now",
            Log::eMessageTypes::Dialogue);
        m_currentState = eUiState::Finished;
        return;
    }
    m_currentState = eUiState::TalkingPlayer;
}
