#include "lin.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include "Mesh.hpp"
#include <vector>

constexpr float DEG2RAD = 3.14159265f / 180.0f;

struct Vec2{
  float x;
  float y;  
};

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1080;
constexpr float f = -150;


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

Vec3 Vec3sum(Vec3 lhs, Vec3 rhs){
    return Vec3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

struct Transform{
    Vec3 pos = {0,0,0};
    Vec3 rot = {0,0,0};
    float scl = {1.0f};
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
    
    Vec3 result = rotateX(v, r.x);
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

int main(){
    uint32_t* pixels = (uint32_t*) malloc(WIDTH * HEIGHT * sizeof(uint32_t));
    lin::init(WIDTH, HEIGHT, pixels);

    lin::background(lin::White);
    
    Mesh teapot = Mesh::UtahTeapot();
    Instance teapot1;
    teapot1.setMesh(teapot);
    teapot1.setScale(59.0f);
    teapot1.setPos({0,0,0});
    
    
    drawInstance(teapot1, lin::Black);
    
    writePPM("Test.ppm", pixels);
    return 0;
}
