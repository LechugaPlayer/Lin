#pragma once
#include <vector>

struct Edge{
  int a;
  int b;
};

struct Vec3{
  float x;
  float y;
  float z;  
};

struct Mesh{
  std::vector<Vec3> vertices;
  std::vector<Edge> edges;

  static Mesh Cube();
  static Mesh UtahTeapot();
  static Mesh Pyramid();
};
