#version 330 core

in vec2 fragmentTextureCoord;

uniform sampler2D inputTexture;

out vec4 fragColor;

void main() {
    fragColor = texture(inputTexture, fragmentTextureCoord);
    if (fragColor.a < 0.1)
        discard;
    fragColor.rgb *= 0.5;
}
