#include <filesystem>

#include "../game.h"
#include "common.h"
#include "mainmenuui.h"
#include "types.h"

using namespace std;

MainMenuUI::MainMenuUI() {}

void MainMenuUI::initialize()
{
    UiCommon::drawTopPanel();
    sfg::Button::Ptr newgameBtn;
    sfg::Button::Ptr loadgameBtn;
    sfg::Button::Ptr arenaBtn;
    sfg::Window::Ptr window = sfg::Window::Create();
    window->SetStyle(window->GetStyle() ^ sfg::Window::RESIZE);
    window->SetStyle(window->GetStyle() ^ sfg::Window::TITLEBAR);
    sfg::Box::Ptr box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 15);
    newgameBtn = sfg::Button::Create();
    newgameBtn->SetLabel("New Game");
    newgameBtn->SetRequisition(sf::Vector2f(200, 30));
    loadgameBtn = sfg::Button::Create();
    loadgameBtn->SetLabel("Load Game");
    loadgameBtn->SetRequisition(sf::Vector2f(200, 30));
    arenaBtn = sfg::Button::Create();
    arenaBtn->SetLabel("Arena");
    arenaBtn->SetRequisition(sf::Vector2f(200, 30));
    auto label = sfg::Label::Create("<Insert Game Name Here>\n");
    newgameBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this] {
        Game::getSingleton()->setupNewgame();
        Game::getSingleton()->setState(Game::eApplicationState::CharCreation);
        hide();
    });
    loadgameBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this] {
        // this is awful as it is a file system access every frame
        bool existingSave = std::filesystem::exists(Game::cSaveString);
        auto widget = sfg::Context::Get().GetActiveWidget();
        if (existingSave) {
            Game::getSingleton()->load(Game::cSaveString);

            hide();
        } else {
            sfg::Window::Ptr popupWindow = sfg::Window::Create();
            auto popupLayout = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5);
            auto exitBtn = sfg::Button::Create("OK");

            popupWindow->SetStyle(popupWindow->GetStyle() ^ sfg::Window::RESIZE);
            popupWindow->SetStyle(popupWindow->GetStyle() ^ sfg::Window::TITLEBAR);
            popupWindow->Add(popupLayout);
            auto label = sfg::Label::Create("No save game found!");
            popupLayout->Pack(label);
            popupLayout->Pack(exitBtn);
            popupWindow->SetPosition(sf::Vector2f(
                Game::getWindow().getSize().x / 2 - popupWindow->GetClientRect().width / 2,
                Game::getWindow().getSize().y / 2 - popupWindow->GetClientRect().height / 2));
            Game::getSingleton()->getDesktop().Add(popupWindow);
            hide();
            exitBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this] {
                show();
                auto widget = sfg::Context::Get().GetActiveWidget();

                while (widget->GetName() != "Window") {
                    widget = widget->GetParent();
                }
                // Remove window from desktop.
                Game::getSingleton()->getDesktop().Remove(widget);
                // Game::getSingleton()->deleteWidget(window);
            });
        }
    });
    arenaBtn->GetSignal(sfg::Button::OnLeftClick).Connect([this] {
        Game::getSingleton()->setupArena();
        Game::getSingleton()->setState(Game::eApplicationState::CharCreation);
        hide();
    });
    box->Pack(label, false, false);
    box->Pack(newgameBtn, false, false);
    box->Pack(loadgameBtn, false, false);
    box->Pack(arenaBtn, false, false);
    window->Add(box);
    Game::getSingleton()->getDesktop().Add(window);
    m_window = window;
}

void MainMenuUI::show() { m_window->Show(); }
void MainMenuUI::hide() { m_window->Show(false); }

void MainMenuUI::cleanup()
{
    // shared ptr has to have lifetime managed like this, otherwise get dangling ptrs
    m_window.reset();
}

void MainMenuUI::run(bool hasKeyEvents, sf::Event event)
{
    m_window->SetPosition(
        sf::Vector2f(Game::getWindow().getSize().x / 2 - m_window->GetClientRect().width / 2,
            Game::getWindow().getSize().y / 2 - m_window->GetClientRect().height / 2));
}
