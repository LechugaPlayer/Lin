#include "lin.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <deque>
#include <vector>

constexpr float DEG2RAD = 3.14159265 / 180.0f;

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


bool writePPM( const char* path, uint32_t* pixels){
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

static Mesh Cube(){
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
struct Transform{
    Vec3 pos = {0,0,0};
    Vec3 rot = {0,0,0};
    float scl = {1};
};

Vec3 rotateX(Vec3 v, float a){
    float c = std::cos(a);
    float s = std::sin(a);
    return {v.x, v.y * c - v.z * s, v.y * s + v.z * c};
}

Vec3 rotateY(Vec3 v, float a){
    float c = std::cos(a);
    float s = std::sin(a);
    return {v.x * c + v.z * s, v.y, -v.x * s + v.z *c};
}

Vec3 rotateZ(Vec3 v, float a){
    float c = std::cos(a);
    float s = std::sin(a);
    return {v.x * c - v.y * s, v.x * s + v.y * c, v.z};
}

Vec3 rotate(const Vec3& v, const Vec3& r){
    Vec3 result;
    result = rotateX(v, r.x);
    result = rotateY(result, r.y);
    result = rotateZ(result, r.z);

    return result;
}
//local to world
Vec3 applyTransform(const Transform& t, Vec3 v){
    Vec3 p { v.x * t.scl, v.y * t.scl, v.z * t.scl}; //scale
    p = rotate(p, t.rot);                            //rotate
    return Vec3sum(p, t.pos);                        //translate 
}

struct Instance{
    const Mesh* mesh = nullptr;
    Transform xform;

    void setMesh(const Mesh& m){ this->mesh = &m; }
    void setMesh(const Mesh&&) = delete;
    
    void setPos(Vec3 p){
        this->xform.pos = p;
    }
    //X axis
    void setPitch(float deg){
        this->xform.rot.x = deg * DEG2RAD;
    }
    //Y axis
    void setYaw(float deg){
        this->xform.rot.y = deg * DEG2RAD;
    }
    //Z axis
    void setRoll(float deg){
        this->xform.rot.z = deg * DEG2RAD;
    }

    void setScale(float s){
        this->xform.scl = s;
    }
};

void drawInstance(const Instance& inst, uint32_t outline){
    if (!inst.mesh) return;
    const Mesh& mesh = *inst.mesh;

    std::vector<Vec2> screen;
    screen.reserve(mesh.vertices.size());
    for (size_t i = 0; i < mesh.vertices.size(); i++){
        //Local to World here
        Vec3 world = applyTransform(inst.xform, mesh.vertices[i]);
        screen.emplace_back(rasterSpace(project(world, f), WIDTH, HEIGHT));
    }

    for (size_t i = 0; i < mesh.edges.size(); i++){
        Vec2 begin = screen[mesh.edges[i].a];
        Vec2 end   = screen[mesh.edges[i].b];
        lin::drawLine((int)begin.x, (int)begin.y, (int)end.x, (int)end.y, outline);
    }
}
struct Scene{
  std::deque<Mesh> meshes;
  std::vector<Instance> instances;  
};

int main(){
    
    lin::init(WIDTH, HEIGHT);
    uint32_t* pixels = lin::framebuffer();

    lin::background(lin::White);    
    Mesh Cube = Mesh::Cube();
    Instance cube1;
    cube1.setMesh(Cube);
    cube1.setScale(0.50f);
    cube1.setPos({90,0,100});
    cube1.setPitch(30);
    

    Instance cube2;
    cube2.setMesh(Cube);
    cube2.setScale(0.50f);
    cube2.setPos({-90,0,100});
    cube2.setYaw(15);


    Instance cube3;
    cube3.setMesh(Cube);
    cube3.setScale(0.50f);
    cube3.setPos({0,0,100});
    cube3.setRoll(30);
    
    drawInstance(cube1, lin::Black);
    drawInstance(cube2, lin::Black);
    drawInstance(cube3, lin::Black);
    
    writePPM("Test.ppm", pixels);
    return 0;
}
