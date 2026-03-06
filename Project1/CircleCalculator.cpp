#include "pch.h"
#include "CircleCalculator.h"
#include <cmath>

bool CalculateCircleFrom3Points(
    const CPoint& p1,
    const CPoint& p2,
    const CPoint& p3,
    Circle& outCircle)
{
    // Exception 1 : 3 points are identical
    if (p1 == p2 && p2 == p3)
    {
        TRACE("Exception: All three points are identical. Infinite circles exist.\n");
        return false;
    }

    // Exception 2: 2 points are identical
    if (p1 == p2 || p2 == p3 || p1 == p3)
    {
        TRACE("Exception: At least two points are identical. Cannot define unique circle.\n");
        return false;
    }

    double A = p2.x - p1.x;
    double B = p2.y - p1.y;
    double C = p3.x - p1.x;
    double D = p3.y - p1.y;

    double E = A * (p1.x + p2.x) + B * (p1.y + p2.y);
    double F = C * (p1.x + p3.x) + D * (p1.y + p3.y);
    double G = 2.0 * (A * (p3.y - p2.y) - B * (p3.x - p2.x));

    // Exception 3 : 3 points are collinear
    if (fabs(G) < 5)
    {
        TRACE("Exception: 3 points are collinear. No circle exists.\n");
        return false;
    }

    double cx = (D * E - B * F) / G;
    double cy = (A * F - C * E) / G;

    outCircle.center = CPoint((int)cx, (int)cy);
    outCircle.radius = (int)std::sqrt(
        (cx - p1.x) * (cx - p1.x) +
        (cy - p1.y) * (cy - p1.y));

    // Exception 4 : Cannot draw huge radius circle
    // UHD 8K resolution : 7680x4320
	if (outCircle.radius > 3840)  
    {
        TRACE("Exception: Cannot draw huge radius circle.\n");
        return false;
    }

    return true;
}
