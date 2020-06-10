//
//  Renderer.cpp
//  Marching Cube - Perlin Noise Terrains
//
//  Created by JuChen Lin on 4/5/20.
//  Copyright © 2020 JuChen Student. All rights reserved.
//

#define STB_IMAGE_IMPLEMENTATION

#include "Renderer.h"

nanogui::Screen* Renderer::nanogui_screen = nullptr;

Camera* Renderer::m_camera = new Camera();

Lighting* Renderer::m_lighting = new Lighting();

Object* Renderer::m_object = new Object();

Texture* Renderer::m_1D_texture = new Texture();

Pipeline* Renderer::m_pipeline = new Pipeline();

MarchingCube* Renderer::m_marchingcube = new MarchingCube();

bool Renderer::keys[1024];

Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}


void Renderer::init()
{
    // Init GLFW
    glfwInit();
    
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    m_camera->init();
    m_lighting->init(light_direction, LIGHT_SOURCE::DIRECTION);
    m_marchingcube->Init(m_object);
    
    // Create a GLFWwindow object
    m_window = glfwCreateWindow(m_camera->width, m_camera->height, "Final Project - Marching Cube", nullptr, nullptr);
    glfwMakeContextCurrent(m_window);
    glewExperimental = GL_TRUE;

    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();
    
    glViewport(0, 0, m_camera->width, m_camera->height);
}


// NanoGui
void Renderer::init_nanogui(GLFWwindow* window)
{
    // Create a nanogui screen and pass the glfw pointer to initialize
    nanogui_screen = new nanogui::Screen();
    nanogui_screen->initialize(window, true);
    
    nanogui_widget(nanogui_screen);
    
    nanogui_screen->setVisible(true);
    nanogui_screen->performLayout();
    
    glfwSetCursorPosCallback(window, [](GLFWwindow *, double x, double y) { nanogui_screen->cursorPosCallbackEvent(x, y); });
    
    glfwSetMouseButtonCallback(window, [](GLFWwindow *, int button, int action, int modifiers) { nanogui_screen->mouseButtonCallbackEvent(button, action, modifiers); });
    
    glfwSetKeyCallback(window, [](GLFWwindow *, int key, int scancode, int action, int mods) {
        if (key >= 0 && key < 1024)
        {
            if (action == GLFW_PRESS)
                keys[key] = true;
            else if (action == GLFW_RELEASE)
                keys[key] = false;
        }
    });
    
    glfwSetCharCallback(window, [](GLFWwindow *, unsigned int codepoint) { nanogui_screen->charCallbackEvent(codepoint); });
    
    glfwSetDropCallback(window, [](GLFWwindow *, int count, const char **filenames) { nanogui_screen->dropCallbackEvent(count, filenames); });
    
    glfwSetScrollCallback(window, [](GLFWwindow *, double x, double y) { nanogui_screen->scrollCallbackEvent(x, y); });
    
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height) { nanogui_screen ->resizeCallbackEvent(width, height); });
    
}

void Renderer::nanogui_widget(nanogui::Screen* nanogui_screen)
{
    nanogui::FormHelper *gui = new nanogui::FormHelper(nanogui_screen);
    nanogui::ref<nanogui::Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Nanogui control bar");

    gui->addGroup("Model Option");
    gui->addVariable("Model name", m_object->model, true)->setItems({ "Sphere", "Terrain_2DNoise", "Terrain_3DNoise" });
    gui->addButton("Reload model", [&]() {
        RunMarchingCube();
        m_camera->reset();
    });
    gui->addVariable("Enable Sea level", m_object->sea_level);
    gui->addVariable("Enable Terracing", m_object->terracing);
    gui->addVariable("2D Noise seed", m_object->seed_2D);
    gui->addVariable("3D Noise seed", m_object->seed_3D);
    
    gui->addGroup("Render Configuration");
    gui->addVariable("Render type", m_pipeline->render_type, true)->setItems({ "Points", "Lines", "Triangles" });
    gui->addVariable("Scale", m_marchingcube->scale)->setSpinnable(true);

    gui->addGroup("Camera");
    gui->addButton("Reset Camera", [&]() {
        m_camera->reset();
    });

}

// Display
void Renderer::display(GLFWwindow* window)
{
    // Build and compile our shader program
    Shader shader = Shader("shader/basic.vert", "shader/basic.frag");
    
    init_nanogui(m_window);

    RunMarchingCube();
    
    // Main frame while loop
    while (!glfwWindowShouldClose(m_window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        
        camera_move();
        
        shader.use();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, m_1D_texture->texture);
        glUniform1i(glGetUniformLocation(shader.program, "texture_1D"), 0);
        glGenerateMipmap(GL_TEXTURE_1D);

        draw_model(shader);
        
        set_uniform(shader);
        
        nanogui_screen->drawWidgets();
        
        // Swap the screen buffers
        glfwSwapBuffers(m_window);
    }
    
    glfwTerminate();
}

void Renderer::RunMarchingCube()
{
    m_marchingcube->Run(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(2.0f, 1.0f, 2.0f));

    link_vertex_attributes();

    load_colorber();
}

void Renderer::link_vertex_attributes()
{
    glGenVertexArrays(1, &vertexarray);
    
    glBindVertexArray(vertexarray);

    // Vertex Positions
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_marchingcube->vertices_position.size(), &m_marchingcube->vertices_position[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

    // Vertex Normals
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_marchingcube->vertices_normal.size(), &m_marchingcube->vertices_normal[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

    glBindVertexArray(0);
}

void Renderer::draw_model(Shader& shader)
{
    // Clear the colorbuffer
    glShadeModel(GL_SMOOTH);
    glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    m_pipeline->shading_model();
    m_pipeline->depth_test();
    m_pipeline->face_culling();
    m_pipeline->primitive_assem(vertexarray, m_marchingcube->vertices_position.size());
}

void Renderer::load_colorber()
{
    m_1D_texture->load_1D_texture();
}

void Renderer::set_uniform(Shader& shader)
{
    //Get uniform location and pass them to the shaders
    
    // Object
    GLint model_loc = glGetUniformLocation(shader.program, "model");
    GLint obj_color_loc = glGetUniformLocation(shader.program, "fs_obj.color");
    GLint obj_shininess_loc = glGetUniformLocation(shader.program, "fs_obj.shininess");
    GLint terrain_model_loc = glGetUniformLocation(shader.program, "fs_obj.is_terrain");
    GLint sea_level_loc = glGetUniformLocation(shader.program, "vs_obj.sea_level");
    GLint terracing_loc = glGetUniformLocation(shader.program, "vs_obj.terracing");
    
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(m_object->obj_matrix));
    glUniform3f(obj_color_loc, m_object->color[0], m_object->color[1], m_object->color[2]);
    glUniform1i(obj_shininess_loc, m_object->shininess);
    glUniform1i(terrain_model_loc, m_object->is_terrain);
    glUniform1i(sea_level_loc, m_object->sea_level);
    glUniform1i(terracing_loc, m_object->terracing);
    
    // Camera
    GLint camera_loc = glGetUniformLocation(shader.program, "camera_pos");
    GLint view_loc = glGetUniformLocation(shader.program, "view");
    GLint proj_loc = glGetUniformLocation(shader.program, "projection");
    
    glUniform3f(camera_loc, m_camera->abs_position[0], m_camera->abs_position[1], m_camera->abs_position[2]);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_camera->viewing_trans()));
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(m_camera->projection()));

    // Direction Lighting
    GLint ambient_dir_loc = glGetUniformLocation(shader.program, "direction_light.ambient");
    GLint diffuse_dir_loc = glGetUniformLocation(shader.program, "direction_light.diffuse");
    GLint specular_dir_loc = glGetUniformLocation(shader.program, "direction_light.specular");
    GLint light_dir_loc = glGetUniformLocation(shader.program, "direction_light.direction");

    glUniform3f(ambient_dir_loc, m_lighting->phong->ambient_color[0], m_lighting->phong->ambient_color[1], m_lighting->phong->ambient_color[2]);
    glUniform3f(diffuse_dir_loc, m_lighting->phong->diffuse_color[0], m_lighting->phong->diffuse_color[1], m_lighting->phong->diffuse_color[2]);
    glUniform3f(specular_dir_loc, m_lighting->phong->specular_color[0], m_lighting->phong->specular_color[1], m_lighting->phong->specular_color[2]);
    glUniform3f(light_dir_loc, m_lighting->direction[0], m_lighting->direction[1], m_lighting->direction[2]);
}


void Renderer::run()
{
    init();
    display(m_window);
}


void Renderer::camera_move()
{
    GLfloat current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    // Camera controls
    if (keys[GLFW_KEY_W])
        m_camera->process_keyboard(Camera_Movement::FORWARD, delta_time);
    if (keys[GLFW_KEY_S])
        m_camera->process_keyboard(Camera_Movement::BACKWARD, delta_time);
    if (keys[GLFW_KEY_A])
        m_camera->process_keyboard(Camera_Movement::LEFT, delta_time);
    if (keys[GLFW_KEY_D])
        m_camera->process_keyboard(Camera_Movement::RIGHT, delta_time);
    if (keys[GLFW_KEY_Q])
        m_camera->process_keyboard(Camera_Movement::UP, delta_time);
    if (keys[GLFW_KEY_E])
        m_camera->process_keyboard(Camera_Movement::DOWN, delta_time);
    if (keys[GLFW_KEY_I])
        m_camera->process_keyboard(Camera_Movement::LOCAL_ROTATE_X_UP, delta_time);
    if (keys[GLFW_KEY_K])
        m_camera->process_keyboard(Camera_Movement::LOCAL_ROTATE_X_DOWN, delta_time);
    if (keys[GLFW_KEY_J])
        m_camera->process_keyboard(Camera_Movement::LOCAL_ROTATE_Y_UP, delta_time);
    if (keys[GLFW_KEY_L])
        m_camera->process_keyboard(Camera_Movement::LOCAL_ROTATE_Y_DOWN, delta_time);
    if (keys[GLFW_KEY_U])
        m_camera->process_keyboard(Camera_Movement::LOCAL_ROTATE_Z_UP, delta_time);
    if (keys[GLFW_KEY_O])
        m_camera->process_keyboard(Camera_Movement::LOCAL_ROTATE_Z_DOWN, delta_time);
    if (keys[GLFW_KEY_H])
        m_camera->process_keyboard(Camera_Movement::GLOBAL_ROTATE_X_UP, delta_time);
    if (keys[GLFW_KEY_F])
        m_camera->process_keyboard(Camera_Movement::GLOBAL_ROTATE_X_DOWN, delta_time);
    if (keys[GLFW_KEY_T])
        m_camera->process_keyboard(Camera_Movement::GLOBAL_ROTATE_Y_UP, delta_time);
    if (keys[GLFW_KEY_G])
        m_camera->process_keyboard(Camera_Movement::GLOBAL_ROTATE_Y_DOWN, delta_time);
}
