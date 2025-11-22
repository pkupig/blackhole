#include "script_parser.h"
#include "particle_effect.h" 
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

void ScriptParser::loadScripts(const std::string& directory) {
    namespace fs = std::filesystem;

    if (!fs::exists(directory)) {
        fs::create_directories(directory);
        createDefaultScripts(directory);
        return;
    }

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.path().extension() == ".effect") {
            parseEffectScript(entry.path().string());
        }
    }
}

void ScriptParser::parseEffectScript(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open effect script: " << filename << std::endl;
        return;
    }

    ParticleEffect effect;
    effect.name = std::filesystem::path(filename).stem().string();

    effect.blackHoleMass = 5000.0f;
    effect.particleLifetime = 10.0f;
    effect.spiralStrength = 1.5f;
    effect.turbulenceStrength = 0.3f;
    effect.accretionDiskRadius = 25.0f;
    effect.particleSize = 0.1f;
    effect.colorIntensity = 2.0f;
    effect.enableJet = false;
    effect.jetStrength = 5.0f;
    effect.enableExplosion = false;
    effect.explosionStrength = 10.0f;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, '=')) {
            std::string value;
            if (std::getline(iss, value)) {
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                parseParameter(key, value, effect);
            }
        }
        else if (line.find("description:") != std::string::npos) {
            effect.description = line.substr(12);
            effect.description.erase(0, effect.description.find_first_not_of(" \t"));
            effect.description.erase(effect.description.find_last_not_of(" \t") + 1);
        }
    }

    effects.push_back(effect);
    std::cout << "Loaded effect: " << effect.name << " - " << effect.description << std::endl;
}

void ScriptParser::parseParameter(const std::string& key, const std::string& value, ParticleEffect& effect) {
    try {
        if (key == "blackHoleMass") effect.blackHoleMass = std::stof(value);
        else if (key == "particleLifetime") effect.particleLifetime = std::stof(value);
        else if (key == "spiralStrength") effect.spiralStrength = std::stof(value);
        else if (key == "turbulenceStrength") effect.turbulenceStrength = std::stof(value);
        else if (key == "accretionDiskRadius") effect.accretionDiskRadius = std::stof(value);
        else if (key == "particleSize") effect.particleSize = std::stof(value);
        else if (key == "colorIntensity") effect.colorIntensity = std::stof(value);
        else if (key == "enableJet") effect.enableJet = (std::stof(value) > 0.5f);
        else if (key == "jetStrength") effect.jetStrength = std::stof(value);
        else if (key == "enableExplosion") effect.enableExplosion = (std::stof(value) > 0.5f);
        else if (key == "explosionStrength") effect.explosionStrength = std::stof(value);
        else {
            std::cerr << "Unknown parameter in effect script: " << key << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing parameter " << key << " with value " << value << ": " << e.what() << std::endl;
    }
}

void ScriptParser::createDefaultScripts(const std::string& directory) {
    std::cout << "Creating default effect scripts in: " << directory << std::endl;

    createEffectScript(directory + "/gentle_swirl.effect",
        "Gentle Swirl", "Calm swirling motion around black hole",
        2000.0f, 15.0f, 1.0f, 0.1f, 20.0f, 0.08f, 1.5f,
        false, 0.0f, false, 0.0f);

    createEffectScript(directory + "/violent_accretion.effect",
        "Violent Accretion", "Intense particle inflow with high turbulence",
        8000.0f, 8.0f, 2.5f, 1.5f, 15.0f, 0.15f, 3.0f,
        false, 0.0f, false, 0.0f);

    createEffectScript(directory + "/distant_rings.effect",
        "Distant Rings", "Wide accretion disk with slow movement",
        3000.0f, 25.0f, 0.5f, 0.05f, 40.0f, 0.05f, 1.0f,
        false, 0.0f, false, 0.0f);

    createEffectScript(directory + "/gamma_jet.effect",
        "Gamma Ray Jet", "Powerful gamma ray jet emission",
        5000.0f, 10.0f, 0.8f, 0.3f, 25.0f, 0.1f, 2.0f,
        true, 8.0f, false, 0.0f);

    createEffectScript(directory + "/supernova.effect",
        "Supernova Explosion", "Massive stellar explosion",
        10000.0f, 5.0f, 3.0f, 2.0f, 30.0f, 0.2f, 4.0f,
        false, 0.0f, true, 15.0f);
}

void ScriptParser::createEffectScript(const std::string& filename,
    const std::string& name,
    const std::string& description,
    float mass, float lifetime, float spiral,
    float turbulence, float radius, float size,
    float colorIntensity, bool enableJet, float jetStrength,
    bool enableExplosion, float explosionStrength) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to create effect script: " << filename << std::endl;
        return;
    }

    file << "description: " << description << "\n";
    file << "blackHoleMass=" << mass << "\n";
    file << "particleLifetime=" << lifetime << "\n";
    file << "spiralStrength=" << spiral << "\n";
    file << "turbulenceStrength=" << turbulence << "\n";
    file << "accretionDiskRadius=" << radius << "\n";
    file << "particleSize=" << size << "\n";
    file << "colorIntensity=" << colorIntensity << "\n";
    file << "enableJet=" << (enableJet ? "1" : "0") << "\n";
    file << "jetStrength=" << jetStrength << "\n";
    file << "enableExplosion=" << (enableExplosion ? "1" : "0") << "\n";
    file << "explosionStrength=" << explosionStrength << "\n";

    file.close();
    std::cout << "Created effect script: " << filename << std::endl;
}