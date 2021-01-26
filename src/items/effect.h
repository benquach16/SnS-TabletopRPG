// this is for item effects, specifically

#pragma once

#include "creatures/creature.h"
#include "creatures/types.h"

enum class eItemEffect { Thirst, Hunger, Stamina, Bandage, Firstaid };

class ItemEffect {
public:
    ItemEffect(int value)
        : m_value(value)
    {
    }
    virtual ~ItemEffect() {}
    virtual eItemEffect getType() const = 0;
    int getValue() const { return m_value; }
    virtual void apply(Creature* creature, eBodyParts part, int level) const = 0;

protected:
    int m_value;
};

class ThirstEffect : public ItemEffect {
public:
    ThirstEffect(int thirst)
        : ItemEffect(thirst)
    {
    }
    eItemEffect getType() const override { return eItemEffect::Thirst; }

    void apply(Creature* creature, eBodyParts part, int level) const override {}
};

class HungerEffect : public ItemEffect {
public:
    HungerEffect(int hunger)
        : ItemEffect(hunger)
    {
    }
    eItemEffect getType() const override { return eItemEffect::Thirst; }
    void apply(Creature* creature, eBodyParts part, int level) const override
    {
        creature->modifyFatigue(eCreatureFatigue::Hunger, -m_value);
    }

private:
};

class StaminaEffect : public ItemEffect {
public:
    StaminaEffect(int stamina)
        : ItemEffect(stamina)
    {
    }
    eItemEffect getType() const override { return eItemEffect::Stamina; }
    void apply(Creature* creature, eBodyParts part, int level) const override {}

private:
};

class BandageEffect : public ItemEffect {
public:
    BandageEffect(int bandage)
        : ItemEffect(bandage)
    {
    }
    eItemEffect getType() const override { return eItemEffect::Bandage; }
    void apply(Creature* creature, eBodyParts part, int level) const override
    {
        creature->reduceBleed(part, getValue());
    }

private:
};

class FirstAidEffect : public ItemEffect {
public:
    FirstAidEffect(int firstAid)
        : ItemEffect(firstAid)
    {
    }
    eItemEffect getType() const override { return eItemEffect::Firstaid; }
    void apply(Creature* creature, eBodyParts part, int level) const override
    {
        creature->reduceWound(part, level, getValue());
        creature->modifyFatigue(eCreatureFatigue::Stamina, getValue());
    }

private:
};
