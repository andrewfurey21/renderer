#version 330 core

struct Material {
    // color material
    // vec3 diffuse;
    // vec3 specular;

    // texture material
    sampler2D diffuse;
    sampler2D specular;

    float shininess;
};

struct DirectionalLight {
    vec3 direction;

    float ambient;
    float diffuse;
    float specular;

    vec3 color;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    float ambient;
    float diffuse;
    float specular;

    vec3 color;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

in vec3 fragmentNormal;
in vec3 fragmentPosition;
in vec2 fragmentTextureCoord;

out vec4 fragColor;

uniform vec3 cameraPosition;
uniform Material material;
uniform DirectionalLight sun;
uniform SpotLight torch;

#define num_lights 4
uniform PointLight lights[num_lights];

vec3 directionalLight(DirectionalLight light) {
    vec3 normLightDir = normalize(-light.direction);
    vec3 normFragmentNormal = normalize(fragmentNormal);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fragmentTextureCoord));

    float diff = max(dot(normFragmentNormal, normLightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fragmentTextureCoord));

    vec3 normViewDirection = normalize(cameraPosition - fragmentPosition);
    vec3 reflectDirection = reflect(-normLightDir, normFragmentNormal);
    float spec = pow(max(dot(normViewDirection, reflectDirection), 0.0f), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fragmentTextureCoord));

    return (ambient + diffuse + specular) * light.color;
}

vec3 pointLight(PointLight light) {
    vec3 normLightDir = normalize(light.position - fragmentPosition);
    vec3 normFragmentNormal = normalize(fragmentNormal);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fragmentTextureCoord));

    float diff = max(dot(normFragmentNormal, normLightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fragmentTextureCoord));

    vec3 normViewDirection = normalize(cameraPosition - fragmentPosition);
    vec3 reflectDir = reflect(-normLightDir, normFragmentNormal);
    float spec = pow(max(dot(normViewDirection, reflectDir), 0.0f), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fragmentTextureCoord));

    float dist = length(light.position - fragmentPosition);
    float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    return attenuation * (ambient + diffuse + specular) * light.color;
}

vec3 spotLight(SpotLight light) {
    vec3 normFragmentNormal = normalize(fragmentNormal);
    vec3 normLightDir = normalize(light.position - fragmentPosition);
    float theta = dot(normLightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = vec3(texture(material.diffuse, fragmentTextureCoord));

    float diff = max(dot(normFragmentNormal, normLightDir), 0.0f);
    vec3 diffuse = diff * vec3(texture(material.diffuse, fragmentTextureCoord));

    vec3 normViewDirection = normalize(cameraPosition - fragmentPosition);
    vec3 reflectDirection = reflect(-normLightDir, normFragmentNormal);
    float spec = pow(max(dot(normViewDirection, reflectDirection), 0.0f), material.shininess);
    vec3 specular = spec * vec3(texture(material.specular, fragmentTextureCoord));

    float dist = length(light.position - fragmentPosition);
    float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    return attenuation * intensity * (ambient + diffuse + specular) * light.color;
}

void main() {
    vec3 color = vec3(0.0);

    color += directionalLight(sun);

    for (int i = 0; i < num_lights; i++) {
        color += pointLight(lights[i]);
    }

    color += spotLight(torch);

    fragColor = vec4(color, 1.0f);
}
