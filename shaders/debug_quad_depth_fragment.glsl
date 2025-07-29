#version 330 core

in vec2 fragmentTextureCoords;

uniform sampler2D depthMap;

out vec4 fragColor;

void main() {
    float depthValue = texture(depthMap, fragmentTextureCoords).r;
    fragColor = vec4(vec3(depthValue), 1.0);
}
