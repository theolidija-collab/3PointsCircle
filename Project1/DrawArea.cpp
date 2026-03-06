#include "pch.h"
#include "DrawArea.h"
#include <cmath>

#include "PrimitiveDrawer.h"
#include "CircleCalculator.h"

IMPLEMENT_DYNAMIC(CDrawArea, CStatic)

CDrawArea::CDrawArea()
{
    m_dragIndex = -1;
    m_bDragging = false;
    m_radius = 8;
    m_nThickness = 1;
}

CDrawArea::~CDrawArea()
{
}

BEGIN_MESSAGE_MAP(CDrawArea, CStatic)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_UPDATE_RANDOM_POINTS, &CDrawArea::OnUpdateRandomPoints)
END_MESSAGE_MAP()

void CDrawArea::Reset()
{
    m_points.clear();
    GetParent()->PostMessage(WM_UPDATE_POINTS);
    m_dragIndex = -1;

    m_radius = 8;
    m_nThickness = 1;
    Invalidate();
}

void CDrawArea::SetPoints(const std::vector<CPoint>& pts)
{
    m_points = pts;
    Invalidate();
}

const std::vector<CPoint>& CDrawArea::GetPoints() const
{
    return m_points;
}

void CDrawArea::OnPaint()
{
    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);

    // ---------------------------
    // 더블 버퍼링
    // ---------------------------
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = rect.Width();
    bmi.bmiHeader.biHeight = -rect.Height();   // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(dc.GetSafeHdc(),
        &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);

    CDC memDC;
    memDC.CreateCompatibleDC(&dc);
    CBitmap bitmap;
    bitmap.Attach(hBitmap);
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

    memDC.FillSolidRect(rect, RGB(64, 64, 64));

    // ---------------------------
    // 1. 클릭된 점 표시
    // ---------------------------
    for (const auto& pt : m_points)
    {
        DrawFilledCircle(&memDC, pt.x, pt.y, m_radius, RGB(255, 0, 0));
    }

    // ---------------------------
    // 2. 3개의 점을 지나는 원을 계산하여 그리기
    // ---------------------------
    if (m_points.size() == 3)
    {
        Circle circle;

        if (CalculateCircleFrom3Points(m_points[0], m_points[1], m_points[2], circle))
        {
            DWORD* buffer = (DWORD*)pBits;
            DrawCircle(buffer, rect.Width(), rect.Height(),
                circle.center.x, circle.center.y, circle.radius,
                RGB(255, 255, 0), m_nThickness);
        }
    }

    // ---------------------------
    // 3. 점이 1개 이상이면 좌표 표시
    // ---------------------------
    if (!m_points.empty())
    {
        GetParent()->PostMessage(WM_UPDATE_POINTS, 0, 0);

        const int BOX_W = 90;
        const int BOX_H = 24;
        const int GAP = 12;

        memDC.SetTextColor(RGB(0, 255, 0));
        memDC.SetBkMode(TRANSPARENT);

        std::vector<CRect> placedRects;

        for (int i = 0; i < (int)m_points.size(); i++)
        {
            CString text;
            text.Format(_T("P%d: %d, %d"), i + 1, m_points[i].x, m_points[i].y);

            CPoint pt = m_points[i];
            CRect boxRect;
            bool placed = false;

            // 8방향 후보
            CPoint offsets[8] =
            {
                CPoint(GAP, -BOX_H / 2),
                CPoint(GAP, GAP),
                CPoint(-BOX_W / 2, GAP),
                CPoint(-BOX_W - GAP, GAP),
                CPoint(-BOX_W - GAP, -BOX_H / 2),
                CPoint(-BOX_W - GAP, -BOX_H - GAP),
                CPoint(-BOX_W / 2, -BOX_H - GAP),
                CPoint(GAP, -BOX_H - GAP)
            };

            for (int dir = 0; dir < 8; dir++)
            {
                int boxX = pt.x + offsets[dir].x;
                int boxY = pt.y + offsets[dir].y;
                CRect candidate(boxX, boxY, boxX + BOX_W, boxY + BOX_H);

                // 화면 경계 체크
                if (!rect.PtInRect(candidate.TopLeft()) || !rect.PtInRect(candidate.BottomRight()))
                    continue;

                // 기존 박스와 충돌 검사
                bool overlap = false;
                for (const auto& r : placedRects)
                {
                    CRect intersect;
                    if (intersect.IntersectRect(candidate, r))
                    {
                        overlap = true;
                        break;
                    }
                }

                if (!overlap)
                {
                    boxRect = candidate;
                    placed = true;
                    break;
                }
            }

            // 모든 방향이 겹치면 기본 위치
            if (!placed)
            {
                boxRect = CRect(pt.x + GAP, pt.y + GAP,
                    pt.x + GAP + BOX_W, pt.y + GAP + BOX_H);
            }

            placedRects.push_back(boxRect);

            // 박스 그리기
            memDC.FillSolidRect(boxRect, RGB(30, 30, 30));
            memDC.Draw3dRect(boxRect, RGB(0, 180, 0), RGB(0, 180, 0));
            memDC.DrawText(text, boxRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            // 연결선
            //memDC.MoveTo(pt);
            //memDC.LineTo(boxRect.CenterPoint());
        }
    }

    // ===============================
    // 4. 화면 출력
    // ===============================
    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

    memDC.SelectObject(pOldBitmap);
}

void CDrawArea::OnLButtonDown(UINT nFlags, CPoint point)
{
    const int hitRadius = m_radius;

    for (int i = 0; i < (int)m_points.size(); i++)
    {
        int dx = point.x - m_points[i].x;
        int dy = point.y - m_points[i].y;

        // 원 내부 판정
        if (dx * dx + dy * dy <= hitRadius * hitRadius)
        {
            m_bDragging = true;
            m_dragIndex = i;
            SetCapture();
            return;
        }
    }

    if (m_points.size() >= 3)
        return;

    m_points.push_back(point);
    Invalidate();

    CStatic::OnLButtonDown(nFlags, point);
}

void CDrawArea::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bDragging && m_dragIndex != -1)
    {
        CRect rect;
        GetClientRect(&rect);
        point.x = max(rect.left, min(point.x, rect.right));
        point.y = max(rect.top, min(point.y, rect.bottom));

        m_points[m_dragIndex] = point;
        Invalidate();
    }
}

void CDrawArea::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bDragging)
    {
        m_bDragging = false;
        m_dragIndex = -1;
        ReleaseCapture();
    }
}

BOOL CDrawArea::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;   // 배경 지우지 않기
}

LRESULT CDrawArea::OnUpdateRandomPoints(WPARAM, LPARAM lParam)
{
    std::vector<CPoint>* pPoints = (std::vector<CPoint>*)lParam;

    m_points = *pPoints;
    delete pPoints;
    Invalidate();

    return 0;
}

int CDrawArea::GetThickness() const 
{ 
    return m_nThickness; 
}

int CDrawArea::GetRadius() const 
{ 
    return m_radius; 
}

void CDrawArea::SetThickness(int value)
{
    m_nThickness = value;
}

void CDrawArea::SetRadius(int value)
{
    m_radius = value;
}


