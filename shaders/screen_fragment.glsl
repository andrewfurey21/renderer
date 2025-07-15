#version 330 core
out vec4 fragColor;

in vec2 fragmentTextureCoords;

uniform sampler2D screenTexture;

void main() {
    vec4 textureColor = texture(screenTexture, fragmentTextureCoords);
    // vec4 textureColor = texture(screenTexture, fragmentTextureCoords);
    fragColor = vec4(1.0 - textureColor.rgb, 1.0);
}
