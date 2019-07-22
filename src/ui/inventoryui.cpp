#include "inventoryui.h"
#include "types.h"
#include "../game.h"
#include "../object/creatureobject.h"
#include "../object/playerobject.h"
#include "../creatures/player.h"
#include "../items/armor.h"
#include "../items/utils.h"
#include "../creatures/utils.h"

using namespace std;

InventoryUI::InventoryUI() : m_uiState(eUiState::Backpack), m_id(-1)
{
}
constexpr int cDisplayLines = 24;

void InventoryUI::run(sf::Event event, PlayerObject* player)
{
	switch(m_uiState) {
	case eUiState::Backpack:
		doBackpack(event, player);
		break;
	case eUiState::Equipped:
		doEquipped(event, player);
		break;
	case eUiState::Detailed:
		displayDetail(event);
		break;
	case eUiState::Profile:
		doProfile(event, player);
		break;
	}
}

void InventoryUI::doBackpack(sf::Event event, PlayerObject* player)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize * cDisplayLines));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);

	Player* playerComponent = static_cast<Player*>(player->getCreatureComponent());

	sf::Text txt;
	txt.setFont(Game::getDefaultFont());
	txt.setCharacterSize(cCharSize);

	string str = "Backpack (1 - Backpack, 2 - Equipment, 3 - Profile):\n";
	
	std::map<int, int> inventory = player->getInventory();

	int count = 0;
	for(auto it : inventory) {
		char idx = ('a' + count);
		const Item* item = ItemTable::getSingleton()->get(it.first);
		str+= idx;
		str+= " - " + item->getName() + " x" + to_string(it.second) + '\n';
		count++;

		if(event.type == sf::Event::TextEntered) {
			char c = event.text.unicode;
			if(c == idx) {
				m_id = it.first;
				m_uiState = eUiState::Detailed;
			}
		}
	}
	txt.setString(str);
	Game::getWindow().draw(txt);
	if(event.type == sf::Event::TextEntered) {
		char c = event.text.unicode;
		switch(c) {
		case '1':
			break;
		case '2':
			m_uiState = eUiState::Equipped;
			break;
		case '3':
			m_uiState = eUiState::Profile;
			break;
		}
	}
}

void InventoryUI::doEquipped(sf::Event event, PlayerObject* player)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x/2, cCharSize * cDisplayLines));
	bkg.setFillColor(sf::Color(12, 12, 23));
	bkg.setOutlineThickness(1);
	bkg.setOutlineColor(sf::Color(22, 22, 33));
	Game::getWindow().draw(bkg);
	sf::RectangleShape bkg2(sf::Vector2f(windowSize.x/2, cCharSize * cDisplayLines));
	bkg2.setPosition(sf::Vector2f(windowSize.x/2, 0));
	bkg2.setFillColor(sf::Color(12, 12, 23));
	bkg2.setOutlineThickness(1);
	bkg2.setOutlineColor(sf::Color(22, 22, 33));
	Game::getWindow().draw(bkg2);

	sf::Text txt;
	txt.setFont(Game::getDefaultFont());
	txt.setCharacterSize(cCharSize);
	string str = "Equipped Armor\n";
	std::vector<int> armorId = player->getCreatureComponent()->getArmorId();
	int i = 0;
	for(i = 0; i < armorId.size(); ++i) {
		const Armor* armor = ArmorTable::getSingleton()->get(armorId[i]);
		char idx = ('a' + i);
		str+= idx;
		str+= " - " +armor->getName() + '\n';

		if(event.type == sf::Event::TextEntered) {
			char c = event.text.unicode;
			if(c == idx) {
				m_id = armorId[i];
				m_uiState = eUiState::Detailed;
			}
		}
	}
	txt.setString(str);
	Game::getWindow().draw(txt);



	sf::Text txt2;
	txt2.setPosition(sf::Vector2f(windowSize.x/2, 0));
	txt2.setFont(Game::getDefaultFont());
	txt2.setCharacterSize(cCharSize);

	string weapontxt = "Equipped Weapons\n";
	const Weapon* weapon = player->getCreatureComponent()->getPrimaryWeapon();
	const char idx = ('a' + i);
	weapontxt += idx;
	weapontxt += " - " + weapon->getName();
	txt2.setString(weapontxt);
	Game::getWindow().draw(txt2);	

	if(event.type == sf::Event::TextEntered) {
		char c = event.text.unicode;
		switch(c) {
		case '1':
			m_uiState = eUiState::Backpack;
			break;
		case '2':
			break;
		case '3':
			m_uiState = eUiState::Profile;
			break;
		}
		if(c == idx) {
			m_id = player->getCreatureComponent()->getPrimaryWeaponId();
			cout << m_id << endl;
			m_uiState = eUiState::Detailed;
		}
	}
}

void InventoryUI::displayDetail(sf::Event event)
{
	assert(m_id != -1);
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize * cDisplayLines));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);

	sf::Text txt;
	txt.setFont(Game::getDefaultFont());
	txt.setCharacterSize(cCharSize);

	string str;
	const Item* item = ItemTable::getSingleton()->get(m_id);
	str += item->getName() + '\n';
	str += item->getDescription() + '\n';
	str += "Type: " + itemTypeToString(item->getItemType()) + '\n';

	if(item->getItemType() == eItemType::Armor) {
		const Armor* armor = static_cast<const Armor*>(item);
		str += "AV: " + to_string(armor->getAV()) + '\n';
		str += "AP: " + to_string(armor->getAP()) + '\n';
		str += "Covers :";
		for(auto i : armor->getCoverage()) {
			
			str += bodyPartToString(i) + ',';
		}
		str += '\n';
	}
	if(item->getItemType() == eItemType::Weapon) {
		const Weapon* weapon = static_cast<const Weapon*>(item);
		str += "Length: " + lengthToString(weapon->getLength()) + '\n';
		str += "Proficiency: " + weaponTypeToString(weapon->getType()) + '\n';
		str += "Weapon Parts: \n";
		std::vector<Component*> components = weapon->getComponents();
		for(int i = 0; i < components.size(); ++i) {
			str += components[i]->getName() + " - ";
			str += "Damage: " + to_string(components[i]->getDamage()) + ' ' +
				damageTypeToString(components[i]->getType()) + " [ ";
			for(auto it : components[i]->getProperties()) {
				str += weaponPropToString(it) + ' ';
			}
			str += "]\n";
		}
	}
	str += '\n';
	str += "Worth " + to_string(item->getCost()) + " silvers\n";
	txt.setString(str);

	Game::getWindow().draw(txt);

	if(event.type == sf::Event::TextEntered) {
		m_uiState = eUiState::Backpack;
	}
}

void InventoryUI::doProfile(sf::Event event, PlayerObject* player)
{
	auto windowSize = Game::getWindow().getSize();
	
	sf::RectangleShape bkg(sf::Vector2f(windowSize.x, cCharSize * cDisplayLines));
	bkg.setFillColor(sf::Color(12, 12, 23));
	Game::getWindow().draw(bkg);

	Creature* creature = player->getCreatureComponent();

	sf::Text ap;
	ap.setCharacterSize(cCharSize);
	ap.setFont(Game::getDefaultFont());
	string str = "Success rate: " + to_string(creature->getSuccessRate()) + "%" +
				 '\n' + "Blood loss: " + to_string(creature->getBloodLoss()) + '\n';

	const std::vector<Wound*> wounds = creature->getWounds();
	for(auto i : wounds) {
		str += "Level " + to_string(i->getLevel()) + " wound at " + bodyPartToString(i->getLocation()) + '\n';
	}
	ap.setString(str);

	sf::Text stats;
	stats.setCharacterSize(cCharSize);
	stats.setFont(Game::getDefaultFont());
	stats.setPosition(sf::Vector2f(windowSize.x/2, 0));
	string statStr = "Primary Attributes\nBrawn: " + to_string(creature->getBrawn()) + '\n' +
					  "Agility: " + to_string(creature->getAgility()) + '\n' +
					  "Cunning: " + to_string(creature->getCunning()) + '\n' +
					  "Perception: " + to_string(creature->getPerception()) + '\n' +
					  "Will: " + to_string(creature->getWill()) + '\n' +
					  "Derived Attributes\n"+
					  "Grit: " + to_string(creature->getGrit()) + '\n' +
					  "Keen: " + to_string(creature->getKeen()) + '\n' +
					  "Reflex: " + to_string(creature->getReflex()) + '\n' +
					  "Speed: " + to_string(creature->getSpeed()) + '\n';

	statStr+="Proficiencies\n";
	statStr+="Polearms: " + to_string(creature->getProficiency(eWeaponTypes::Polearms)) + '\n';
	stats.setString(statStr);
	Game::getWindow().draw(ap);
	Game::getWindow().draw(stats);
		if(event.type == sf::Event::TextEntered) {
		char c = event.text.unicode;
		switch(c) {
		case '1':
			m_uiState = eUiState::Backpack;
			break;
		case '2':
			m_uiState = eUiState::Equipped;
			break;

		}
	}
}
