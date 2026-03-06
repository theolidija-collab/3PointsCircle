#pragma once
#include <afxwin.h>

struct Circle
{
    CPoint center = CPoint(0, 0);
    int radius = 0;
};

bool CalculateCircleFrom3Points(
    const CPoint& p1, 
    const CPoint& p2, 
    const CPoint& p3, 
    Circle& outCircle);
