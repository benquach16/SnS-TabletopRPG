#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class PlayerObject;
class CreatureObject;

class DialogueNode {
public:
    DialogueNode(const std::string& message, const std::vector<std::string>& responses);
    const std::vector<std::string>& getResponses() const { return m_responses; }
    std::string getMessage() const { return m_message; }

private:
    DialogueNode();
    std::string m_message;
    // key into dialogue tree
    std::vector<std::string> m_responses;
};

class DialogueUI {
public:
    DialogueUI();
    ~DialogueUI();
    void resetState() { m_currentState = eUiState::TalkingNPC; }
    bool run(bool hasKeyEvents, sf::Event event, PlayerObject* player, CreatureObject* creature);
    void init(std::string startingLabel);

private:
    void doTalkingNPC(PlayerObject* player, CreatureObject* creature);
    void doTalkingPlayer(
        bool hasKeyEvents, sf::Event event, PlayerObject* player, CreatureObject* creature);
    enum eUiState { TalkingNPC, TalkingPlayer, Finished };
    eUiState m_currentState;
    std::string m_currentLabel;

    std::unordered_map<std::string, DialogueNode*> m_dialogueTree;
};
