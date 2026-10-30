#include "lin.hpp"
#include <cstddef>
#include <cstdlib>
#include <stdlib.h>
#include <algorithm>
#include <cstdint>
#include <utility>
#include <cmath>


static size_t WIDTH = 0;
static size_t HEIGHT = 0;
static uint32_t* PIXELS = nullptr;

struct V2{
  int x;
  int y;
};

void lin::init(size_t width, size_t height, uint32_t* pixels){
  PIXELS = pixels;
  WIDTH = width;
  HEIGHT = height;
}

void lin::changeRes(size_t width, size_t height, uint32_t *pixels){
  PIXELS = pixels;
  WIDTH = width;
  HEIGHT = height;
}

void lin::background(uint32_t color){
   for (size_t y = 0; y < HEIGHT; y++) {
     for (size_t x = 0; x < WIDTH; x++) {
       PIXELS[y * WIDTH + x] = color;
     }
   } 
}

void lin::drawPixel(int x, int y, uint32_t color){
    PIXELS[y * WIDTH + x] = color;
}

bool clipLeft(int& srcX, int& srcY, int& desX, int& desY) {

    if (srcX <  0 && desX <  0) return false;
    if (srcX >= 0 && desX >= 0) return true;

    float dx = float(desX - srcX);
    float dy = float(desY - srcY);

    float t = float(-srcX) / dx;
    int intersectionY = std::round(srcY + t * dy);

    if (srcX < 0) {
        srcX = 0;
        srcY = intersectionY;
    } else {
        desX = 0;
        desY = intersectionY;
    }

    return true;
}

bool clipRight(int& srcX, int& srcY, int& desX, int& desY) {

    if (srcX >= (int)WIDTH && desX >= (int)WIDTH) return false;
    if (srcX <  (int)WIDTH && desX <  (int)WIDTH) return true;

    float dx = float(desX - srcX);
    float dy = float(desY - srcY);

    float t = float((WIDTH - 1) - srcX) / dx;
    int intersectionY = std::round(srcY + t * dy);

    if (srcX >= (int)WIDTH) {
        srcX = WIDTH - 1;
        srcY = intersectionY;
    } else {
        desX = WIDTH - 1;
        desY = intersectionY;
    }

    return true;
}

bool clipTop(int& srcX, int& srcY, int& desX, int& desY) {

    if (srcY <  0 && desY <  0) return false;
    if (srcY >= 0 && desY >= 0) return true;

    float dx = float(desX - srcX);
    float dy = float(desY - srcY);

    float t = float(-srcY) / dy;
    int intersectionX = std::round(srcX + t * dx);

    if (srcY < 0) {
        srcY = 0;
        srcX = intersectionX;
    } else {
        desY = 0;
        desX = intersectionX;
    }

    return true;
}

bool clipBot(int& srcX, int& srcY, int& desX, int& desY) {

    if (srcY >= (int)HEIGHT && desY >= (int)HEIGHT) return false;
    if (srcY <  (int)HEIGHT && desY <  (int)HEIGHT) return true;

    float dx = float(desX - srcX);
    float dy = float(desY - srcY);

    float t = float((HEIGHT - 1) - srcY) / dy;
    int intersectionX = std::round(srcX + t * dx);

    if (srcY >= (int)HEIGHT) {
        srcY = HEIGHT - 1;
        srcX = intersectionX;
    } else {
        desY = HEIGHT - 1;
        desX = intersectionX;
    }

    return true;
}
void lin::drawLine(int srcX, int srcY, int desX, int desY, uint32_t color){

  
    if(!clipLeft (srcX,srcY,desX,desY)) return;
    if(!clipRight(srcX,srcY,desX,desY)) return;
    if(!clipTop  (srcX,srcY,desX,desY)) return;
    if(!clipBot  (srcX,srcY,desX,desY)) return;
    

  //bresenham's algorithm
  int dx = abs(desX - srcX);
  int dy = abs(desY - srcY);
  int sx = (srcX < desX) ? 1 : -1;
  int sy = (srcY < desY) ? 1 : -1;
  int err = dx - dy;

  //TODO:There's a cheap guard because clip still has some bugs and out of bounds vertices manage to get
  // trough, with an underflow int
  while (true) {
    if (srcX < 0 || srcY < 0 || srcX >= (int)WIDTH || srcY >= (int)HEIGHT) break;
    int offset = srcY * WIDTH + srcX;
    PIXELS[offset] = color;

    
    if ((srcX == desX && srcY == desY)) break;
    int e2 = 2* err;
    if (e2 > -dy) {
      err -= dy;
      srcX += sx;
    }
    if (e2 < dx) {
      err += dx;
      srcY += sy;
    }
  }
}

void lin::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color){
    lin::drawLine(x1, y1, x2, y2, color);
    lin::drawLine(x2, y2, x3, y3, color);
    lin::drawLine(x1, y1, x3, y3, color);
}

void lin::drawRect(int minX, int minY, int maxX, int maxY, uint32_t color){
  lin::drawLine(minX, minY, minX , maxY, color);
  lin::drawLine(minX, minY, maxX , minY, color);
  lin::drawLine(maxX, maxY, minX , maxY, color);
  lin::drawLine(maxX, minY, maxX, maxY,  color);
  
}

void lin::fillRect(int minX, int minY, int maxX, int maxY, uint32_t border, uint32_t body){
  lin::drawRect(minX, minY, maxX, maxY, border);
  
  for (int y = minY + 1; y < maxY; y++) {
    for (int x = minX + 1; x < maxX; x++) {
      lin::drawLine(x, y, maxX, y, body);
    }
  }
}

void plotCirclePoints(int cx, int cy, int x, int y, uint32_t color) {
  lin::drawPixel(cx + x, cy + y, color);
  lin::drawPixel(cx - x, cy + y, color);
  lin::drawPixel(cx + x, cy - y, color);
  lin::drawPixel(cx - x, cy - y, color);
  lin::drawPixel(cx + y, cy + x, color);
  lin::drawPixel(cx - y, cy + x, color);
  lin::drawPixel(cx + y, cy - x, color);
  lin::drawPixel(cx - y, cy - x, color);
}

void fillCirclePoints(int cx, int cy, int x, int y, uint32_t color) {
  
  lin::drawLine(cx + x, cy + y, cx - x, cy + y, color);
  lin::drawLine(cx + x, cy - y, cx - x, cy - y, color);
  lin::drawLine(cx + y, cy + x, cx - y, cy + x, color);
  lin::drawLine(cx + y, cy - x, cx - y, cy - x, color);
}

void lin::drawCircle(int centerX, int centerY, int radius, uint32_t color){
  
  int x = 0;
  int y = radius;
  int p = 1 - radius;

  while (x < y) {
    x++;
    if (p < 0) {
      p += 2 * x + 1;
    } else {
      y--;
      p += 2 * (x - y) + 1;
  }

  plotCirclePoints(centerX, centerY, x, y, color);
  }
}

void lin::fillCircle(int centerX, int centerY, int radius, uint32_t color){
  
  int x = 0;
  int y = radius;
  int p = 1 - radius;

  while (x < y) {
    x++;
    if (p < 0) {
      p += 2 * x + 1;
    } else {
      y--;
      p += 2 * (x - y) + 1;
  }

  fillCirclePoints(centerX, centerY, x, y, color);
  }
  lin::drawLine(centerX - radius, centerY, centerX + radius, centerY, color);
}
  
  
void plotEllipsePoints(int cx, int cy, int x, int y, uint32_t color) {
  lin::drawPixel(cx + x, cy + y, color);
  lin::drawPixel(cx - x, cy + y, color);
  lin::drawPixel(cx + x, cy - y, color);
  lin::drawPixel(cx - x, cy - y, color);
}

  void lin::drawEllipse(int major, int minor, int centerX, int centerY, uint32_t color){
    
  int dx, dy, d1, d2, x , y;    
  x = 0;
  y = minor;
  
  d1 = (minor * minor) - ( major * major) + (0.25 * major * major);
  dx = 2 * minor * minor * x;
  dy = 2 * major * major * y;

  while (dx < dy) {
  plotEllipsePoints(centerX, centerY, x, y, color);
    if (d1 < 0) {
      
      x++;
      dx = dx + (2 * minor * minor);
      d1 = d1 + dx + (minor * minor);
    }else{
      x++;
      y--;
      dx = dx + (2 * minor * minor);
      dy = dy - (2 * major *major);
      d1 = d1 + dx - dy + (minor * minor);
    }
  }


   d2 = ((minor * minor) * ((x + 0.5) * (x + 0.5)))
         + ((major * major) * ((y - 1) * (y - 1)))
         - (major * major * minor * minor);
   while (y >= 0) {
     plotEllipsePoints(centerX, centerY, x, y, color);

     if (d2 > 0) {
       y--;
       dy =dy - (2 * major * major);
       d2 = d2 + (major * major) - dy;
     }else{
       y--;
       x++;
       dx = dx + (2 * minor * minor);
       dy = dy - (2 * major * major);
       d2 = d2 +dx -dy + (major * major);
     }
   }
}
    
void fillEllipsePoints(int cx, int cy, int x, int y, uint32_t color) {
  lin::drawLine(cx + x, cy + y, cx - x, cy + y, color);
  lin::drawLine(cx + x, cy - y, cx - x, cy - y, color);
}

void lin::fillEllipse(int major, int minor, int centerX, int centerY, uint32_t color){
    
  int dx, dy, d1, d2, x , y;    
  x = 0;
  y = minor;
  
  d1 = (minor * minor) - ( major * major) + (0.25 * major * major);
  dx = 2 * minor * minor * x;
  dy = 2 * major * major * y;

  while (dx < dy) {
  fillEllipsePoints(centerX, centerY, x, y, color);
    if (d1 < 0) {
      
      x++;
      dx = dx + (2 * minor * minor);
      d1 = d1 + dx + (minor * minor);
    }else{
      x++;
      y--;
      dx = dx + (2 * minor * minor);
      dy = dy - (2 * major *major);
      d1 = d1 + dx - dy + (minor * minor);
    }
  }


   d2 = ((minor * minor) * ((x + 0.5) * (x + 0.5)))
         + ((major * major) * ((y - 1) * (y - 1)))
         - (major * major * minor * minor);
   while (y >= 0) {
     fillEllipsePoints(centerX, centerY, x, y, color);

     if (d2 > 0) {
       y--;
       dy =dy - (2 * major * major);
       d2 = d2 + (major * major) - dy;
     }else{
       y--;
       x++;
       dx = dx + (2 * minor * minor);
       dy = dy - (2 * major * major);
       d2 = d2 +dx -dy + (major * major);
     }
   }
}

static void fillFlatTopTriangle(int x0, int y0, int x1, int y1, int x2, int y2, 
                                uint32_t border, uint32_t body)
{
    // y0 == y1 (flat top), y2 is bottom
    float invSlope1 = 0, invSlope2 = 0;
    
    if (y2 != y0) invSlope1 = (float)(x2 - x0) / (float)(y2 - y0);
    if (y2 != y1) invSlope2 = (float)(x2 - x1) / (float)(y2 - y1);
    
    float curX1 = (float)x0;
    float curX2 = (float)x1;
    
    for (int y = y0; y <= y2; y++) {
        // Draw horizontal span from left to right
        lin::drawLine((int)curX1, y, (int)curX2, y, body);
        curX1 += invSlope1;
        curX2 += invSlope2;
    }
    
    // Draw edges if needed (optional - your other fills don't always draw border separately)
    lin::drawTriangle(x0, y0, x1, y1, x2, y2, border);
}

static void fillFlatBottomTriangle(int x0, int y0, int x1, int y1, int x2, int y2,
                                   uint32_t border, uint32_t body)
{
    // y0 top, y1 == y2 (flat bottom)
    float invSlope1 = 0, invSlope2 = 0;
    
    if (y1 != y0) invSlope1 = (float)(x1 - x0) / (float)(y1 - y0);
    if (y2 != y0) invSlope2 = (float)(x2 - x0) / (float)(y2 - y0);
    
    float curX1 = (float)x0;
    float curX2 = (float)x0;
    
    for (int y = y0; y <= y1; y++) {
        lin::drawLine((int)curX1, y, (int)curX2, y, body);
        curX1 += invSlope1;
        curX2 += invSlope2;
    }
    
    lin::drawTriangle(x0, y0, x1, y1, x2, y2, border);
}

void lin::fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, 
                       uint32_t border, uint32_t body)
{
    // Sort vertices by y ascending (top to bottom)
    struct V { int x, y; };
    V v[3] = { {x1,y1}, {x2,y2}, {x3,y3} };
    
    if (v[1].y < v[0].y) std::swap(v[1], v[0]);
    if (v[2].y < v[0].y) std::swap(v[2], v[0]);
    if (v[2].y < v[1].y) std::swap(v[2], v[1]);
    
    // Check if flat bottom
    if (v[1].y == v[2].y) {
        fillFlatBottomTriangle(v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y, border, body);
    }
    // Check if flat top
    else if (v[0].y == v[1].y) {
        fillFlatTopTriangle(v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y, border, body);
    }
    else {
        // Split at middle y: interpolate x on long edge
        int midY = v[1].y;
        float t = (float)(midY - v[0].y) / (float)(v[2].y - v[0].y);
        int midX = v[0].x + (int)((v[2].x - v[0].x) * t + 0.5f); // +0.5 for rounding if desired
        
        // Draw flat bottom (top part) and flat top (bottom part)
        fillFlatBottomTriangle(v[0].x, v[0].y, v[1].x, v[1].y, midX, midY, border, body);
        fillFlatTopTriangle(v[1].x, v[1].y, midX, midY, v[2].x, v[2].y, border, body);
        lin::drawLine(midX, midY, v[1].x, midY, body);
    }
}
