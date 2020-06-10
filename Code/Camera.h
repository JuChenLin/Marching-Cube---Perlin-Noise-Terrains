//
//  Camera.h
//  Marching Cube - Perlin Noise Terrains
//
//  Created by JuChen Lin on 4/5/20.
//  Copyright © 2020 JuChen Student. All rights reserved.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <algorithm>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    LOCAL_ROTATE_X_UP,
    LOCAL_ROTATE_X_DOWN,
    LOCAL_ROTATE_Y_UP,
    LOCAL_ROTATE_Y_DOWN,
    LOCAL_ROTATE_Z_UP,
    LOCAL_ROTATE_Z_DOWN,
    GLOBAL_ROTATE_X_UP,
    GLOBAL_ROTATE_X_DOWN,
    GLOBAL_ROTATE_Y_UP,
    GLOBAL_ROTATE_Y_DOWN,
    GLOBAL_ROTATE_Z_UP,
    GLOBAL_ROTATE_Z_DOWN,
};

class Camera
{
    
public:
    
    // View matrix
    glm::mat4 view_matrix;
    // Projection matrix
    glm::mat4 projection_matrix;
    
    // Position
    glm::vec3 abs_position;
    // Direction
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 front;
    
    // World center
    glm::vec3 world_center = glm::vec3(0.0f, 0.0f, 0.0f);
    // Relative position
    glm::vec3 world_position = glm::vec3(0.0f, 5.0f, 6.0f);
    
    int width = 1600;
    int height = 900;
    
    float near = 0.1;
    float far = 100;
    float fovy = 40.0;
    
    float rotate_angle = 3.0f;
    
public:
    
    void init() {
        reset();
        abs_position = world_center + world_position;
    }
    
    void reset() {

        // Projection matrix
        projection_matrix = glm::mat4(1.0f);
        
        // Direction reset
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        right = glm::vec3(1.0f, 0.0f, 0.0f);

        set_position(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 5.0f));
    }
    
    void set_position(glm::vec3 center, glm::vec3 rel_pos) {
        
        world_center = center;
        world_position = rel_pos;
        abs_position = world_center + world_position;
        
    }
    
    // Rotate specific angle along local camera system(LCS)
    void rotate_x(int sign)
    {
        glm::mat4 rotation_mat = glm::mat4(1.0f);
        rotation_mat = glm::rotate(rotation_mat, glm::radians(sign * rotate_angle), right);
        up = glm::normalize( glm::vec3(rotation_mat * glm::vec4(up, 1.0)) );
        front = glm::normalize( glm::vec3(rotation_mat * glm::vec4(front, 1.0)) );
    }
    
    void rotate_y(int sign)
    {
        glm::mat4 rotation_mat = glm::mat4(1.0f);
        rotation_mat = glm::rotate(rotation_mat, glm::radians(sign * rotate_angle), up);
        front = glm::normalize( glm::vec3(rotation_mat * glm::vec4(front, 1.0)) );
        right = glm::normalize( glm::vec3(rotation_mat * glm::vec4(right, 1.0)) );
    }
    
    void rotate_z(int sign)
    {
        glm::mat4 rotation_mat = glm::mat4(1.0f);
        rotation_mat = glm::rotate(rotation_mat, glm::radians(sign * rotate_angle), front);
        right = glm::normalize( glm::vec3(rotation_mat * glm::vec4(right, 1.0)) );
        up = glm::normalize( glm::vec3(rotation_mat * glm::vec4(up, 1.0)) );
    }
    
    // Viewing Transformation
    glm::mat4 viewing_trans()
    {
        abs_position = world_center + world_position;
        
        view_matrix = glm::lookAt( world_center + world_position, world_center + world_position + front, up);
        
        return view_matrix;
    }
    
    // Perspective projection matrix
    glm::mat4 projection()
    {
        projection_matrix = glm::perspective(glm::radians(fovy), (float)width / (float)height, near, far);
        return projection_matrix;
    }
    
    void process_keyboard(Camera_Movement direction, GLfloat delta_time)
    {
        GLfloat move_velocity = delta_time * 10;
        GLfloat rotate_velocity = delta_time * 50;
        
        if (direction == Camera_Movement::FORWARD)
            this->world_position += this->front * move_velocity;
        if (direction == Camera_Movement::BACKWARD)
            this->world_position -= this->front * move_velocity;
        if (direction == Camera_Movement::LEFT)
            this->world_position -= this->right * move_velocity;
        if (direction == Camera_Movement::RIGHT)
            this->world_position += this->right * move_velocity;
        if (direction == Camera_Movement::UP)
            this->world_position += this->up * move_velocity;
        if (direction == Camera_Movement::DOWN)
            this->world_position -= this->up * move_velocity;
        if (direction == Camera_Movement::LOCAL_ROTATE_X_UP)
            local_rotate_x(rotate_velocity);
        if (direction == Camera_Movement::LOCAL_ROTATE_X_DOWN)
            local_rotate_x(-rotate_velocity);
        if (direction == Camera_Movement::LOCAL_ROTATE_Y_UP)
            local_rotate_y(rotate_velocity);
        if (direction == Camera_Movement::LOCAL_ROTATE_Y_DOWN)
            local_rotate_y(-rotate_velocity);
        if (direction == Camera_Movement::LOCAL_ROTATE_Z_UP)
            local_rotate_z(rotate_velocity);
        if (direction == Camera_Movement::LOCAL_ROTATE_Z_DOWN)
            local_rotate_z(-rotate_velocity);
        if (direction == Camera_Movement::GLOBAL_ROTATE_X_UP)
            global_rotate_x(rotate_velocity);
        if (direction == Camera_Movement::GLOBAL_ROTATE_X_DOWN)
            global_rotate_x(-rotate_velocity);
        if (direction == Camera_Movement::GLOBAL_ROTATE_Y_UP)
            global_rotate_y(rotate_velocity);
        if (direction == Camera_Movement::GLOBAL_ROTATE_Y_DOWN)
            global_rotate_y(-rotate_velocity);
    }
    
    // Rotate specific angle along local camera system(LCS)
    void local_rotate_x(GLfloat angle)
    {
        glm::vec3 up = this->up;
        glm::mat4 rotation_mat(1);
        rotation_mat = glm::rotate(rotation_mat, glm::radians(angle), this->right);
        this->up = glm::normalize(glm::vec3(rotation_mat * glm::vec4(up, 0.0)));
        this->front = glm::normalize(glm::cross(this->up, this->right));
    }
    
    void local_rotate_y(GLfloat angle)
    {
        glm::vec3 front = this->front;
        glm::mat4 rotation_mat(1);
        rotation_mat = glm::rotate(rotation_mat, glm::radians(angle), this->up);

        this->front = glm::normalize(glm::vec3(rotation_mat * glm::vec4(front, 0.0)));
        this->right = glm::normalize(glm::cross(this->front, this->up));
    }
    
    void local_rotate_z(GLfloat angle)
    {
        glm::vec3 right = this->right;
        glm::mat4 rotation_mat(1);
        rotation_mat = glm::rotate(rotation_mat, glm::radians(angle), this->front);

        this->right = glm::normalize(glm::vec3(rotation_mat * glm::vec4(right, 0.0)));
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }
    
    void global_rotate_x(GLfloat angle)
    {
        glm::vec3 cam_dir = world_position;
        glm::mat4 rotation_mat(1);
        rotation_mat = glm::rotate(rotation_mat, glm::radians(angle), this->up);

        glm::vec3 move_dir = glm::vec3(rotation_mat * glm::vec4(cam_dir, 1.0)) - cam_dir;
        world_position += move_dir;
        this->front = glm::normalize(glm::vec3(rotation_mat * glm::vec4(front, 0.0)));
    }
    
    void global_rotate_y(GLfloat angle)
    {
        glm::vec3 cam_dir = world_position;
        glm::mat4 rotation_mat(1);
        rotation_mat = glm::rotate(rotation_mat, glm::radians(-angle), this->right);

        glm::vec3 move_dir = glm::vec3(rotation_mat * glm::vec4(cam_dir, 1.0)) - cam_dir;
        world_position += move_dir;
        this->front = glm::normalize(glm::vec3(rotation_mat * glm::vec4(front, 0.0)));
    }
    
    
};
