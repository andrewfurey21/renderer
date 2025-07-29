#version 330 core

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 color;
};

struct PointLight {
    vec3 position;

    float ambient;
    float diffuse;
    float specular;

    vec3 color;

    // point light
    float constant;
    float linear;
    float quadratic;
};

in vec3 fragmentNormal;
in vec3 fragmentPosition;
in vec2 fragmentTextureCoord;

out vec4 fragColor;

uniform sampler2D texture_diffuse1;
uniform DirectionalLight light;

#define num_lights 7
uniform PointLight plights[num_lights];

vec3 pointLight(PointLight light) {
    vec3 normLightDir = normalize(light.position - fragmentPosition);
    vec3 normFragmentNormal = normalize(fragmentNormal);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, fragmentTextureCoord));

    float diff = max(dot(normFragmentNormal, normLightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, fragmentTextureCoord));

    float dist = length(light.position - fragmentPosition);
    float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    return attenuation * (ambient + diffuse) * light.color;
}

void main() {
    vec3 ambient = light.ambient * texture(texture_diffuse1, fragmentTextureCoord).rgb;

    vec3 normFragmentNormal = normalize(fragmentNormal);
    vec3 normLightDir = normalize(-light.direction);
    float diff = max(dot(normFragmentNormal, normLightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, fragmentTextureCoord).rgb;

    vec3 specular = vec3(0.0f);

    fragColor = vec4((ambient + diffuse + specular) * light.color, 1.0f);

    for (int i = 0; i < num_lights; i++) {
        fragColor += vec4(pointLight(plights[i]), 0.0f);
    }
}
