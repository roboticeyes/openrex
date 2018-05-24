#version 330 core

in vec3 color;
in vec3 normal;
in vec3 fragPos;

void main(void) {
    vec3 lightPos = vec3(1.0,1.0,1.0);
    vec3 lightCol = vec3(1.0,1.0,1.0);
    vec3 ambient = vec3(0.5,0.5,0.5);

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightCol;

    vec3 result = (ambient + diffuse) * color;
    gl_FragColor = vec4(result, 1.0); // full opaque
}
