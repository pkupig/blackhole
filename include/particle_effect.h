#ifndef PARTICLE_EFFECT_H
#define PARTICLE_EFFECT_H

#include <string>

struct ParticleEffect {
    std::string name;
    std::string description;
    float blackHoleMass;
    float particleLifetime;
    float spiralStrength;
    float turbulenceStrength;
    float accretionDiskRadius;
    float particleSize;
    float colorIntensity;
    bool enableJet;
    float jetStrength;
    bool enableExplosion;
    float explosionStrength;
};

#endif