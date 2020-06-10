//
//  Renderer.h
//  Marching Cube - Perlin Noise Terrains
//
//  Created by JuChen Lin on 4/5/20.
//  Copyright © 2020 JuChen Student. All rights reserved.
//

#pragma once

#define GLEW_STATIC
#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <nanogui/nanogui.h>
#include <nanogui/window.h>
#include <stb_image.h>

#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "Lighting.h"
#include "Pipeline.h"
#include "Texture.h"
#include "Slicing.h"
#include "MarchingCube.h"

class Renderer
{
public:
    GLFWwindow* m_window;
    
    GLfloat delta_time = 0.0;
    GLfloat last_frame = 0.0;
    
    //GLuint texture;
    GLuint vertexarray;
    GLuint vertexbuffer;
    GLuint normalbuffer;
    GLuint elementbuffer;
    GLuint texcoordbuffer;
    
    static nanogui::Screen* nanogui_screen;
    
    static Camera* m_camera;

    static Lighting* m_lighting;
    
    static Object* m_object;
    
    static Texture* m_1D_texture;
    
    static Pipeline* m_pipeline;

    static MarchingCube* m_marchingcube;
    
    std::vector<float> alpha = { 0.0f, 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    
    glm::mat4 world_mat = glm::mat4(1.0f);

    glm::vec3 background_color = glm::vec4(0.835, 0.811, 0.949, 1.0f);
    
    glm::vec3 light_direction = glm::vec3(0.0f, 1.0f, 0.5f);
    glm::vec3 light_position;
    
    glm::vec3 world_center = glm::vec3(0.0f, 0.0f, 0.0f);
    
    GLuint texture1;
    GLuint texture2;
    
    float slicing_part = 1.0f;
    
    bool trans_func = false;
    
    static bool keys[1024];

private:
    
    
public:
    Renderer();
    ~Renderer();
    
    void init();
    void run();
    
    void init_nanogui(GLFWwindow* window);

    void nanogui_widget(nanogui::Screen* nanogui_screen);

    void display(GLFWwindow* window);

    void load_colorber();
    
    void set_uniform(Shader& shader);
    
    void draw_model(Shader& shader);
    
    void link_vertex_attributes();

    void RunMarchingCube();
    
    void camera_move();
    
};
