#version 330 core

out vec4 fragColor;

uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    fragColor = vec4(ambient * objectColor, 1.0f);
}
