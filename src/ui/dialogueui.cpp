#include <fstream>

#include "../3rdparty/json.hpp"
#include "../game.h"
#include "../log.h"
#include "../object/creatureobject.h"
#include "../object/playerobject.h"
#include "common.h"
#include "dialogueui.h"
#include "types.h"

using namespace std;

const string filepath = "data/dialogue.json";

DialogueNode::DialogueNode(const std::string& message, const std::vector<std::string>& responses)
    : m_message(message)
    , m_responses(responses)
{
}

DialogueUI::DialogueUI()
{
    resetState();

    ifstream file(filepath);

    nlohmann::json parsedDialogue;
    file >> parsedDialogue;

    for (auto& iter : parsedDialogue.items()) {
        string key = iter.key();

        auto values = iter.value();

        assert(values["message"].is_null() == false);

        string message = values["message"];
        vector<string> responses;

        if (values["responses"].is_null() == false) {
            responses = static_cast<vector<string>>(values["responses"]);
        }

        assert(m_dialogueTree.find(key) == m_dialogueTree.end());

        m_dialogueTree[key] = new DialogueNode(message, responses);
    }
}

DialogueUI::~DialogueUI()
{
    for (auto it : m_dialogueTree) {
        delete it.second;
    }
}

void DialogueUI::init(std::string startingLabel)
{
    m_currentLabel = startingLabel;
    m_currentState = eUiState::TalkingNPC;
}

bool DialogueUI::run(sf::Event event, PlayerObject* player, CreatureObject* creature)
{
    switch (m_currentState) {
    case eUiState::TalkingNPC:
        doTalkingNPC(event, player, creature);
        break;
    case eUiState::TalkingPlayer:
        doTalkingPlayer(event, player, creature);
        break;
    case eUiState::Finished:
        return false;
        break;
    }
    return true;
}

void DialogueUI::doTalkingNPC(sf::Event event, PlayerObject* player, CreatureObject* creature)
{
    // UiCommon::drawTopPanel();
    string message = m_dialogueTree.at(m_currentLabel)->getMessage();
    Log::push(
        creature->getCreatureComponent()->getName() + ": " + message, Log::eMessageTypes::Dialogue);
    m_currentState = eUiState::TalkingPlayer;
}

void DialogueUI::doTalkingPlayer(sf::Event event, PlayerObject* player, CreatureObject* creature)
{
    UiCommon::drawTopPanel();

    sf::Text text;
    text.setCharacterSize(cCharSize);
    text.setFont(Game::getDefaultFont());

    std::vector<std::string> responses = m_dialogueTree.at(m_currentLabel)->getResponses();

    if (responses.size() == 0) {
        m_currentState = eUiState::Finished;
        return;
    }

    string str = "Dialogue with " + creature->getName() + '\n';
    for (unsigned i = 0; i < responses.size(); ++i) {
        char idx = 'a' + i;
        str += idx;
        DialogueNode* node = m_dialogueTree[responses[i]];

        str += " - " + node->getMessage() + "\n";

        if (event.type == sf::Event::TextEntered) {
            char c = event.text.unicode;
            if (c == idx) {
                Log::push(
                    player->getName() + ": " + node->getMessage(), Log::eMessageTypes::Dialogue);

                vector<string> chosenResponses = node->getResponses();
                if (chosenResponses.size() == 0) {
                    m_currentState = eUiState::Finished;
                    return;
                }
                // player responses should only have 1 follow up label
                assert(chosenResponses.size() == 1);
                m_currentLabel = chosenResponses[0];
                m_currentState = eUiState::TalkingNPC;
                return;
            }
        }
    }
    text.setString(str);
    Game::getWindow().draw(text);
    m_currentState = eUiState::TalkingPlayer;
}
