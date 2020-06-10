//
//  Object.h
//  Marching Cube - Perlin Noise Terrains
//
//  Created by JuChen Lin on 4/14/20.
//  Copyright © 2020 JuChen Student. All rights reserved.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <nanogui/nanogui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Noise.h"

enum MODEL
{
    SPHERE,
    TERRAIN_2DNOISE,
    TERRAIN_3DNOISE
};

class Object
{
    
public:

    MODEL model = TERRAIN_3DNOISE;

    nanogui::Color color = nanogui::Color(0.8f, 0.7f, 0.0f, 1.0f);

    glm::mat4 obj_matrix = glm::mat4(1.0f);
    glm::vec3 sphere_center = glm::vec3(0.0f, 0.0f, 0.0f);

    int shininess = 32;

    float sphere_radius = 0.5;

    bool is_terrain = true;
    bool sea_level = false;
    bool terracing = false;

    int seed_2D = 221;
    int seed_3D = 50;

private:

    float SphereDensity(glm::vec3 position)
    {
        float result;
        float d = pow((position[0] - sphere_center[0]), 2) + pow((position[1] - sphere_center[1]), 2) + pow((position[2] - sphere_center[2]), 2);

        return result = sphere_radius - d;
    }

    float TerrainDensity(glm::vec3 position)
    {
        float result;

        double noise;

        // Create a PerlinNoise object with a random permutation vector generated with seed

        if (model == TERRAIN_2DNOISE)
        {
            seed_2D = 221;
            PerlinNoise pn(seed_2D);

            noise = OctavePerlin(pn, (double)position[0], 0.0, (double)position[2], 2, 0.5);
        }

        if (model == TERRAIN_3DNOISE)
        {
            seed_3D = 50;
            PerlinNoise pn(seed_3D);

            noise = OctavePerlin(pn, (double)position[0], (double)position[1], (double)position[2], 7, 0.5);
        }

        result = -position[1] + (float)noise;

        return result;
    }

    double OctavePerlin(PerlinNoise pn, double x, double y, double z, int octaves, double persistence) {

        double result;

        double totalVol = 0;
        double totalAmp = 0;
        double frequency = 2;
        double amplitude = 256;

        if (model == TERRAIN_2DNOISE)
        {
            totalVol += pn.noise(x * 0.4, y * 0.4, z * 0.4) * 1024;
            //totalVol += pn.noise(x * 1.6, y * 1.6, z * 1.6) * 256;
            totalVol += pn.noise(x * 1.2, y * 1.2, z * 1.2) * 256;
            totalVol += pn.noise(x * 4.8, y * 4.8, z * 4.8) * 32;
            totalVol += pn.noise(x * 48, y * 48, z * 48) * 4;

            /*for (int i = 1; i < octaves; i++) {

                amplitude *= persistence;
                frequency *= 2;

                totalVol += pn.noise(x * frequency, y * frequency, z * frequency) * amplitude;

                totalAmp += amplitude;
            }*/
            
            result = totalVol / (totalAmp + 1024 + 256);
            result -= 0.45;
            result *= 3.5;
        }
        else if (model == TERRAIN_3DNOISE)
        {
            totalVol += pn.noise(x, y, z) * 1024;

            for (int i = 1; i < octaves; i++) {

                amplitude *= persistence;
                frequency *= 2;

                totalVol += pn.noise(x * frequency, y * frequency, z * frequency) * amplitude;

                totalAmp += amplitude;
            }
            
            result = totalVol / (totalAmp + 1024);
            result -= 0.4;
            result *= 3;
        }

        return result;
    }
    
public:
    
    Object() {};

    float computeDensity(glm::vec3 position)
    {
        float result;
        
        if (model == SPHERE)
        {
            is_terrain = false;
            result = SphereDensity(position);
        }
        else
        {
            is_terrain = true;
            result = TerrainDensity(position);
        }

        return result;
    }

    ~Object() {};

};


