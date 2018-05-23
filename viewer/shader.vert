#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out vec3 normal;

void main(void) {
    normal = in_Normal;
    gl_Position =  projection * view * model * vec4(in_Position, 1);
}
