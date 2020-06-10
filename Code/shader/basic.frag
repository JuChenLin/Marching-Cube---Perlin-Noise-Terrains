#version 330 core

layout( location = 0 ) out vec4 color;

in VS_OUT {
    vec3 vertex_pos;
    vec3 vertex_normal;
    float height;
} fs_in;

// Object
struct FS_Object {
    vec3 color;
    int shininess;
    bool is_terrain;
};

// Lighting
struct Direction_Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
};

// Object
uniform FS_Object fs_obj;
// Light
uniform Direction_Light direction_light;
// Camera
uniform vec3 camera_pos;

uniform sampler1D texture_1D;
vec4 color_1D;

vec3 dir_blinn_phong(Direction_Light light, vec3 n, vec3 v){
    
    vec3 l = normalize(-light.direction);
    vec3 h = normalize(l + v);

    vec3 ambient_color = light.ambient;
    vec3 diffuse_color = clamp(dot(l , n), 0, 1) * light.diffuse;
    vec3 specular_color = pow(clamp(dot(n, h), 0, 1), fs_obj.shininess) * light.specular;

    vec3 blinn_phong_light = diffuse_color + specular_color;

    return blinn_phong_light;
}

vec3 color_mapping(float height)
{
    vec4 result;
    
    color_1D = texture(texture_1D, height);
        
    if(fs_obj.is_terrain)
    {
        result = vec4(color_1D.rgb, 1.0f);
    }
    else
    {
        result = vec4(fs_obj.color, 1.0f);
    }

    return result;
}

void main()
{
    vec3 n;
    vec3 v;

    n = normalize(fs_in.vertex_normal);
    v = normalize(camera_pos - fs_in.vertex_pos);

    vec3 dir_phong_light = dir_blinn_phong(direction_light, n, v);
    vec3 obj_color = color_mapping(fs_in.height);
    
    vec3 frag_color = dir_phong_light * obj_color;

    color = vec4(frag_color, 1.0f);
}

