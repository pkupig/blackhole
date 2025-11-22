#include "gui.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

GUI::GUI(GLFWwindow* window, Camera& camera)
    : m_camera(camera) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void GUI::render(ParticleSystem& particleSystem, ScriptParser& scriptParser) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Black Hole Particle System Control");

    if (ImGui::CollapsingHeader("Particle Parameters")) {
        auto& params = particleSystem.getParameters();

        ImGui::SliderFloat("Black Hole Mass", &params.blackHoleMass, 100.0f, 10000.0f);
        ImGui::SliderFloat("Particle Lifetime", &params.particleLifetime, 1.0f, 30.0f);
        ImGui::SliderFloat("Spiral Strength", &params.spiralStrength, 0.0f, 5.0f);
        ImGui::SliderFloat("Turbulence Strength", &params.turbulenceStrength, 0.0f, 2.0f);
        ImGui::SliderFloat("Accretion Disk Radius", &params.accretionDiskRadius, 5.0f, 50.0f);
        ImGui::SliderFloat("Particle Size", &params.particleSize, 0.01f, 0.5f);
        ImGui::SliderFloat("Color Intensity", &params.colorIntensity, 0.5f, 5.0f);
    }

    if (ImGui::CollapsingHeader("Advanced Lighting")) {
        auto& params = particleSystem.getParameters();

        ImGui::ColorEdit3("Light Color", &params.lightColor[0]);
        ImGui::SliderFloat("Light Intensity", &params.lightIntensity, 0.0f, 3.0f);

        ImGui::Checkbox("Directional Light", &params.directionalLight);

        if (params.directionalLight) {
            ImGui::SliderFloat3("Light Direction", &params.lightDirection[0], -1.0f, 1.0f);
            params.lightDirection = glm::normalize(params.lightDirection);
        }
        else {
            ImGui::SliderFloat3("Light Position", &params.lightPosition[0], -50.0f, 50.0f);
        }

        if (ImGui::Button("White Light")) {
            params.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        ImGui::SameLine();
        if (ImGui::Button("Blue Light")) {
            params.lightColor = glm::vec3(0.5f, 0.7f, 1.0f);
        }
        ImGui::SameLine();
        if (ImGui::Button("Orange Light")) {
            params.lightColor = glm::vec3(1.0f, 0.6f, 0.3f);
        }

        if (ImGui::Button("Top Light")) {
            params.lightPosition = glm::vec3(0.0f, 50.0f, 0.0f);
        }
        ImGui::SameLine();
        if (ImGui::Button("Front Light")) {
            params.lightPosition = glm::vec3(0.0f, 0.0f, 50.0f);
        }
        ImGui::SameLine();
        if (ImGui::Button("Side Light")) {
            params.lightPosition = glm::vec3(50.0f, 0.0f, 0.0f);
        }
    }

    if (ImGui::CollapsingHeader("Special Effects")) {
        auto& params = particleSystem.getParameters();

        ImGui::Text("Gamma Ray Jet:");
        ImGui::Checkbox("Enable Jet", &params.enableJet);

        if (params.enableJet) {
            ImGui::SliderFloat("Jet Strength", &params.jetStrength, 1.0f, 20.0f);
            ImGui::SliderFloat("Jet Angle", &params.jetAngle, 5.0f, 45.0f);
            ImGui::SliderFloat("Jet Speed", &params.jetParticleSpeed, 5.0f, 50.0f);
            ImGui::SliderFloat3("Jet Direction", &params.jetDirection[0], -1.0f, 1.0f);

            params.jetDirection = glm::normalize(params.jetDirection);

            if (ImGui::Button("Vertical Jet")) {
                params.jetDirection = glm::vec3(0.0f, 1.0f, 0.0f);
            }
            ImGui::SameLine();
            if (ImGui::Button("Horizontal Jet")) {
                params.jetDirection = glm::vec3(1.0f, 0.0f, 0.0f);
            }
        }

        ImGui::Separator();

        ImGui::Text("Supernova Explosion:");
        ImGui::Checkbox("Enable Explosion", &params.enableExplosion);

        if (params.enableExplosion) {
            ImGui::SliderFloat("Explosion Strength", &params.explosionStrength, 1.0f, 20.0f);
        }

        if (ImGui::Button("Trigger Explosion Now!")) {
            particleSystem.triggerExplosionEffect();
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(Manually triggered explosion)");
    }

    if (ImGui::CollapsingHeader("Camera Control")) {
        static glm::vec3 target = glm::vec3(0.0f);
        static float radius = 25.0f;

        if (ImGui::SliderFloat3("Target", &target[0], -50.0f, 50.0f)) {
            m_camera.SetTarget(target);
        }

        if (ImGui::SliderFloat("Radius", &radius, 2.0f, 100.0f)) {
            m_camera.SetRadius(radius);
        }

        if (ImGui::SliderFloat("Field of View", &m_camera.Zoom, 20.0f, 90.0f)) {
        }

        if (ImGui::Button("Reset Camera")) {
            target = glm::vec3(0.0f);
            radius = 25.0f;
            m_camera.SetTarget(target);
            m_camera.SetRadius(radius);
            m_camera.Zoom = 45.0f;
        }

        ImGui::Text("Controls:");
        ImGui::Text("  Right Click + Drag: Rotate around target");
        ImGui::Text("  Mouse Wheel: Zoom in/out");
        ImGui::Text("  RC + : Move target");
        ImGui::Text("  RC + Q/E: Move target up/down");
        ImGui::Text("  ESC: Exit program");

        ImGui::Text("Current Position: (%.1f, %.1f, %.1f)",
            m_camera.Position.x, m_camera.Position.y, m_camera.Position.z);
        ImGui::Text("Current Target: (%.1f, %.1f, %.1f)",
            m_camera.Target.x, m_camera.Target.y, m_camera.Target.z);
        ImGui::Text("Current FOV: %.1f", m_camera.Zoom);
    }

    if (ImGui::CollapsingHeader("Performance")) {
        ImGui::Text("Particle Count: %d", particleSystem.getParticleCount());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}