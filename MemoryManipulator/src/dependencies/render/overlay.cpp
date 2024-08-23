#include "overlay.h"
#include <comdef.h>
#include <corecrt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool Overlay::init()
{
    window = FindWindowA("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay");

    if (!window)
        return false;

    auto info = GetWindowLongA(window, -20);

    if (!info)
        return false;

    auto attrchange = SetWindowLongPtrA(window, -20, (LONG_PTR)(info | 0x20));

    if (!attrchange)
        return false;

    MARGINS margin;
    margin.cyBottomHeight = margin.cyTopHeight = margin.cxLeftWidth = margin.cxRightWidth = -1;

    if (DwmExtendFrameIntoClientArea(window, &margin) != S_OK)
        return false;

    if (!SetLayeredWindowAttributes(window, 0x000000, 0xFF, 0x02))
        return false;

    if (!SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, 0x0002 | 0x0001))
        return false;

    ShowWindow(window, SW_SHOW);

    return true;
}

bool Overlay::startup_d2d()
{
    RECT rc;
    if (HRESULT ret = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory) == S_OK)
    {
        if (ret = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)(&write_factory)) == S_OK)
        {
            /* not adding more checks here, this should all succeed. Should be pretty safe. */
            write_factory->CreateTextFormat(font, NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"en-us", &format);
            GetClientRect(window, &rc);

            if (ret = d2d_factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)), D2D1::HwndRenderTargetProperties(window, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)), &tar) == S_OK)
            {
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
    tar->BeginDraw();
}

void Overlay::end_scene()
{
    tar->EndDraw();
}

void Overlay::clear_scene()
{
    tar->Clear();
}

void Overlay::draw_line(int x1, int y1, int x2, int y2, D2D1::ColorF color) {
    brush->SetColor(color);  // Set the line color

    // Draw the line using D2D1's rendering functionalities
    tar->DrawLine(
        D2D1::Point2F(x1, y1),  // Start point
        D2D1::Point2F(x2, y2),  // End point
        brush
    );
}

void Overlay::drawCircle(int centerX, int centerY, float radius, D2D1::ColorF color) {
    const int numSegments = 100;
    float angleStep = 2 * 3.14159265f / numSegments;

    for (int i = 0; i < numSegments; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        int x1 = static_cast<int>(centerX + radius * cos(angle1));
        int y1 = static_cast<int>(centerY + radius * sin(angle1));
        int x2 = static_cast<int>(centerX + radius * cos(angle2));
        int y2 = static_cast<int>(centerY + radius * sin(angle2));

        tar->DrawLine(
            D2D1::Point2F(x1, y1),  // Start point
            D2D1::Point2F(x2, y2),  // End point
            brush
        );
    }
}

void Overlay::draw_text(int x, int y, const char* text, D2D1::ColorF color, float font_size, ...)
{
    char buffer[4096];
    int length;
    wchar_t out[256];
    RECT window_metrics;

    if (!GetWindowRect(window, &window_metrics))
        return;

    va_list arg_list;
    va_start(arg_list, text);
    vsnprintf(buffer, sizeof(buffer), text, arg_list);
    va_end(arg_list);

    length = strlen(buffer);
    mbstowcs(out, buffer, length);

    // Create a DirectWrite factory
    IDWriteFactory* pDWriteFactory = nullptr;
    HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
    if (FAILED(hr)) {
        // Handle error
        return;
    }

    // Create a text format
    IDWriteTextFormat* pTextFormat = nullptr;
    hr = pDWriteFactory->CreateTextFormat(
        L"Arial",                // Font family name
        NULL,                    // Font collection (NULL sets it to use the system font collection)
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        font_size,               // Font size
        L"en-us",                // Locale
        &pTextFormat             // Output parameter
    );

    if (FAILED(hr)) {
        // Handle error
        pDWriteFactory->Release();
        return;
    }

    tar->DrawText(
        out,                     // The string to draw
        length,                  // The string's length
        pTextFormat,             // The text format
        D2D1::RectF(x, y, window_metrics.right - window_metrics.left, window_metrics.bottom - window_metrics.top),
        brush                    // The brush used to draw the text
    );

    brush->SetColor(color);

    // Release resources
    pTextFormat->Release();
    pDWriteFactory->Release();
}

void Overlay::draw_filled_rectangle(int top_x, int top_y, int bottom_x, int bottom_y, D2D1::ColorF color)
{
    // Set the color for the brush
    brush->SetColor(color);

    // Define the rectangle to be filled
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
