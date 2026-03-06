#include "pch.h"
#include "PrimitiveDrawer.h"

static inline DWORD ConvertColor(COLORREF color)
{
    return (GetBValue(color)) |
           (GetGValue(color) << 8) |
           (GetRValue(color) << 16);
}

static inline void PutPixel(DWORD* buffer, int width, int height, int x, int y, DWORD color)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        buffer[y * width + x] = color;
    }
}

// Bresenham's Circle (Midpoint Algorithm)
void DrawCircle(DWORD* buffer, int width, int height,
    int cx, int cy, int radius,
    COLORREF color, int thickness)
{
    if (thickness < 1)
        thickness = 1;

    DWORD pixelColor = ConvertColor(color);

    for (int t = 0; t < thickness; t++)
    {
        int r = radius - t;
        if (r <= 0)
            break;

        int x = 0;
        int y = r;
        int d = 1 - r;

        while (x <= y)
        {
            PutPixel(buffer, width, height, cx + x, cy + y, pixelColor);
            PutPixel(buffer, width, height, cx - x, cy + y, pixelColor);
            PutPixel(buffer, width, height, cx + x, cy - y, pixelColor);
            PutPixel(buffer, width, height, cx - x, cy - y, pixelColor);
            PutPixel(buffer, width, height, cx + y, cy + x, pixelColor);
            PutPixel(buffer, width, height, cx - y, cy + x, pixelColor);
            PutPixel(buffer, width, height, cx + y, cy - x, pixelColor);
            PutPixel(buffer, width, height, cx - y, cy - x, pixelColor);

            if (d < 0)
                d += 2 * x + 3;
            else
            {
                d += 2 * (x - y) + 5;
                y--;
            }
            x++;
        }
    }
}

// 채워진 원 (Scanline 방식)
void DrawFilledCircle(CDC* pDC, int cx, int cy, int radius, COLORREF color)
{
    CPen pen(PS_SOLID, 1, color);
    CPen* pOldPen = pDC->SelectObject(&pen);

    for (int y = -radius; y <= radius; y++)
    {
        int dx = (int)sqrt(radius * radius - y * y);

        pDC->MoveTo(cx - dx, cy + y);
        pDC->LineTo(cx + dx + 1, cy + y);
    }

    pDC->SelectObject(pOldPen);
}

