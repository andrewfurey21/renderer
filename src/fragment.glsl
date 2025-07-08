#version 330 core

in vec4 vertexColor;
in vec2 vertexTextureCoord;

out vec4 fragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    // vec4 color1 = texture(texture1, vertexTextureCoord);
    // vec4 color2 = texture(texture2, vertexTextureCoord);
    // fragColor = mix(color1, color2, 1.0f);

    fragColor = mix(texture(texture1, vertexTextureCoord).rgba, texture(texture2, vertexTextureCoord).rgba, vec4(texture(texture2, vertexTextureCoord)).a * 0.2);
}
