# Black Hole Particle System

A real-time 3D visualization of black hole accretion disk physics with advanced particle system simulation.

![Black Hole Simulation](https://img.shields.io/badge/Simulation-Black%20Hole-blueviolet)
![OpenGL](https://img.shields.io/badge/Graphics-OpenGL-orange)
![C++](https://img.shields.io/badge/Language-C%2B%2B-blue)

## Features

- **Realistic Physics Simulation**: Newtonian gravity with relativistic corrections
- **Dynamic Particle System**: 12,000+ particles with individual physics
- **Advanced Visual Effects**:
  - Gamma ray jets
  - Supernova explosions  
  - Accretion disk formation
  - Gravitational lensing effects
- **Interactive GUI**: Real-time parameter adjustment
- **Scriptable Effects**: Load and save particle effect configurations
- **Orbital Camera**: Free movement around the black hole

## Requirements

- **Operating System**: Windows, Linux, or macOS, on Visual Studio.
- **Graphics**: OpenGL 3.3+ compatible GPU
- **Libraries**:
  - GLFW 3.0+
  - GLEW 2.0+
  - GLM 0.9.9+
  - Dear ImGui 1.80+

## Building the Project

### Using CMake (Recommended)

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build . --config Debug
