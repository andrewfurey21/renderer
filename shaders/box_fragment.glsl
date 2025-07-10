#version 330 core

in vec3 fragmentNormal;
in vec3 fragmentPosition;

out vec4 fragColor;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

uniform vec3 objectColor;

void main() {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 normFragmentNormal = normalize(fragmentNormal);
    vec3 normLightDir = normalize(lightPosition - fragmentPosition);
    float diff = max(dot(normFragmentNormal, normLightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 normViewDirection = normalize(cameraPosition - fragmentPosition);
    vec3 reflectDirection = reflect(-normLightDir, normFragmentNormal);
    float spec = pow(max(dot(normViewDirection, reflectDirection), 0.0f), 32);
    vec3 specular = specularStrength * spec * lightColor;

    fragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0f);
}
