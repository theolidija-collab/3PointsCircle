#pragma once
#include <vector>

#define WM_UPDATE_RANDOM_POINTS (WM_USER + 100)
#define WM_THREAD_FINISHED      (WM_USER + 101)
#define WM_UPDATE_POINTS        (WM_USER + 200)

class CDrawArea : public CStatic
{
    DECLARE_DYNAMIC(CDrawArea)

public:
    CDrawArea();
    virtual ~CDrawArea();

    void Reset();
    void SetPoints(const std::vector<CPoint>& pts);
    const std::vector<CPoint>& GetPoints() const;        

    int GetThickness() const;
    int GetRadius() const;
    void SetThickness(int value);
    void SetRadius(int value);

protected:
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg LRESULT OnUpdateRandomPoints(WPARAM, LPARAM);

    DECLARE_MESSAGE_MAP()

private:
    int  m_radius = 8;
    int  m_nThickness = 1;
    bool m_bDragging;
    int  m_dragIndex;
    std::vector<CPoint> m_points;
};
