#version 330 core

layout(location = 0) in vec3 in_Position;

uniform mat4 MVP;

void main(void) {
    gl_Position =  MVP * vec4(in_Position, 1);
}
