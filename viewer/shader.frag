#version 330 core

in vec3 color;
in vec3 normal;
in vec3 fragPos;

uniform vec3 lightPos;

void main(void) {
    vec3 lightCol = vec3(0.9, 0.9, 0.7);
    vec3 ambient = vec3(0.4,0.4,0.4);

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);

    float diff = clamp(dot(norm, lightDir), 0.1, 1);
    vec3 diffuse = diff * lightCol;

    vec3 result = (ambient + diffuse) * color;
    gl_FragColor = vec4(result, 1.0); // full opaque
}
