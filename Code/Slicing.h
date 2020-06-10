//
//  Slicing.h
//  Marching Cube - Perlin Noise Terrains
//
//  Created by JuChen Lin on 4/8/20.
//  Copyright © 2020 JuChen Student. All rights reserved.
//

#ifndef Slicing_h
#define Slicing_h

class Slicing
{
    struct View_Point {
        // Position
        glm::vec3 position;
        // Distance
        float distance;
    };
    
public:
    
    // Define vertices and indices as cube data
    std::vector<glm::vec3> sliced_vertices;
    std::vector<glm::vec3> sliced_texcoords;
    std::vector<glm::vec3> final_vertices;
    std::vector<glm::vec3> final_texcoords;
    
    int s0 = 20;
    int sample_rate = 10;
    int last_rate = 0;
    float relative_s;
    
    float slicing_part = 1.0f;
    
    float d_max_tx = 0;
    float d_min_tx = INFINITY;
    float d_max_tc = 0;
    float d_min_tc = INFINITY;
    
public:
    
    void reset()
    {
        d_max_tx = 0;
        d_min_tx = INFINITY;
        d_max_tc = 0;
        d_min_tc = INFINITY;
        
        sliced_vertices.clear();
        sliced_texcoords.clear();
        final_vertices.clear();
        final_texcoords.clear();
    }
    
    std::vector<glm::vec3> transfer_back(std::vector<glm::vec3> sliced_ponits, glm::mat4 view_mat_inv)
    {
        std::vector<glm::vec3> result;
        
        for(auto point : sliced_ponits)
        {
            point = glm::vec3(view_mat_inv * glm::vec4(point, 1.0));
            result.push_back(point);
        }
        
        return result;
    }
    
    void Tessellate(std::vector<glm::vec2> cube_edges, std::vector<glm::vec3> cube_vertices, std::vector<glm::vec3> cube_texcoords, glm::mat4 view_mat, glm::vec3 cube_front)
    {
        reset();
        
        relative_s = (float) s0/sample_rate;
        
        glm::mat4 view_mat_inv = glm::inverse(view_mat);
        
        std::vector<View_Point> view_vertices = point_struct(cube_vertices, view_mat, false);
        
        std::vector<glm::vec2> left_edges = cube_edges;
        
        int plane_num = (int) sample_rate * slicing_part;
        float spacing = (d_max_tx - d_min_tx) / (sample_rate);
        
        float d_plane_tx;
        float d_plane_tc;
        
        for(int i = 0; i < plane_num; i++)
        {
            d_plane_tx = d_max_tx - i * spacing;
            
            std::vector<glm::vec3> vertex_polygon = calculate_polygon(view_vertices, left_edges, spacing, d_plane_tx);
            
            glm::vec3 ref_center_tx = compute_center(vertex_polygon);
            
            std::vector<glm::vec3> sorted_polygon_tx = sort_points(vertex_polygon, ref_center_tx);
            
            glm::vec3 polygon_center_tx = compute_center(sorted_polygon_tx);
            
            make_triangles(sorted_polygon_tx, polygon_center_tx, d_plane_tx, false);
        }
        
        final_vertices = transfer_back(sliced_vertices, view_mat_inv);
    }
    
    
    std::vector<View_Point> point_struct(std::vector<glm::vec3> cube_points, glm::mat4 view_mat, bool is_textcoord)
    {
        std::vector<View_Point> result;
        
        float d_max = 0;
        float d_min = INFINITY;
        
        for(auto cube_point : cube_points)
        {
            View_Point point;
            glm::vec3 view_dir = glm::vec3(0.0f, 0.0f, 1.0f);
            
            cube_point = glm::vec3(view_mat * glm::vec4(cube_point, 1.0f));
            
            float d_vertex = view_dir[0] * cube_point[0] + view_dir[1] * cube_point[1] + view_dir[2] * cube_point[2];
            float distance = abs (d_vertex);
            
            point.position = glm::vec3(cube_point);
            point.distance = distance;
            
            d_max = std::max(point.distance, d_max);
            d_min = std::min(point.distance, d_min);
            
            result.push_back(point);
        }
        
        if(is_textcoord)
        {
            d_max_tc = d_max;
            d_min_tc = d_min;
        }
        else
        {
            d_max_tx = d_max;
            d_min_tx = d_min;
        }
        
        return result;
    }
    
    
    void make_triangles(std::vector<glm::vec3> sorted_polygon, glm::vec3 polygon_center, float d_plane, bool is_textcoord)
    {
        
        if(sorted_polygon.size() >= 3)
        {
            for(int j = 0; j < sorted_polygon.size(); j++)
            {
                int size = sorted_polygon.size();
                
                glm::vec3 vertex_1 = polygon_center;
                glm::vec3 vertex_2 = sorted_polygon[j  % size];
                glm::vec3 vertex_3 = sorted_polygon[(j + 1) % size];
                
                if(is_textcoord)
                {
                    sliced_texcoords.push_back(vertex_1);
                    sliced_texcoords.push_back(vertex_2);
                    sliced_texcoords.push_back(vertex_3);
                }
                else
                {
                    sliced_vertices.push_back(vertex_1);
                    sliced_vertices.push_back(vertex_2);
                    sliced_vertices.push_back(vertex_3);
                } 
            }
        }      
    }
    
    
    std::vector<glm::vec3> sort_points(std::vector<glm::vec3> polygon_points, glm::vec2 ref_center)
    {
        std::vector<glm::vec3> sorted_polygon;
        std::vector<float> sorted_pseudo_agl;
        
        for(auto point : polygon_points)
        {
            float pseudo_agl = Pseudoangle(ref_center, glm::vec2(point));
            
            int offset = 0;
            std::vector<float>::iterator it_agl;
            
            if(sorted_pseudo_agl.empty())
            {
                sorted_pseudo_agl.push_back(pseudo_agl);
                sorted_polygon.push_back(point);
            }
            else
            {
                // Sort vertices CCW
                for ( it_agl = sorted_pseudo_agl.begin() ; it_agl != sorted_pseudo_agl.end() ; it_agl++)
                {
                    if(pseudo_agl == *it_agl)
                    {
                        break;
                    }
                    else if(pseudo_agl < *it_agl)
                    {
                        sorted_pseudo_agl.insert(it_agl, pseudo_agl);
                        sorted_polygon.insert((sorted_polygon.begin() + offset), point);
                        break;
                    }
                    else if((pseudo_agl > *it_agl) && (it_agl == sorted_pseudo_agl.end()-1))
                    {
                        sorted_pseudo_agl.push_back(pseudo_agl);
                        sorted_polygon.push_back(point);
                        break;
                    }
                    
                    offset++;
                }
            }
        }
        
        return sorted_polygon;
    }
    
    std::vector<glm::vec3> calculate_polygon(std::vector<View_Point> view_points, std::vector<glm::vec2> left_edges, float spacing, float d_plane)
    {
        std::vector<glm::vec3> result;
        std::vector<glm::vec2>::iterator it;
        
        for ( it = left_edges.begin() ; it != left_edges.end() ; it++)
        {
            float d_vet0 = view_points[(*it)[0]].distance;
            float d_vet1 = view_points[(*it)[1]].distance;
            float d_0 = d_vet0 - d_plane;
            float d_1 = d_vet1 - d_plane;
            bool sign_0 = signbit(d_0);
            bool sign_1 = signbit(d_1);
            
            if((d_0 == 0) && (d_1 == 0))
            {
                result.push_back(view_points[(*it)[0]].position);
                result.push_back(view_points[(*it)[1]].position);
                
                left_edges.erase(it);
                it -= 1;
            }
            else if((d_0 == 0) && (d_1 != 0))
            {
                result.push_back(view_points[(*it)[0]].position);
            }
            else if((d_0 != 0) && (d_1 == 0))
            {
                result.push_back(view_points[(*it)[1]].position);
            }
            else if(sign_0 != sign_1)
            {
                float t = (d_plane - d_vet0)/(d_vet1 - d_vet0);
                
                glm::vec3 point = ( 1 - t ) * view_points[(*it)[0]].position + t * view_points[(*it)[1]].position;
                result.push_back(point);
            }
            else if(!sign_0 && !sign_1)
            {
                left_edges.erase(it);
                it -= 1;
            }
            
        }
        
        return result;
    }
    
    glm::vec3 compute_center(std::vector<glm::vec3> polygon)
    {
        glm::vec3 result;
        
        glm::vec2 sum = glm::vec2(0.0f, 0.0f);
        int count = polygon.size();
        
        for(auto vertex : polygon)
        {
            sum += glm::vec2(vertex);
        }
        result = glm::vec3(sum[0]/count, sum[1]/count, polygon[0][2]);
        
        return result;
    }
    
    float Pseudoangle(glm::vec2 p1, glm::vec2 p2) {
        
        glm::vec2 delta = p2 - p1;
        
        float result;
        
        if ((delta.x == 0) && (delta.y == 0)) {
            return -1;
        } else {
            result = delta.y / (abs(delta.x) + abs(delta.y));
            
            if (delta.x < 0.0) {
                result = 2.0f - result;
            } else {
                result = 4.0f + result;
            }
        }
        
        return result;
    }
};


#endif 
