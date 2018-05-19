#version 330 core

layout(location = 0) in vec3 in_Position;

uniform mat4 projection;
uniform mat4 modelview;

void main(void) {
    gl_Position =  projection * modelview * vec4(in_Position, 1);
}
