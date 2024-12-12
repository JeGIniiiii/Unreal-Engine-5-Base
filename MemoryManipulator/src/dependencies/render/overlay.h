/**
 * @file overlay.h
 * @brief 渲染覆盖层类的声明,提供2D渲染功能
 */

#pragma once
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <string>
#include <vector>
#include <windows.h>

/**
 * @class Overlay
 * @brief 提供2D渲染覆盖层功能
 */
class Overlay
{
private:
    HWND window;                    // 窗口句柄

    ID2D1Factory* d2d_factory;              // Direct2D工厂
    ID2D1HwndRenderTarget* tar;             // 渲染目标
    IDWriteFactory* write_factory;          // DirectWrite工厂
    ID2D1SolidColorBrush* brush;           // 画刷
    IDWriteTextFormat* format;             // 文本格式

    const wchar_t* font;                    // 字体名称
    float          size;                    // 字体大小

public:
    /**
     * @brief 构造函数
     * @param Font 字体名称
     * @param Font_Size 字体大小
     */
    Overlay(const wchar_t* Font, float Font_Size)
    {
        this->font = Font;
        this->size = Font_Size;
    }

    /**
     * @brief 析构函数,释放Direct2D资源
     */
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

    bool init();                // 初始化覆盖层
    bool startup_d2d();        // 启动Direct2D

    void begin_scene();        // 开始渲染场景
    void end_scene();          // 结束渲染场景
    void clear_scene();        // 清除渲染场景

    /* 渲染辅助函数 */
    /**
     * @brief 绘制文本
     * @param x,y 文本位置
     * @param text 文本内容
     * @param color 文本颜色
     * @param font_size 字体大小
     */
    void draw_text(int x, int y, const char* text, D2D1::ColorF color, float font_size, ...);
    
    /**
     * @brief 绘制直线
     * @param x1,y1 起点坐标
     * @param x2,y2 终点坐标
     * @param color 线条颜色
     */
    void draw_line(int x1, int y1, int x2, int y2, D2D1::ColorF color);
    
    /**
     * @brief 绘制矩形边框
     * @param top_x,top_y 左上角坐标
     * @param bottom_x,bottom_y 右下角坐标
     * @param color 边框颜色
     */
    void draw_rectangle(int top_x, int top_y, int bottom_x, int bottom_y, D2D1::ColorF color);
    
    /**
     * @brief 绘制圆形
     * @param centerX,centerY 圆心坐标
     * @param radius 半径
     * @param color 圆形颜色
     */
    void drawCircle(int centerX, int centerY, float radius, D2D1::ColorF color);
    
    /**
     * @brief 绘制填充矩形
     * @param top_x,top_y 左上角坐标
     * @param bottom_x,bottom_y 右下角坐标
     * @param color 填充颜色
     */
    void draw_filled_rectangle(int top_x, int top_y, int bottom_x, int bottom_y, D2D1::ColorF color);
};