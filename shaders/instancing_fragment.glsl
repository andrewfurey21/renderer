#version 330 core

in vec3 inputColor;

out vec4 fragColor;

void main() {
    fragColor = vec4(inputColor, 1.0f);
}
