#version 460 core

in VS_OUT {
    vec3 frag_pos;
    vec3 normal;
} fs_in;

out vec4 frag_color;

uniform bool u_enable_light;
uniform vec3 u_light_pos; // point light in world space
uniform vec3 u_view_pos; // camera position
uniform vec3 u_albedo; // base colour

// tweakables
const float ambient_strength = 0.10;
const float spec_strength    = 0.30;
const float shininess        = 32.0;     // bigger = tighter hotspot
  
void main()
{
    if (!u_enable_light) {
        frag_color = vec4(u_albedo, 1.0);
        return;
    }
    // Surface data
    vec3 N = normalize(fs_in.normal);
    vec3 V = normalize(u_view_pos - fs_in.frag_pos);

    // Light contribution
    vec3 L = normalize(u_light_pos - fs_in.frag_pos);
    float diff = max(dot(N, L), 0.0);

    // Blinn–Phong specular
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), shininess);

    vec3 ambient  = ambient_strength * u_albedo;
    vec3 diffuse  = diff             * u_albedo;
    vec3 specular = spec_strength    * spec * vec3(1.0); // white highlights

    frag_color = vec4(ambient + diffuse + specular, 1.0);
}