#version 330 core

out vec4 outColor;

uniform vec3 color;

void main() {
    outColor = vec4(color, 1.0f);
}
