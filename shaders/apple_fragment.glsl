#version 330 core

uniform vec3 color;

out vec4 fragColor;

int main() {
    fragColor = vec4(color, 1.0f);
}
