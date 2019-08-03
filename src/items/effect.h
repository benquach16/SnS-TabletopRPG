//this is for item effects, specifically

#pragma once

enum class eEffect {
    Thirst,
    Hunger,
    Morale,
    Stamina
};

class Effect {
public:
    Effect() {}
    virtual eEffect getType() const = 0;

private:
    eEffect m_type;
};

class ThirstEffect : public Effect {
public:
    ThirstEffect(int thirst)
        : m_thirst(thirst)
    {
    }
    eEffect getType() const override { return eEffect::Thirst; }

private:
    int m_thirst;
};

class HungerEffect : public Effect {
public:
    HungerEffect(int hunger)
        : m_hunger(hunger)
    {
    }
    eEffect getType() const override { return eEffect::Thirst; }

private:
    int m_hunger;
};

class StaminaEffect : public Effect {
public:
    StaminaEffect(int stamina)
        : m_stamina(stamina)
    {
    }
    eEffect getType() const override { return eEffect::Stamina; }

private:
    int m_stamina;
};
