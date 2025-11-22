#include "particle_system.h"
#include <random>
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>  

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

ParticleSystem::ParticleSystem(int maxParticles) : maxParticles(maxParticles) {
    setupSphereGeometry();

    initializeParticles();
    setupBuffers();

    params.blackHoleMass = 5000.0f;
    params.particleLifetime = 10.0f;
    params.spiralStrength = 1.5f;
    params.turbulenceStrength = 0.3f;
    params.accretionDiskRadius = 25.0f;
    params.particleSize = 0.1f;
    params.colorIntensity = 2.0f;
    params.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    params.lightIntensity = 1.0f;
    params.lightPosition = glm::vec3(10.0f, 10.0f, 10.0f);
    params.lightDirection = glm::vec3(-1.0f, -1.0f, -1.0f);
    params.directionalLight = false;

    // 喷流参数
    params.enableJet = false;
    params.jetStrength = 5.0f;
    params.jetAngle = 15.0f;
    params.jetDirection = glm::vec3(0.0f, 1.0f, 0.0f);
    params.jetParticleSpeed = 20.0f;

    // 爆炸参数
    params.enableExplosion = false;
    params.explosionStrength = 10.0f;
    params.explosionDuration = 2.0f;
    params.explosionRadius = 15.0f;

    // 特效状态
    explosionTimer = 0.0f;
    explosionActive = false;
}

ParticleSystem::~ParticleSystem() {
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteBuffers(1, &instanceVBO);
}

void ParticleSystem::setupSphereGeometry() {
    int stacks = 8;
    int slices = 8;

    sphereVertices.clear();
    sphereNormals.clear();
    sphereIndices.clear();

    for (int i = 0; i <= stacks; ++i) {
        float phi = M_PI * i / stacks;
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * M_PI * j / slices;

            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);

            sphereVertices.push_back(glm::vec3(x, y, z));
            sphereNormals.push_back(glm::vec3(x, y, z));
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = i * (slices + 1) + j;
            int second = first + slices + 1;

            sphereIndices.push_back(first);
            sphereIndices.push_back(second);
            sphereIndices.push_back(first + 1);

            sphereIndices.push_back(first + 1);
            sphereIndices.push_back(second);
            sphereIndices.push_back(second + 1);
        }
    }

    sphereIndexCount = sphereIndices.size();

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    std::vector<float> vertexData;
    for (size_t i = 0; i < sphereVertices.size(); ++i) {
        vertexData.push_back(sphereVertices[i].x);
        vertexData.push_back(sphereVertices[i].y);
        vertexData.push_back(sphereVertices[i].z);
        vertexData.push_back(sphereNormals[i].x);
        vertexData.push_back(sphereNormals[i].y);
        vertexData.push_back(sphereNormals[i].z);
    }

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float),
        vertexData.data(), GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    // index
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int),
        sphereIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void ParticleSystem::initializeParticles() {
    particles.resize(maxParticles);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::uniform_real_distribution<float> distColor(0.5f, 1.0f);

    for (int i = 0; i < maxParticles; ++i) {
        resetParticle(particles[i], gen, dist, distColor);
        particles[i].type = 0; 
    }
}

void ParticleSystem::resetParticle(Particle& p, std::mt19937& gen,
    std::uniform_real_distribution<float>& dist,
    std::uniform_real_distribution<float>& distColor) {
    // 在吸积盘平面内随机位置
    float angle = dist(gen) * 2.0f * M_PI;
    float radius = 5.0f + dist(gen) * params.accretionDiskRadius;
    float height = (dist(gen) - 0.5f) * 2.0f;

    p.position = glm::vec3(
        cos(angle) * radius,
        height,
        sin(angle) * radius
    );

    // 初始速度（轨道速度 + 随机分量）
    float orbitalSpeed = sqrt(params.blackHoleMass / radius) * 0.8f;
    glm::vec3 tangent = glm::normalize(glm::vec3(-sin(angle), 0.0f, cos(angle)));
    glm::vec3 orbitalVelocity = tangent * orbitalSpeed;

    // 向黑洞的径向速度
    glm::vec3 radialDir = glm::normalize(-p.position);
    glm::vec3 inwardVelocity = radialDir * (0.1f + dist(gen) * 0.3f);

    p.velocity = orbitalVelocity + inwardVelocity;
    p.velocity.y += (dist(gen) - 0.5f) * 0.5f;

    p.life = params.particleLifetime * (0.8f + dist(gen) * 0.4f);
    p.size = params.particleSize * (0.5f + dist(gen));

    // 颜色基于位置和速度
    float colorFactor = glm::length(p.velocity) / 10.0f;
    p.color = glm::vec3(
        0.3f + distColor(gen) * 0.7f * colorFactor,
        0.3f + distColor(gen) * 0.5f * colorFactor,
        1.0f
    );

    p.mass = 0.01f;
    p.type = 0; 
}

void ParticleSystem::setupBuffers() {
    // 创建实例化VBO用于粒子数据
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(sphereVAO);

    // 实例位置
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glVertexAttribDivisor(2, 1);

    // 实例颜色
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
    glVertexAttribDivisor(3, 1);

    // 实例大小
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);
}

void ParticleSystem::update(float deltaTime, const glm::vec3& cameraPosition) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::uniform_real_distribution<float> distColor(0.5f, 1.0f);

    if (explosionActive) {
        updateExplosionParticles(deltaTime);
        explosionTimer -= deltaTime;
        if (explosionTimer <= 0.0f) {
            explosionActive = false;
        }
    }

    if (params.enableJet) {
        updateJetParticles(deltaTime);
    }

    for (auto& p : particles) {
        if (p.life <= 0.0f && p.type == 0) { // 只重置正常粒子
            resetParticle(p, gen, dist, distColor);
            continue;
        }

        // 计算到黑洞的距离
        float distance = glm::length(p.position);

        if (distance < 0.5f && p.type == 0) {
            // 粒子被黑洞吞噬
            p.life = 0.0f;
            continue;
        }

        glm::vec3 gravityDir = glm::normalize(-p.position);

        // 牛顿引力 + 相对论经验修正
        float gravity = params.blackHoleMass / (distance * distance);
        float relFactor = 1.0f + 2.0f / (distance + 0.5f); 

        // 螺旋吸积效应
        glm::vec3 spiralForce = glm::cross(gravityDir, glm::vec3(0.0f, 1.0f, 0.0f)) * params.spiralStrength;

        // 湍流效应
        glm::vec3 turbulence = glm::vec3(
            (dist(gen) - 0.5f) * 2.0f,
            (dist(gen) - 0.5f) * 2.0f,
            (dist(gen) - 0.5f) * 2.0f
        ) * params.turbulenceStrength;

        glm::vec3 acceleration = gravityDir * gravity * relFactor + spiralForce + turbulence;

        // 只有正常粒子受黑洞引力影响
        if (p.type == 0) {
            p.velocity += acceleration * deltaTime;
        }

        // 速度限制
        float speed = glm::length(p.velocity);
        if (speed > 100.0f) {
            p.velocity = glm::normalize(p.velocity) * 100.0f;
        }

        p.position += p.velocity * deltaTime;

        if (p.type == 0) {
            // 正常粒子受潮汐力影响
            float tidalFactor = 1.0f + 5.0f / (distance * distance + 0.1f);
            p.life -= deltaTime * tidalFactor;
        }
        else if (p.type == 1 || p.type == 2) {
            // 特效粒子固定寿命衰减
            p.life -= deltaTime;
        }

        if (p.type == 0) {
            // 正常粒子颜色
            float speedFactor = glm::length(p.velocity) / 50.0f;
            float energyRelease = 1.0f / (distance + 0.5f);

            p.color.r = 0.5f + energyRelease * 0.5f;
            p.color.g = 0.3f + speedFactor * 0.5f;
            p.color.b = 1.0f - energyRelease * 0.3f;
        }
        else if (p.type == 1) {
            // 喷流粒子 - 黄色/橙色
            p.color = glm::vec3(1.0f, 0.8f, 0.2f);
        }
        else if (p.type == 2) {
            // 爆炸粒子 - 红色/橙色
            float lifeRatio = p.life / 3.0f;
            p.color = glm::vec3(1.0f, 0.5f * lifeRatio, 0.1f * lifeRatio);
        }

        p.color = glm::clamp(p.color, 0.0f, 1.0f);
    }

    updateBuffers();
}

void ParticleSystem::updateJetParticles(float deltaTime) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    // 在黑洞附近生成喷流粒子
    int jetParticlesToCreate = 5;

    for (int i = 0; i < jetParticlesToCreate; ++i) {
        for (auto& p : particles) {
            if (p.life <= 0.0f) {
                // 在黑洞中心重置为喷流粒子
                p.position = glm::vec3(0.0f);

                // 计算喷流方向（在锥形范围内随机）
                float angle = params.jetAngle * M_PI / 180.0f;
                float randomAngle = dist(gen) * angle;
                float randomRotation = dist(gen) * 2.0f * M_PI;

                glm::vec3 jetDir = params.jetDirection;

                glm::vec3 axis = glm::vec3(cos(randomRotation), 0.0f, sin(randomRotation));
                glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), randomAngle, axis);

                glm::vec3 randomDir = glm::vec3(rotationMatrix * glm::vec4(jetDir, 1.0f));

                p.velocity = randomDir * params.jetParticleSpeed;
                p.life = 2.0f;
                p.size = params.particleSize * 0.3f;
                p.color = glm::vec3(1.0f, 0.8f, 0.2f);
                p.type = 1; // 喷流粒子

                break;
            }
        }
    }
}

void ParticleSystem::triggerExplosion() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    // 重置爆炸状态
    explosionActive = true;
    explosionTimer = params.explosionDuration;

    // 创建爆炸粒子
    int explosionParticles = 500;

    for (int i = 0; i < explosionParticles; ++i) {
        for (auto& p : particles) {
            if (p.life <= 0.0f) {
                // 在黑洞中心创建爆炸粒子
                p.position = glm::vec3(0.0f);

                // 随机方向
                float theta = dist(gen) * 2.0f * M_PI;
                float phi = acos(2.0f * dist(gen) - 1.0f);

                p.velocity = glm::vec3(
                    sin(phi) * cos(theta),
                    sin(phi) * sin(theta),
                    cos(phi)
                ) * params.explosionStrength * (0.8f + dist(gen) * 0.4f);

                p.life = 3.0f;
                p.size = params.particleSize * (0.8f + dist(gen) * 0.4f);
                p.color = glm::vec3(1.0f, 0.5f, 0.1f);
                p.type = 2; // 爆炸粒子

                break;
            }
        }
    }
}

void ParticleSystem::updateExplosionParticles(float deltaTime) {
    // 爆炸粒子会自然衰减，不需要特殊更新
    // 生命周期已经在主更新循环中处理
}

void ParticleSystem::updateBuffers() {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), particles.data());
}

void ParticleSystem::render(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::vec3& viewPos) {
    shader.use();

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    shader.setVec3("lightColor", params.lightColor);
    shader.setFloat("lightIntensity", params.lightIntensity);
    shader.setVec3("lightPos", params.lightPosition);
    shader.setVec3("lightDir", params.lightDirection);
    shader.setBool("directionalLight", params.directionalLight);

    shader.setFloat("colorIntensity", params.colorIntensity);
    shader.setVec3("viewPos", viewPos);

    glBindVertexArray(sphereVAO);
    glDrawElementsInstanced(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0, particles.size());
    glBindVertexArray(0);
}

void ParticleSystem::applyEffect(const ParticleEffect& effect) {
    params.blackHoleMass = effect.blackHoleMass;
    params.particleLifetime = effect.particleLifetime;
    params.spiralStrength = effect.spiralStrength;
    params.turbulenceStrength = effect.turbulenceStrength;
    params.accretionDiskRadius = effect.accretionDiskRadius;
    params.particleSize = effect.particleSize;
    params.colorIntensity = effect.colorIntensity;
    params.enableJet = effect.enableJet;
    params.jetStrength = effect.jetStrength;
    params.enableExplosion = effect.enableExplosion;
    params.explosionStrength = effect.explosionStrength;
}