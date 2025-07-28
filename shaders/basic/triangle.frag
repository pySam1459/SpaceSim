#version 460 core

in vec3 vColor;
out vec4 FragColor;
uniform float uTime;

void main() {
    float b1 = 0.5 + 0.5 * sin(uTime);
    float b2 = 0.5 + 0.5 * cos(uTime);
    FragColor = vec4(vColor.r * b1, vColor.g * b2, vColor.b, 1.0);
}