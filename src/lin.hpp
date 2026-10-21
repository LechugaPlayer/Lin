#include <cstddef>
#include <cstdint>

namespace lin {
using Color = uint32_t;

constexpr Color Transparent = 0x00000000;
constexpr Color Black       = 0xFF000000;
constexpr Color White       = 0xFFFFFFFF;

constexpr Color Red         = 0xFFFF0000;
constexpr Color Green       = 0xFF00FF00;
constexpr Color Blue        = 0xFF0000FF;

constexpr Color Yellow      = 0xFFFFFF00;
constexpr Color Cyan        = 0xFF00FFFF;
constexpr Color Magenta     = 0xFFFF00FF;

constexpr Color Gray        = 0xFF808080;
constexpr Color DarkGray    = 0xFF404040;
constexpr Color LightGray   = 0xFFC0C0C0;

constexpr Color Orange      = 0xFFFFA500;
constexpr Color Purple      = 0xFF800080;
constexpr Color Pink        = 0xFFFFC0CB;
constexpr Color Brown       = 0xFFA52A2A;

constexpr Color Navy        = 0xFF000080;
constexpr Color Teal        = 0xFF008080;
constexpr Color Olive       = 0xFF808000;
constexpr Color Lime        = 0xFF00FF00;

constexpr Color Maroon      = 0xFF800000;
constexpr Color Silver      = 0xFFC0C0C0;
constexpr Color Gold        = 0xFFFFD700;

constexpr Color SkyBlue     = 0xFF87CEEB;
constexpr Color RoyalBlue   = 0xFF4169E1;
constexpr Color Violet      = 0xFFEE82EE;
constexpr Color Indigo      = 0xFF4B0082;

constexpr Color DarkRed     = 0xFF8B0000;
constexpr Color DarkGreen   = 0xFF006400;
constexpr Color DarkBlue    = 0xFF00008B;

constexpr Color LightRed    = 0xFFFF6666;
constexpr Color LightGreen  = 0xFF66FF66;
constexpr Color LightBlue   = 0xFF6666FF;

    void init(size_t width, size_t height);
    void shutdown();
    void background(uint32_t color);

    void drawPixel(int x, int  y, uint32_t color);
    void drawLine(int srcX, int srcY, int desX, int des, uint32_t color);
    void drawRect(int minX, int minY, int maxX, int maxY, uint32_t color);
    void drawTriangle(int x1, int y1,int x2, int y2, int x3, int y3, uint32_t color);
    void drawCircle(int centerX, int centerY, int radius, uint32_t color);
    void drawEllipse(int major, int minor, int centerX, int centerY, uint32_t color);
    
    void fillRect(int minX, int minY, int maxX, int maxY, uint32_t border, uint32_t body);
    void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t border, uint32_t body);
    void fillCircle(int centerX, int centerY, int radius, uint32_t color);
    void fillEllipse(int major, int minor, int centerX, int centerY, uint32_t color);
    
    
}
