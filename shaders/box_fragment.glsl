#version 330 core

in vec3 fragmentNormal;
in vec3 fragmentPosition;

out vec4 fragColor;

uniform vec3 lightColor;
uniform vec3 lightPosition;

uniform vec3 objectColor;

void main() {
    float ambientStrength = 0.1;
    vec3 normFragmentNormal = normalize(fragmentNormal);
    vec3 normLightDir = normalize(lightPosition - fragmentPosition);
    float diff = max(dot(normFragmentNormal, normLightDir), 0.0f);
    vec3 ambient = ambientStrength * lightColor;
    vec3 diffuse = diff * lightColor;
    fragColor = vec4((ambient + diffuse) * objectColor, 1.0f);
}
