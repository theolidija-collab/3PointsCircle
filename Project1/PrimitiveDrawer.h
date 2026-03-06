#pragma once
#include <afxwin.h>

void DrawCircle(DWORD* buffer, int width, int height,
    int cx, int cy, int radius,
    COLORREF color, int thickness = 1);
void DrawFilledCircle(CDC* pDC, int cx, int cy, int radius, COLORREF color);
