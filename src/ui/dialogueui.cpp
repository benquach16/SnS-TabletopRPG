#include <fstream>

#include "3rdparty/json.hpp"
#include "common.h"
#include "dialogueui.h"
#include "game.h"
#include "log.h"
#include "object/creatureobject.h"
#include "object/playerobject.h"
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
            responses = values["responses"].get<vector<string>>();
        }

        if (values["effects"].is_null() == false) {
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

void DialogueUI::initialize()
{
    auto windowSize = Game::getWindow().getSize();
    m_window = sfg::Window::Create();
    m_box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10);
    m_window->Add(m_box);
    m_label = sfg::Label::Create();
    m_box->Pack(m_label, false, false);
    m_window->SetStyle(m_window->GetStyle() ^ sfg::Window::RESIZE);
    m_window->SetStyle(m_window->GetStyle() ^ sfg::Window::TITLEBAR);
    Game::getSingleton()->getDesktop().Add(m_window);
    hide();
}

void DialogueUI::hide()
{
    m_window->Show(false);
    for (auto it : m_responses) {
        m_box->Remove(it);
    }
    m_responses.clear();
}

void DialogueUI::init(std::string startingLabel)
{
    m_currentLabel = startingLabel;
    m_currentState = eUiState::TalkingNPC;
}

bool DialogueUI::run(
    bool hasKeyEvents, sf::Event event, PlayerObject* player, CreatureObject* creature)
{
    m_window->SetPosition(
        sf::Vector2f(Game::getWindow().getSize().x / 2 - m_window->GetClientRect().width / 2,
            Game::getWindow().getSize().y / 2 - m_window->GetClientRect().height / 2));
    switch (m_currentState) {
    case eUiState::TalkingNPC:
        doTalkingNPC(player, creature);
        break;
    case eUiState::TalkingPlayer:
        m_window->Show();
        doTalkingPlayer(hasKeyEvents, event, player, creature);
        break;
    case eUiState::Finished:
        hide();
        return false;
        break;
    }
    return true;
}

void DialogueUI::doTalkingNPC(PlayerObject* player, CreatureObject* creature)
{
    UiCommon::drawTopPanel();
    hide();
    string message = m_dialogueTree.at(m_currentLabel)->getMessage();
    Log::push(
        creature->getCreatureComponent()->getName() + ": " + message, Log::eMessageTypes::Dialogue);
    m_currentState = eUiState::TalkingPlayer;
}

void DialogueUI::doTalkingPlayer(
    bool hasKeyEvents, sf::Event event, PlayerObject* player, CreatureObject* creature)
{
    std::vector<std::string> responses = m_dialogueTree.at(m_currentLabel)->getResponses();

    if (responses.size() == 0) {
        m_currentState = eUiState::Finished;
        hide();
        return;
    }
    string str;
    if (creature->isPlayer() == false) {
        m_label->SetText("Dialogue with " + creature->getName() + '\n');
    } else {
        m_label->SetText("Internal Monologue\n");
    }
    if (m_responses.size() == 0) {
        for (unsigned i = 0; i < responses.size(); ++i) {
            DialogueNode* node = m_dialogueTree[responses[i]];
            auto response = sfg::Button::Create(node->getMessage());
            response->SetRequisition(sf::Vector2f(0, 20));
            response->GetSignal(sfg::Button::OnLeftClick).Connect([this, node, player] {
                Log::push(player->getName() + ": " + node->getMessage(),
                    Log::eMessageTypes::OtherDialogue);
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
            });
            m_box->Pack(response, false, false);
            m_responses.push_back(response);
        }
    }

    m_currentState = eUiState::TalkingPlayer;
}
