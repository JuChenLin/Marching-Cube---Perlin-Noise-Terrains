#version 330 core

layout (points) in;
layout (points, max_vertices = 1) out;

out GS_OUT {
    vec3 vertex_pos;
    vec3 vertex_normal;
    float height;
} gs_out;

in VS_OUT {
    vec3 vertex_pos;
    vec3 vertex_normal;
    float height;
} gs_in[];

//out vec3 gs_color;

void main() {    
    
    gl_Position = (gl_in[0].gl_Position.x, max(0.0, gl_in[0].gl_Position.y), gl_in[0].gl_Position.z);
    gs_out.height = 
    EmitVertex();

    EndPrimitive();
}

