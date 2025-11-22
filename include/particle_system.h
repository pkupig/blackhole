#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <random>
#include "shader.h"
#include "particle_effect.h" 

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 color;
    float life;
    float size;
    float mass;
    int type; // 0=正常粒子, 1=喷流粒子, 2=爆炸粒子
};

struct ParticleParameters {
    float blackHoleMass;
    float particleLifetime;
    float spiralStrength;
    float turbulenceStrength;
    float accretionDiskRadius;
    float particleSize;
    float colorIntensity;
    glm::vec3 lightColor;
    float lightIntensity;
    glm::vec3 lightPosition;
    glm::vec3 lightDirection;
    bool directionalLight;

    bool enableJet;
    float jetStrength;
    float jetAngle;
    glm::vec3 jetDirection;
    float jetParticleSpeed;

    bool enableExplosion;
    float explosionStrength;
    float explosionDuration;
    float explosionRadius;
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
    int maxParticles;

    GLuint sphereVAO, sphereVBO, sphereEBO;
    std::vector<glm::vec3> sphereVertices;
    std::vector<glm::vec3> sphereNormals;
    std::vector<unsigned int> sphereIndices;
    int sphereIndexCount;

    GLuint instanceVBO;

    ParticleParameters params;

    float explosionTimer;
    bool explosionActive;

    void initializeParticles();
    void resetParticle(Particle& p, std::mt19937& gen,
        std::uniform_real_distribution<float>& dist,
        std::uniform_real_distribution<float>& distColor);
    void setupSphereGeometry();
    void setupBuffers();
    void updateBuffers();

    void updateJetParticles(float deltaTime);
    void triggerExplosion();
    void updateExplosionParticles(float deltaTime);

public:
    ParticleSystem(int maxParticles);
    ~ParticleSystem();
    void update(float deltaTime, const glm::vec3& cameraPosition);
    void render(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::vec3& viewPos);
    void applyEffect(const ParticleEffect& effect);

    ParticleParameters& getParameters() { return params; }
    int getParticleCount() const { return particles.size(); }

    void setLightPosition(const glm::vec3& pos) { params.lightPosition = pos; }
    void setLightDirection(const glm::vec3& dir) { params.lightDirection = glm::normalize(dir); }
    void setDirectionalLight(bool directional) { params.directionalLight = directional; }

    void setJetEnabled(bool enabled) { params.enableJet = enabled; }
    void setJetStrength(float strength) { params.jetStrength = strength; }
    void triggerExplosionEffect() { triggerExplosion(); }
    void setExplosionEnabled(bool enabled) { params.enableExplosion = enabled; }
    void setExplosionStrength(float strength) { params.explosionStrength = strength; }
};

#endif