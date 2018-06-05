#version 330 core

in vec3 color;
in vec3 fragPos;

out vec4 FragColor;

uniform vec3 lightPos;

void main(void) {
    vec3 lightCol = vec3(0.9, 0.9, 0.7);
    vec3 ambient = vec3(0.4,0.4,0.4);

    vec3 norm = vec3(0.0,1.0,0.0);
    vec3 lightDir = normalize(lightPos - fragPos);

    float diff = clamp(dot(norm, lightDir), 0.5, 1);
    vec3 diffuse = diff * lightCol;

    vec3 result = (ambient + diffuse) * color;
    FragColor = vec4(result, 1.0); // full opaque
}
