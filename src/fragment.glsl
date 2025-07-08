#version 330 core

in vec4 vertexColor;
out vec4 fragColor;
uniform float green;

void main() {
    fragColor = vec4(vertexColor.r, green, vertexColor.b, vertexColor.a);
}
