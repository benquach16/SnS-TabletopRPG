#pragma once

#include <SFML/Graphics.hpp>

class PlayerObject;
class CreatureObject;

class DialogueUI {
public:
    DialogueUI();
    void resetState() { m_currentState = eUiState::TalkingNPC; }
    void run(sf::Event event, PlayerObject* player, CreatureObject* creature);

private:
    void doTalkingNPC(sf::Event event, PlayerObject* player, CreatureObject* creature);
    void doTalkingPlayer(sf::Event event, PlayerObject* player, CreatureObject* creature);
    enum eUiState { TalkingNPC, TalkingPlayer, Finished };
    eUiState m_currentState;
};
