#include "log.h"
#include "game.h"
#include "ui/types.h"
#include <algorithm>
#include <iostream>

using namespace std;

sfg::ScrolledWindow::Ptr Log::m_display = nullptr;
sfg::Box::Ptr Log::m_windowBox = nullptr;

void Log::initialize()
{
    m_display = sfg::ScrolledWindow::Create();
    m_windowBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);

    m_display->SetScrollbarPolicy(
        sfg::ScrolledWindow::VERTICAL_ALWAYS | sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC);
    m_display->SetPlacement(sfg::ScrolledWindow::Placement::TOP_LEFT);
    m_display->SetAllocation(sf::FloatRect(30, 30, 1500, 400));
    m_display->AddWithViewport(m_windowBox);
    auto alignment = sfg::Alignment::Create();
    alignment->SetAlignment(sf::Vector2f(0, 0));
    m_windowBox->Pack(alignment);
    Game::getSingleton()->getDesktop().Add(m_display);
    m_display->Show(false);
}

void Log::push(std::string str, eMessageTypes type)
{
    auto windowSize = Game::getWindow().getSize();
    auto label = sfg::Label::Create(str);
    label->SetLineWrap(true);
    int newlines = 1;

    unsigned width = static_cast<unsigned>(str.size() * cCharWidth);
    // hack because sfgui does not requisition properly by default
    if (width > windowSize.x) {
        while (width > windowSize.x) {
            unsigned diff = width - windowSize.x;
            int count = diff / cCharWidth;
            int size = str.size() - count;
            str = str.substr(size, count);
            width = str.size() * cCharWidth;
            newlines++;
        }
    }
    label->SetRequisition(sf::Vector2f(windowSize.x - 32, 19 * newlines));
    switch (type) {
    case eMessageTypes::Announcement:
        label->SetId("yellow");
        break;
    case eMessageTypes::Alert:
        label->SetId("cyan");
        break;
    case eMessageTypes::Damage:
        label->SetId("red");
        break;
    case eMessageTypes::Dialogue:
        label->SetId("magenta");
        break;
    case eMessageTypes::OtherDialogue:
        label->SetId("other");
        break;
    case eMessageTypes::Background:
        label->SetId("background");
        break;
    }
    m_windowBox->Pack(label);
    constexpr unsigned maxHistory = 50;
    if (m_windowBox->GetChildren().size() > maxHistory) {
        m_windowBox->Remove(m_windowBox->GetChildren()[0]);
    }
    // force to the bottom
    m_display->GetVerticalAdjustment()->IncrementPage();
    m_display->GetVerticalAdjustment()->SetValue(m_display->GetVerticalAdjustment()->GetUpper());
    // force update in case too many items get added at once
    m_windowBox->Update(0.f);
}

void Log::run()
{
    m_display->Show(true);
    unsigned rectHeight = cCharSize * (cLinesDisplayed + 1);
    auto windowSize = Game::getWindow().getSize();
    m_display->SetAllocation(
        sf::FloatRect(3, windowSize.y - rectHeight, windowSize.x - 3, rectHeight - 10));
    const unsigned maxHistory = (windowSize.y - 100) / cCharSize - 2;
    // magic numbers
    sf::RectangleShape logBkg(sf::Vector2f(windowSize.x - 6, rectHeight - 3));
    logBkg.setPosition(3, windowSize.y - rectHeight);

    logBkg.setFillColor(sf::Color(12, 12, 23));
    logBkg.setOutlineThickness(3);
    logBkg.setOutlineColor(sf::Color(22, 22, 33));

    Game::getWindow().draw(logBkg);
}
