#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out VS_OUT {
    vec3 vertex_pos;
    vec3 vertex_normal;

    float height;
} vs_out;

// Object
struct VS_Object {
    bool sea_level;
    bool terracing;
};

// Object
uniform VS_Object vs_obj;

// Verteices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec3 apply_seal_evel(vec3 pos) 
{
    vec3 result;

    float h = pos.y;

    if(vs_obj.sea_level) {
        result = vec3(pos.x, max(0.0, h), pos.z);
    }
    else {
        result = pos;
    }

    return result;
}

vec3 apply_terracing(vec3 pos)
{
    vec3 result;

    float h;

    if(vs_obj.terracing) {
        float h = floor( pos.y * 20) / 20;
        result = vec3(pos.x, h, pos.z);
    }
    else {
        result = pos;
    }

    return result;
}

void main(){

    vec3 sl_position = apply_seal_evel(position);
    vec3 te_position = apply_terracing(sl_position);
    
    gl_Position = projection * view * model * vec4(te_position, 1.0f);
    
    vs_out.height = position.y;

    vs_out.vertex_pos = vec3(model * vec4(position, 1.0f));
    vs_out.vertex_normal = normal;
}


