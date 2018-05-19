#version 330 core

layout(location = 0) in vec3 in_Position;

void main(void) {
    gl_Position.xyz = in_Position;
    gl_Position.w = 1.0;
}
