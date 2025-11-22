#version 330 core
out vec4 FragColor;

uniform vec3 viewPos;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    
    if(dist < 0.2) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }
    
    float glow = smoothstep(0.4, 0.2, dist);
    vec3 glowColor = vec3(0.8, 0.3, 0.1) * glow;
    
    float lensing = smoothstep(0.5, 0.3, dist) * 0.3;
    glowColor += vec3(0.4, 0.6, 1.0) * lensing;
    
    FragColor = vec4(glowColor, glow * 0.8);
}