//
//  Lighting.h
//  Marching Cube - Perlin Noise Terrains
//
//  Created by JuChen Lin on 4/6/20.
//  Copyright © 2020 JuChen Student. All rights reserved.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

enum class LIGHT_SOURCE
{
    DIRECTION,
    POINT,
    SPOT
};

class Lighting
{
    
public:
    
    struct Phong_Illumination {
        // Ambient light
        nanogui::Color ambient_color = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);
        // Diffuse light
        nanogui::Color diffuse_color = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);
        //Specular light
        nanogui::Color specular_color = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);
    };

    glm::mat4 model_rotated;
    
    glm::vec3 position;
    glm::vec3 direction;

    float rotate_angle = 0.0f;
    float rotate_angle_new = 0.0f;
    
    // Light status
    bool status = false;

private:

    LIGHT_SOURCE light_source = LIGHT_SOURCE::DIRECTION;
    
public:
    
    Phong_Illumination *phong = new Phong_Illumination();
    
    Lighting() {};
    
    ~Lighting() {};

    void init(glm::vec3 pos_dir, LIGHT_SOURCE source)
    {
        if (source == LIGHT_SOURCE::DIRECTION)
        {
            direction = pos_dir;
        }
        if (source == LIGHT_SOURCE::POINT)
        {
            position = pos_dir;
        }
    }
};
