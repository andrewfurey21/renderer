#version 330 core

in vec2 fragmentTextureCoord;

out vec4 fragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main() {
    fragColor = mix(texture(texture_diffuse1, fragmentTextureCoord), texture(texture_specular1, fragmentTextureCoord), 0.2);
}
