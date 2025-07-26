#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoords;
layout(location = 3) in mat4 model;

uniform mat4 projection;
uniform mat4 view;
uniform float time;

out vec3 fragmentPosition;
out vec3 fragmentNormal;
out vec2 fragmentTextureCoord;

void main() {
    float swayAmount = 0.2;
    float swaySpeed = 2.0;
    float swayFreq = 1.5;

    float phase = pos.x * 3.0 + pos.z * 2.5;

    float verticalFactor = pos.y;

    float sway = sin(time * swaySpeed + phase) * swayAmount * verticalFactor;

    vec3 adjusted_pos = pos;
    adjusted_pos.x += sway * 0.5;
    adjusted_pos.z += sway * 0.4;

    fragmentNormal = mat3(transpose(inverse(model))) * normal;
    fragmentPosition = vec3(model * vec4(pos + adjusted_pos, 1.0f));
    fragmentTextureCoord = textureCoords;
    gl_Position = projection * view * vec4(fragmentPosition, 1.0f); // Directional Light
}
