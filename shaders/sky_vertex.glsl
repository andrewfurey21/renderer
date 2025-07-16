#version 330 core

layout(location = 0) in vec3 position;

out vec3 textureCoords;

uniform mat4 projection;
uniform mat4 view;
// with a cubemap, model is the identity matrix

void main()
{
    textureCoords = position;
    vec4 pos = projection * view * vec4(position, 1.0f);
    gl_Position = pos.xyww;
}
