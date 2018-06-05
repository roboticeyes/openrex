#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out vec3 color;
out vec3 fragPos;

void main(void) {
    color = in_Color;
    fragPos = vec3(model * vec4(in_Position, 1));
    gl_PointSize = 5.0;
    gl_Position = projection * view * model * vec4(in_Position, 1);
}
