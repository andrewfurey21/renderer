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

uniform sampler2D inputTexture;
uniform DirectionalLight light;

out vec4 fragColor;

void main() {
    vec4 textureValue = texture(inputTexture, fragmentTextureCoord);
    vec3 ambient = light.ambient * textureValue.rgb;

    vec3 normFragmentNormal = normalize(fragmentNormal);
    vec3 normLightDir = normalize(-light.direction);
    float diff = max(dot(normFragmentNormal, normLightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * textureValue.rgb;

    vec3 specular = vec3(0.0f);

    fragColor = vec4((ambient + diffuse + specular) * light.color, textureValue.a);

    if (fragColor.a < 0.1)
        discard;
}
