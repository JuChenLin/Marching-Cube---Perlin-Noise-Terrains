//
//  Texture.h
//  Marching Cube - Perlin Noise Terrains
//
//  Created by Priscilla Lin on 4/6/20.
//  Copyright © 2020 Priscilla Student. All rights reserved.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <algorithm>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Object.h"

class Texture
{
    
public:
    
    GLuint texture = 0;
    
    std::string texture_1D_name = "colorbar.png";
    
    bool status = 0;
    
public:
    
    Texture() {};
    
    ~Texture() {};
    
    void load_1D_texture()
    {
        std::string path = "image/terrain_colorbar.png";
        
        bind_1D_texture(path);
    }
    
    void bind_1D_texture(std::string path)
    {
        // Load image, create texture and generate mipmaps
        int width, height, channels;
        unsigned char *image = stbi_load(path.c_str(), &width, &height, &channels, 0);
        
        if (image)
        {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_1D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
            // Set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // Set texture filtering parameters
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            GLenum format;
            switch (channels)
            {
                case 1:
                    format = GL_LUMINANCE;
                    break;
                case 2:
                    format = GL_LUMINANCE_ALPHA;
                    break;
                case 3:
                    format = GL_RGB;
                    break;
                case 4:
                    format = GL_RGBA;
                    break;
            }
            
            std::cout << "Load 1D texture successfully =====" << std::endl;
            // set the texture wrapping/filtering options
            glTexImage1D(GL_TEXTURE_1D, 0, format, width, 0, format, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_1D);
        }
        else
        {
            std::cout << "Failed loading 1D texture =====" << std::endl;
        }
        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_1D, 0); // Unbind texture when done
    }  
};

