#include "lin.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <vector>

struct Vec3{
    float x;
    float y;
    float z;    
};

struct Vec2{
  float x;
  float y;  
};

constexpr int WIDTH = 600;
constexpr int HEIGHT = 600;
constexpr float f = 100;
uint32_t* pixels = nullptr;

bool writePPM( const char* path){
    FILE* file = std::fopen(path, "wb");

    if (!file) {
        return false;
    }
    std::fprintf(file, "P6\n%u %u\n 255 \n",
                 WIDTH, HEIGHT);
    for (uint32_t y = 0; y < HEIGHT; y++) {
        for (uint32_t x = 0; x < WIDTH; x++) {
           uint32_t pixel = pixels[y * WIDTH + x];

           uint8_t r = (pixel >> 16 ) & 0xFF;
           uint8_t g = (pixel >> 8 ) & 0xFF;
           uint8_t b = (pixel >> 0) & 0xFF;

           
            std::fputc(g, file);
            std::fputc(b, file);
            std::fputc(r, file);
        }
        
    }
    std::fclose(file);

    return true;
}

Vec2 project(Vec3 v, float f){
    return {f * (v.x/v.z), f * (v.y/v.z)};
}

Vec2 rasterSpace(Vec2 v, int width, int height){
    return {v.x + ((float)width/2), -v.y + ((float)height/2)};
}

struct Edge{
    int a;
    int b;
};

Vec3 Vec3sum(Vec3 lhs, Vec3 rhs){
    return Vec3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

struct Mesh{
    std::vector<Vec3> vertices;
    std::vector<Edge> edges;

void draw(Vec3 pos, uint32_t outline){
    std::vector<Vec2> projectedVertices;
    projectedVertices.reserve(this->vertices.size());
    for (size_t i = 0; i < this->vertices.size(); i++) {
        projectedVertices.emplace_back(project(Vec3sum(this->vertices[i], pos),f));
    }
    
    std::vector<Vec2> rasterVertices;
    rasterVertices.reserve(this->vertices.size());

    for (size_t i = 0; i < this->vertices.size(); i++){
        rasterVertices.emplace_back(rasterSpace(projectedVertices[i], WIDTH, HEIGHT));
    }
    
    for (size_t i = 0; i < this->edges.size(); i++) {
        Vec2 begin = rasterVertices[this->edges[i].a];
        Vec2 end   = rasterVertices[this->edges[i].b];
        lin::drawLine(begin.x, begin.y, end.x, end.y, outline);
        }
    }

static Mesh makeCube(){
    //Front vertices
    Vec3 ver1 = {-50, -50, -50};
    Vec3 ver2 = {50, -50, -50};    
    Vec3 ver3 = {-50, 50, -50};
    Vec3 ver4 = {50, 50, -50};
    //Back vertices0    0
    Vec3 ver5 = {-50, -50, 50};
    Vec3 ver6 = {50, -50, 50};
    Vec3 ver7 = {-50, 50, 50};
    Vec3 ver8 = {50, 50, 50};
    
    Mesh cube;
    cube.vertices.emplace_back(ver1);    
    cube.vertices.emplace_back(ver2);    
    cube.vertices.emplace_back(ver3);    
    cube.vertices.emplace_back(ver4);    
    cube.vertices.emplace_back(ver5);    
    cube.vertices.emplace_back(ver6);    
    cube.vertices.emplace_back(ver7);    
    cube.vertices.emplace_back(ver8);

    cube.edges.emplace_back(Edge{0, 1});
    cube.edges.emplace_back(Edge{1, 3});
    cube.edges.emplace_back(Edge{2, 3});
    cube.edges.emplace_back(Edge{2, 0});

    cube.edges.emplace_back(Edge{4, 5});
    cube.edges.emplace_back(Edge{5, 7});
    cube.edges.emplace_back(Edge{6, 4});
    cube.edges.emplace_back(Edge{7, 6});
    
    
    cube.edges.emplace_back(Edge{0, 4});
    cube.edges.emplace_back(Edge{1, 5});
    cube.edges.emplace_back(Edge{2, 6});
    cube.edges.emplace_back(Edge{3, 7});

    return cube;
}

};

struct Instance{
    //this means I cant' change the mesh pointer?
    const Mesh* mesh = nullptr;
    Vec3 pos;
    //          p y r  
    Vec3 rot = {0,0,0};
    float scl;
    
    void setMesh(const Mesh& m){
        this->mesh = &m;
    }
    void translate(Vec3 pos){
        
    }

    void rotate(Vec3 rot){
        
    }

    void scale(float scl){
        
    }
};

struct Scene{
  std::vector<Instance> instances;  
};

int main(){
    pixels = (uint32_t*)malloc(WIDTH * HEIGHT * sizeof(uint32_t));
    lin::init(WIDTH, HEIGHT);

    Scene scene;

    lin::background(lin::LightGray);    
    Mesh Cube = Mesh::makeCube();
    Instance cube1;
    cube1.setMesh(Cube);
    /*
    cube.draw({WIDTH/2 - 101, HEIGHT/2 - 101, 99}, lin::Black);
    
    cube.draw({100, 100,100}, lin::Black);
    cube.draw({-100, -100,100}, lin::Black);
    cube.draw({100, -100,100}, lin::Black);
    cube.draw({-100, 100,100}, lin::Black);

    
    cube.draw({0, 0,100}, lin::Black);
    */

    //position is located at a the bottom-lef
    
    

        writePPM("Test.ppm");
    return 0;
}
