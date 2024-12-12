/**
 * @file overlay.cpp
 * @brief 渲染覆盖层类的实现,提供2D渲染功能
 */

#include "overlay.h"
#include <comdef.h>
#include <corecrt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool Overlay::init()
{
    // 查找NVIDIA GeForce Overlay窗口
    window = FindWindowA("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay");

    if (!window)
        return false;

    // 获取窗口扩展样式
    auto info = GetWindowLongA(window, -20);

    if (!info)
        return false;

    // 设置窗口扩展样式,添加透明属性
    auto attrchange = SetWindowLongPtrA(window, -20, (LONG_PTR)(info | 0x20));

    if (!attrchange)
        return false;

    // 设置窗口边距
    MARGINS margin;
    margin.cyBottomHeight = margin.cyTopHeight = margin.cxLeftWidth = margin.cxRightWidth = -1;

    // 扩展窗口框架到客户区
    if (DwmExtendFrameIntoClientArea(window, &margin) != S_OK)
        return false;

    // 设置分层窗口属性
    if (!SetLayeredWindowAttributes(window, 0x000000, 0xFF, 0x02))
        return false;

    // 设置窗口位置为顶层
    if (!SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, 0x0002 | 0x0001))
        return false;

    // 显示窗口
    ShowWindow(window, SW_SHOW);

    return true;
}

bool Overlay::startup_d2d()
{
    RECT rc;
    // 创建Direct2D工厂
    if (HRESULT ret = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory) == S_OK)
    {
        // 创建DirectWrite工厂
        if (ret = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)(&write_factory)) == S_OK)
        {
            // 创建文本格式
            write_factory->CreateTextFormat(font, NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"en-us", &format);
            GetClientRect(window, &rc);

            // 创建渲染目标
            if (ret = d2d_factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)), D2D1::HwndRenderTargetProperties(window, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)), &tar) == S_OK)
            {
                // 创建画刷
                D2D1_BRUSH_PROPERTIES properties = { 1.0f, D2D1::Matrix3x2F::Identity() };
                tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), properties, &brush);

                return true;
            }
        }
    }
    return false;
}

void Overlay::begin_scene()
{
    // 开始绘制场景
    tar->BeginDraw();
}

void Overlay::end_scene()
{
    // 结束绘制场景
    tar->EndDraw();
}

void Overlay::clear_scene()
{
    // 清除场景内容
    tar->Clear();
}

void Overlay::draw_line(int x1, int y1, int x2, int y2, D2D1::ColorF color) {
    // 设置线条颜色
    brush->SetColor(color);

    // 使用Direct2D绘制直线
    tar->DrawLine(
        D2D1::Point2F(x1, y1),  // 起点
        D2D1::Point2F(x2, y2),  // 终点
        brush
    );
}

void Overlay::drawCircle(int centerX, int centerY, float radius, D2D1::ColorF color) {
    // 设置圆形的分段数
    const int numSegments = 100;
    float angleStep = 2 * 3.14159265f / numSegments;

    // 通过绘制多条线段来近似圆形
    for (int i = 0; i < numSegments; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        // 计算线段的起点和终点
        int x1 = static_cast<int>(centerX + radius * cos(angle1));
        int y1 = static_cast<int>(centerY + radius * sin(angle1));
        int x2 = static_cast<int>(centerX + radius * cos(angle2));
        int y2 = static_cast<int>(centerY + radius * sin(angle2));

        // 绘制线段
        tar->DrawLine(
            D2D1::Point2F(x1, y1),  // 起点
            D2D1::Point2F(x2, y2),  // 终点
            brush
        );
    }
}

void Overlay::draw_text(int x, int y, const char* text, D2D1::ColorF color, float font_size, ...)
{
    // 文本缓冲区
    char buffer[4096];
    int length;
    wchar_t out[256];
    RECT window_metrics;

    // 获取窗口尺寸
    if (!GetWindowRect(window, &window_metrics))
        return;

    // 处理可变参数
    va_list arg_list;
    va_start(arg_list, text);
    vsnprintf(buffer, sizeof(buffer), text, arg_list);
    va_end(arg_list);

    // 转换为宽字符
    length = strlen(buffer);
    mbstowcs(out, buffer, length);

    // 创建DirectWrite工厂
    IDWriteFactory* pDWriteFactory = nullptr;
    HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
    if (FAILED(hr)) {
        return;
    }

    // 创建文本格式
    IDWriteTextFormat* pTextFormat = nullptr;
    hr = pDWriteFactory->CreateTextFormat(
        L"Arial",                // 字体名称
        NULL,                    // 字体集合(NULL表示使用系统字体集合)
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        font_size,               // 字体大小
        L"en-us",                // 区域设置
        &pTextFormat             // 输出参数
    );

    if (FAILED(hr)) {
        pDWriteFactory->Release();
        return;
    }

    // 绘制文本
    tar->DrawText(
        out,                     // 要绘制的字符串
        length,                  // 字符串长度
        pTextFormat,             // 文本格式
        D2D1::RectF(x, y, window_metrics.right - window_metrics.left, window_metrics.bottom - window_metrics.top),
        brush                    // 用于绘制文本的画刷
    );

    brush->SetColor(color);

    // 释放资源
    pTextFormat->Release();
    pDWriteFactory->Release();
}

void Overlay::draw_filled_rectangle(int top_x, int top_y, int bottom_x, int bottom_y, D2D1::ColorF color)
{
    // 设置画刷颜色
    brush->SetColor(color);

    // 定义要填充的矩形区域
    D2D1_RECT_F rectangle = D2D1::RectF(top_x, top_y, bottom_x, bottom_y);

    // Fill the rectangle
    tar->FillRectangle(&rectangle, brush);
}


void Overlay::draw_rectangle(int top_x, int top_y, int bottom_x, int bottom_y, D2D1::ColorF color)
{
    // Draw top line
    draw_line(top_x, top_y, bottom_x, top_y, color);
    // Draw bottom line
    draw_line(top_x, bottom_y, bottom_x, bottom_y, color);
    // Draw left line
    draw_line(top_x, top_y, top_x, bottom_y, color);
    // Draw right line
    draw_line(bottom_x, top_y, bottom_x, bottom_y, color);
}
