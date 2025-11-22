#version 330 core
layout (location = 0) in vec3 aPos;      
layout (location = 1) in vec3 aNormal;   
layout (location = 2) in vec3 instancePos; 
layout (location = 3) in vec3 instanceColor; 
layout (location = 4) in float instanceSize; 

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

uniform mat4 projection;
uniform mat4 view;

void main() {
    vec3 worldPos = instancePos + aPos * instanceSize;
    FragPos = worldPos;
    Normal = aNormal;
    Color = instanceColor;
    gl_Position = projection * view * vec4(worldPos, 1.0);
}