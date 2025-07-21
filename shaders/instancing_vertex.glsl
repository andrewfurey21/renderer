#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
layout(location = 2) in mat4 model;

out vec3 inputColor;

uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * model * vec4(position, 0.0, 1.0);
    inputColor = color;
}
