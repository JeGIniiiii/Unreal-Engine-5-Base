#pragma once
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <string>
#include <vector>
#include <windows.h>

class Overlay
{
private:
    HWND window;

    ID2D1Factory* d2d_factory;
    ID2D1HwndRenderTarget* tar;
    IDWriteFactory* write_factory;
    ID2D1SolidColorBrush* brush;
    IDWriteTextFormat* format;

    const wchar_t* font;
    float          size;

public:
    Overlay(const wchar_t* Font, float Font_Size)
    {
        this->font = Font;
        this->size = Font_Size;
    }

    ~Overlay()
    {
        begin_scene();
        clear_scene();
        end_scene();

        tar->Release();
        write_factory->Release();
        brush->Release();
        d2d_factory->Release();

        printf("tar: %p, write: %p, brush: %p, factory: %p", tar, write_factory, brush, d2d_factory);
    }

    bool init();
    bool startup_d2d();

    void begin_scene();
    void end_scene();
    void clear_scene();

    /* create helper functions here */
    void draw_text(int x, int y, const char* text, D2D1::ColorF color, float font_size, ...);
    void draw_line(int x1, int y1, int x2, int y2, D2D1::ColorF color);
    void draw_rectangle(int top_x, int top_y, int bottom_x, int bottom_y, D2D1::ColorF color);
    void drawCircle(int centerX, int centerY, float radius, D2D1::ColorF color);
    void draw_filled_rectangle(int top_x, int top_y, int bottom_x, int bottom_y, D2D1::ColorF color);
};