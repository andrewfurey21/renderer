#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 light_view;
uniform mat4 light_proj;

out vec3 fragmentPosition;
out vec3 fragmentNormal;
out vec2 fragmentTextureCoords;
out vec4 fragmentPositionShadow;

void main() {
    fragmentPosition = vec3(model * vec4(pos, 1.0));
    fragmentNormal = transpose(inverse(mat3(model))) * normal;
    fragmentTextureCoords = textureCoords;
    fragmentPositionShadow = light_proj * light_view * vec4(fragmentPosition, 1.0);

    gl_Position = projection * view * vec4(fragmentPosition, 1.0);
}
