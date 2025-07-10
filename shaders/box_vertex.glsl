#version 330 core
layout(location = 0) in vec3 position;

out vec4 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 lightColor;
uniform vec4 color;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vertexColor = lightColor * color;
}
