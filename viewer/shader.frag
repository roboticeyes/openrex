#version 330 core

in vec3 normal;

out vec3 color;

void main(void) {
    vec3 n = normalize(normal);
    vec3 col = vec3(0.5,0.5,0.5);
    vec3 light = vec3(0.0,0.0,1.0);
    float cosTheta = clamp(dot(n,light),0.5,1);
    color = col * cosTheta;
}
