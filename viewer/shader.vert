#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec3 in_Color;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out vec3 normal;
out vec3 color;

void main(void) {
    normal = in_Normal;
    color = in_Color;
    gl_Position =  projection * view * model * vec4(in_Position, 1);
}
