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
    Neck = 3,
    Shoulder = 4,
    UpperArm = 5,
    Elbow = 6,
    Forearm = 7,
    Hand = 8,
    Armpit = 9,
    Ribs = 10,
    Abs = 11,
    Hip = 12,
    Groin = 13,
    Thigh = 14,
    Knee = 15,
    Shin = 16,
    Foot = 17,
    Back = 18,
    Hamstring = 19,
    Wing = 20,
    Tail = 21,
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

enum class eCreatureStance {
    Standing,
    Prone
};
