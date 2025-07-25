#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 2) in vec2 textureCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 fragmentTextureCoord;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0f);
    fragmentTextureCoord = textureCoord;
}
