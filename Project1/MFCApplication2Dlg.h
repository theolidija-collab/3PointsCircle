
// MFCApplication2Dlg.h: 헤더 파일
//

#pragma once

#include <vector>

#include "DrawArea.h"
#include "RandomWorkerThread.h"

class CMFCApplication2Dlg : public CDialogEx
{
public:
	static constexpr int RADIUS_MIN = 1;
	static constexpr int RADIUS_MAX = 64;
	static constexpr int THICKNESS_MIN = 1;
	static constexpr int THICKNESS_MAX = 128;
	CMFCApplication2Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION2_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();	
	afx_msg void OnDestroy();
	afx_msg LRESULT OnThreadFinished(WPARAM, LPARAM);
	afx_msg void OnBnClickedInitbutton();
	afx_msg void OnEnChangeEdit1Radius();
	afx_msg void OnEnChangeEdit2Thickness();
	afx_msg void OnBnClickedRandombutton();
	afx_msg void OnDeltaposSpinRadius(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinThickness(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnUpdatePoints(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnInfo();

	DECLARE_MESSAGE_MAP()

private:
	CRandomWorkerThread* m_pRandomThread;
	CDrawArea m_drawArea;
	int m_nRadius;
	int m_nThickness;
	CMFCButton m_btnRandom;
	CStatic m_staticPoints;
};
