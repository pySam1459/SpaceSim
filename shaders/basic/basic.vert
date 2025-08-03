#version 460 core

layout(location = 0) in vec3 aPos;

out vec3 vColor;

uniform mat4 u_mvp;
uniform vec3 u_color;

void main() {
    vColor = u_color;
    gl_Position = u_mvp * vec4(aPos, 1.0);
}
