#version 410 core
layout (location = 0) in vec3 vp;
layout (location = 1) in vec3 aNormal;

uniform mat3 normalMatrix;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;


out vec3 Normal;
out vec3 FragPos;

void main() {
    //gl_Position = vec4(vp, 1.0);
    Normal = normalMatrix * aNormal;
    FragPos = vec3(model * vec4(vp, 1.0));

    gl_Position = projection * view * model * vec4(vp, 1.0);
}