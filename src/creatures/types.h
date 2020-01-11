#pragma once

#include <map>

enum class eHitLocations : unsigned {
    Head = 0,
    Chest = 1,
    Neck = 2,
    Arm = 3,
    Belly = 4,
    Thigh = 5,
    Shin = 6,
    Wing,
    Tail
};

enum class eBodyParts : unsigned {
    Crown = 0,
    Face = 1,
    Chin = 2,
    Eyes = 3,
    Neck = 4,
    Shoulder = 5,
    UpperArm = 6,
    Elbow = 7,
    Forearm = 8,
    Hand = 9,
    Armpit = 10,
    Ribs = 11,
    Abs = 12,
    Hip = 13,
    Groin = 14,
    Thigh = 15,
    Knee = 16,
    Shin = 17,
    Foot = 18,
    Back = 19,
    Hamstring = 20,
    Wing = 21,
    Tail = 22,
    // roll for another location
    SecondLocationHead,
    SecondLocationArm
};

enum class eEffects {
    KO1,
    KO2,
    KO3,
    KO,
    BL1,
    BL2,
    BL3,
    drop1,
    drop2,
    drop3,
    drop,
    KD1,
    KD2,
    KD3,
    KD,
    Death,
};

enum class eCreatureStance { Standing, Prone };
