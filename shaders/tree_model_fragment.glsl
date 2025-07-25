#version 330 core

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 color;
};

in vec3 fragmentNormal;
in vec3 fragmentPosition;
in vec2 fragmentTextureCoord;

out vec4 fragColor;

uniform sampler2D texture_diffuse1;
uniform DirectionalLight light;

void main() {
    vec3 ambient = light.ambient * texture(texture_diffuse1, fragmentTextureCoord).rgb;

    vec3 normFragmentNormal = normalize(fragmentNormal);
    vec3 normLightDir = normalize(-light.direction);
    float diff = max(dot(normFragmentNormal, normLightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, fragmentTextureCoord).rgb;

    vec3 specular = vec3(0.0f);

    fragColor = vec4((ambient + diffuse + specular) * light.color, 1.0f);
}
