#version 330 core

in vec3 textureCoords;

out vec4 fragColor;

uniform samplerCube skyTexture;

void main() {
    fragColor = texture(skyTexture, textureCoords);
}
