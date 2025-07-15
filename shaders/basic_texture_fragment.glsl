#version 330 core

in vec2 fragmentTextureCoord;

out vec4 fragColor;

uniform sampler2D inputTexture;

void main() {
    fragColor = texture(inputTexture, fragmentTextureCoord);
}
