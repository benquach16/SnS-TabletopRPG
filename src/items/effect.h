// this is for item effects, specifically

#pragma once

enum class eItemEffect { Thirst, Hunger, Morale, Stamina };

class ItemEffect {
public:
    ItemEffect(int value)
        : m_value(value)
    {
    }
    virtual eItemEffect getType() const = 0;
    int getValue() const { return m_value; }

private:
    int m_value;
};

class ThirstEffect : public ItemEffect {
public:
    ThirstEffect(int thirst)
        : ItemEffect(thirst)
    {
    }
    eItemEffect getType() const override { return eItemEffect::Thirst; }
};

class HungerEffect : public ItemEffect {
public:
    HungerEffect(int hunger)
        : ItemEffect(hunger)
    {
    }
    eItemEffect getType() const override { return eItemEffect::Thirst; }
    int getHunger() const { return m_hunger; }

private:
    int m_hunger;
};

class StaminaEffect : public ItemEffect {
public:
    StaminaEffect(int stamina)
        : ItemEffect(stamina)
    {
    }
    eItemEffect getType() const override { return eItemEffect::Stamina; }

private:
    int m_stamina;
};
