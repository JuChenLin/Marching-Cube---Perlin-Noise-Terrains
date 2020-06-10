//
//  Pipeline.h
//  Marching Cube - Perlin Noise Terrains
//
//  Created by JuChen Lin on 4/6/20.
//  Copyright © 2020 JuChen Student. All rights reserved.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <algorithm>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

enum RENDER_TYPE
{
    POINTS,
    LINES,
    TRIANGLES
};

enum CULLING_TYPE
{
    CW,
    CCW
};

enum SHADING_TYPE
{
    FLAT,
    SMOOTH
};

enum DEPTH_TYPE
{
    LESS,
    ALWAYS
};


class Pipeline {
    
public:
    
    RENDER_TYPE render_type = TRIANGLES;
    
    CULLING_TYPE culling_type = CCW;
    
    SHADING_TYPE shading_type = SMOOTH;
    
    DEPTH_TYPE depth_type = LESS;
    
    bool flat_en = false;
    
    
public:
    
    void primitive_assem(GLuint vao, GLsizei size)
    {
        glBindVertexArray(vao);
        
        if (render_type == TRIANGLES)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawArrays(GL_TRIANGLES, 0, size);
        }
        
        if (render_type == LINES)
        {
            glLineWidth(2.0);
            
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawArrays(GL_TRIANGLES, 0, size);
        }
        
        if (render_type == POINTS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINTS);
            glDrawArrays(GL_POINTS, 0, size);
        }
        
        glBindVertexArray(0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    void face_culling()
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        
        if (culling_type == CW)
        {
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CW);
        }
        if (culling_type == CCW)
        {
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
        }
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    void shading_model()
    {
        if (shading_type == FLAT)
        {
            //glShadeModel(GL_FLAT);
            flat_en = true;
        }
        if (shading_type == SMOOTH)
        {
            //glShadeModel(GL_SMOOTH);
            flat_en = false;
        }
        
    }
    
    void depth_test()
    {
        glEnable(GL_DEPTH_TEST);
        
        if (depth_type == LESS)
        {
            glDepthFunc(GL_LESS);
        }
        if (depth_type == ALWAYS)
        {
            glDepthFunc(GL_ALWAYS);
        }
        
    }
    
};
