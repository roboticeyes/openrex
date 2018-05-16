#version 330 core

// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
attribute vec3 in_Position;
attribute vec4 in_Color;

// We output the ex_Color variable to the next shader in the chain
out vec4 ex_Color;

void main(void) {
    // Since we are using flat lines, our input only had two points: x and y.
    // Set the Z coordinate to 0 and W coordinate to 1
    gl_Position = vec4(in_Position.x, in_Position.y, in_Position.z, 1.0);

    // Pass the color on to the fragment shader
    ex_Color = in_Color;
}
