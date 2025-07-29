#version 330 core

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 color;
};

in vec3 fragmentPosition;
in vec3 fragmentNormal;
in vec2 fragmentTextureCoords;

uniform vec3 color;
uniform DirectionalLight light;

out vec4 fragColor;

void main() {
    vec3 ambient = light.ambient * color;

    vec3 normFragmentNormal = normalize(fragmentNormal);
    vec3 normLightDir = normalize(-light.direction);
    float diff = max(dot(normFragmentNormal, normLightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * color;

    fragColor = vec4((ambient + diffuse) * light.color, 1.0f);
}
