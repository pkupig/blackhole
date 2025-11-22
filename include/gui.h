#ifndef GUI_H
#define GUI_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "particle_system.h"
#include "script_parser.h"
#include "camera.h"

class GUI {
public:
    GUI(GLFWwindow* window, Camera& camera);
    ~GUI() = default;
    
    void render(ParticleSystem& particleSystem, ScriptParser& scriptParser);
    void cleanup();

private:
    Camera& m_camera;
};

#endif