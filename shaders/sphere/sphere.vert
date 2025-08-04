#version 460 core

layout(location = 0) in vec3 a_pos;      // vertex position
layout(location = 1) in vec3 a_normal;   // vertex normal (from your mesh)

out VS_OUT {
    vec3 frag_pos;
    vec3 normal;
} vs_out;

uniform mat4 u_model;
uniform mat4 u_vp;

void main()
{
    // World-space position
    vec4 world_pos   = u_model * vec4(a_pos, 1.0);
    vs_out.frag_pos  = world_pos.xyz;

    // normal can be simply applied since scale is uniform across x/y/z
    vs_out.normal    = mat3(u_model) * a_normal;

    gl_Position = u_vp * world_pos;
}