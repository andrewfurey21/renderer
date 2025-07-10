#version 330 core

in vec3 fragmentNormal;
in vec3 fragmentPosition;
in vec2 fragmentTextureCoord;

out vec4 fragColor;

uniform vec3 lightColor;
uniform vec3 cameraPosition;

uniform vec3 objectColor;

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

void main() {
    // vec3 ambient = light.ambient * lightColor * material.ambient;
    vec3 ambient = light.ambient * lightColor * vec3(texture(material.diffuse, fragmentTextureCoord));

    vec3 normFragmentNormal = normalize(fragmentNormal);
    vec3 normLightDir = normalize(light.position - fragmentPosition);
    float diff = max(dot(normFragmentNormal, normLightDir), 0.0f);
    // vec3 diffuse = light.diffuse * lightColor * (diff * material.diffuse);
    vec3 diffuse = light.diffuse * lightColor * (diff * vec3(texture(material.diffuse, fragmentTextureCoord)));

    vec3 normViewDirection = normalize(cameraPosition - fragmentPosition);
    vec3 reflectDirection = reflect(-normLightDir, normFragmentNormal);
    float spec = pow(max(dot(normViewDirection, reflectDirection), 0.0f), material.shininess);
    vec3 specular = light.specular * lightColor * (spec * material.specular);

    fragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0f);
}
