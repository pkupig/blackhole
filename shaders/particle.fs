#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 lightPos;
uniform vec3 lightDir;
uniform bool directionalLight;
uniform float colorIntensity;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDirCalc;
    if (directionalLight) {
        lightDirCalc = normalize(-lightDir);
    } else {
        lightDirCalc = normalize(lightPos - FragPos);
    }
    
    // 环境光
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    
    // 漫反射
    float diff = max(dot(norm, lightDirCalc), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // 镜面反射
    float specularStrength = 0.8;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirCalc, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    // 组合光照
    vec3 result = (ambient + diffuse + specular) * Color * lightIntensity * colorIntensity;
    
    // 添加辉光效果
    float glow = 0.5; // 基础辉光
    vec3 glowColor = Color * glow;
    result += glowColor;
    
    FragColor = vec4(result, 1.0);
    
    gl_FragDepth = gl_FragCoord.z;
}