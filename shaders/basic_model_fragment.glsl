#version 330 core

in vec2 fragmentTextureCoord;

out vec4 fragColor;

uniform sampler2D texture_diffuse1;

void main() {
    fragColor = texture(texture_diffuse1, fragmentTextureCoord);
}
