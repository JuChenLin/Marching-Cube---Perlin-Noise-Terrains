//
//  MarchingCube.cpp
//  Marching Cube - Perlin Noise Terrains
//
//  Created by JuChen Lin on 4/8/20.
//  Copyright © 2020 JuChen Student. All rights reserved.
//

#include "MarchingCube.h"

//MarchingCube::MarchingCube(){}

MarchingCube::~MarchingCube(){}

GLvoid MarchingCube::Init(Object* object)
{
    //std::cout << "MarchingCube Initializing ......" << std::endl;
    this->object = object;
}

GLvoid MarchingCube::Run(glm::vec3 corner_1, glm::vec3 corner_2)
{
    //std::cout << "MarchingCube RUNNING ......" << std::endl;

    reset();

    std::vector<glm::vec3> current_cube(8);

    int length_x = abs(corner_2[0] - corner_1[0]);
    int length_y = abs(corner_2[1] - corner_1[1]);
    int length_z = abs(corner_2[2] - corner_1[2]);
    
    for (int i = 0; i < 8; i++)
    {
        current_cube[i] = corner_1 + glm::vec3( length_x * WorldUnitCube[i][0], length_y * WorldUnitCube[i][1], length_z * WorldUnitCube[i][2] );
    }

    //DiceToVexel(current_cube, i, length_x, length_y, length_z);

    // Dice world to vexel cubes and perform marching cube
    int total_vexel = length_x * length_y * length_z * pow(scale, 3);

    for (int i = 0; i < total_vexel; i++)
    {
        //  Find cube vertices
        std::vector<glm::vec3> cube_vertices = VexelCube(current_cube, i, length_x, length_y, length_z);
        
        MarchCube(cube_vertices);
    }
}

// VexelCube compute 8 vertices of the vexel cube 
std::vector<glm::vec3> MarchingCube::VexelCube(std::vector<glm::vec3> current_cube, int number, int length_x, int length_y, int length_z)
{
    std::vector<glm::vec3> result;

    float grid = 1.0f / scale;

    int index_z = number / (scale * length_x * scale * length_y);
    int index_y = (number % (scale * length_x * scale * length_y)) / (scale * length_x);
    int index_x = number % (scale * length_x * scale * length_y) % (scale * length_x);

    glm::vec3 vertex0 = glm::vec3((current_cube[0][0] + index_x * grid), (current_cube[0][1] + index_y * grid), (current_cube[0][2] + index_z * grid));

    for (int i = 0; i < 8; i++)
    {
        glm::vec3 vertex = vertex0 + grid * glm::vec3(WorldUnitCube[i][0], WorldUnitCube[i][1], WorldUnitCube[i][2]);
        result.push_back(vertex);
    }

    return result;
}

//MarchCube performs Marching Cubes algorithm on a single cube
GLvoid MarchingCube::MarchCube(std::vector<glm::vec3> cube_vertices)
{
    VerticesStatus vertices_status;

    std::vector<glm::vec3> intersect_pos(12, glm::vec3(0.0f));
    
    vertices_status = GetVerticesStatus(cube_vertices);

    //If the cube is entirely inside or outside of the surface, then there will be no intersections
    if (CubeEdgeFlags[vertices_status.flag_index] == 0)
    {
        return;
    }
    
    //Find the point of intersection of the surface with each edge
    //Then find the normal to the surface at those points
    for (int i = 0; i < 12; i++)
    {
        //if there is an intersection on this edge
        if (CubeEdgeFlags[vertices_status.flag_index] & (1 << i))
        {
            float density_0 = vertices_status.vertices_density[CubeEdges[i][0]];
            float density_1 = vertices_status.vertices_density[CubeEdges[i][1]];
            float ratio = (0 - density_0) / (density_1 - density_0);

            intersect_pos[i] = (1 - ratio) * cube_vertices[CubeEdges[i][0]] + ratio * cube_vertices[CubeEdges[i][1]];
        }

    }

    //Draw the triangles that were found.  There can be up to five per cube
    for (int i = 0; i < 5; i++)
    {
        if (TriangleConnectionTable[vertices_status.flag_index][3 * i] < 0)
            break;

        std::vector<glm::vec3> single_triangle(3);

        for (int j = 0; j < 3; j++)
        {
            int edge_index = TriangleConnectionTable[vertices_status.flag_index][3 * i + j];

            glm::vec3 triangle_vertex = intersect_pos[edge_index];

            single_triangle[j] = triangle_vertex;
            vertices_position.push_back(triangle_vertex);
        }

        glm::vec3 normal = ComputeNormal(single_triangle);

        for (int j = 0; j < 3; j++)
        {
            vertices_normal.push_back(normal);
        }
    }
}

MarchingCube::VerticesStatus MarchingCube::GetVerticesStatus(std::vector<glm::vec3> cube_vertices)
{
    VerticesStatus result = { 0x00, std::vector<float>(8, 0.0f) };

    //Find which vertices which are inside of the surface and which are outside
    for (int i = 0; i < 8; i++)
    {
        float density = object->computeDensity(cube_vertices[i]);
        result.vertices_density[i] = density;

        if (density >= 0)
            result.flag_index |= 1 << i; 
    }

    return result;
}

//Triangle normal for lighting calculations
glm::vec3 MarchingCube::ComputeNormal(std::vector<glm::vec3> points)
{
    glm::vec3 result;

    glm::vec3 v1 = points[1] - points[0];
    glm::vec3 v2 = points[2] - points[0];

    result = cross(v1, v2);

    return result;
}

GLvoid MarchingCube::reset() 
{
    vertices_position.clear();
    vertices_normal.clear();
}

//GLvoid MarchingCube::DiceToVexel(vector<vec3> current_cube, int i, int length_x, int length_y, int length_z)
//{
//    // Dice world to vexel cubes and perform marching cube
//    int total_vexel = length_x * length_y * length_z * pow(scale, 3);
//
//    for (int i = 0; i < total_vexel; i++)
//    {
//        //  Find cube vertices
//        vector<vec3> cube_vertices = VexelCube(current_cube, i, length_x, length_y, length_z);
//       
//        MarchCube(cube_vertices);
//    }
//}

