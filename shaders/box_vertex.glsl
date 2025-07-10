#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

// gets interpolated per fragment
out vec3 fragmentNormal;
out vec3 fragmentPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    fragmentNormal = normal;
    fragmentPosition = vec3(model * vec4(position, 1.0f));
}
