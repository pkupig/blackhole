#ifndef SCRIPT_PARSER_H
#define SCRIPT_PARSER_H

#include <string>
#include <vector>

// Ç°ÏòÉùÃ÷
struct ParticleEffect;

class ScriptParser {
private:
    std::vector<ParticleEffect> effects;

    void parseEffectScript(const std::string& filename);
    void parseParameter(const std::string& key, const std::string& value, ParticleEffect& effect);
    void createDefaultScripts(const std::string& directory);
    void createEffectScript(const std::string& filename,
        const std::string& name,
        const std::string& description,
        float mass, float lifetime, float spiral,
        float turbulence, float radius, float size,
        float colorIntensity, bool enableJet, float jetStrength,
        bool enableExplosion, float explosionStrength);

public:
    void loadScripts(const std::string& directory);
    const std::vector<ParticleEffect>& getEffects() const { return effects; }
};

#endif