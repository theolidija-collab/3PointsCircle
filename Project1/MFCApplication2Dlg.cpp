
// MFCApplication2Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication2.h"
#include "MFCApplication2Dlg.h"
#include "afxdialogex.h"

#include "PrimitiveDrawer.h"
#include "CircleCalculator.h"
#include "RandomWorkerThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication2Dlg 대화 상자
CMFCApplication2Dlg::CMFCApplication2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION2_DIALOG, pParent),
	m_pRandomThread(nullptr), m_nRadius(5), m_nThickness(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX); 
	DDX_Control(pDX, IDC_DRAW_AREA, m_drawArea);

	DDX_Text(pDX, IDC_EDIT1_RADIUS, m_nRadius);
	DDV_MinMaxInt(pDX, m_nRadius, RADIUS_MIN, RADIUS_MAX);

	DDX_Text(pDX, IDC_EDIT2_THICKNESS, m_nThickness);
	DDV_MinMaxInt(pDX, m_nThickness, THICKNESS_MIN, THICKNESS_MAX);

	DDX_Control(pDX, IDC_RANDOMBUTTON, m_btnRandom);

	DDX_Control(pDX, IDC_STATIC_POINTS, m_staticPoints);
}

BEGIN_MESSAGE_MAP(CMFCApplication2Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_INITBUTTON, &CMFCApplication2Dlg::OnBnClickedInitbutton)
	ON_EN_CHANGE(IDC_EDIT1_RADIUS, &CMFCApplication2Dlg::OnEnChangeEdit1Radius)
	ON_EN_CHANGE(IDC_EDIT2_THICKNESS, &CMFCApplication2Dlg::OnEnChangeEdit2Thickness)
	ON_BN_CLICKED(IDC_RANDOMBUTTON, &CMFCApplication2Dlg::OnBnClickedRandombutton)	
	ON_MESSAGE(WM_THREAD_FINISHED, &CMFCApplication2Dlg::OnThreadFinished)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2_RADIUS, &CMFCApplication2Dlg::OnDeltaposSpinRadius)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN3_THICKNESS, &CMFCApplication2Dlg::OnDeltaposSpinThickness)
	ON_MESSAGE(WM_UPDATE_POINTS, &CMFCApplication2Dlg::OnUpdatePoints)
	ON_BN_CLICKED(IDC_BUTTON1_INFO, &CMFCApplication2Dlg::OnBnClickedBtnInfo)
END_MESSAGE_MAP()


// CMFCApplication2Dlg 메시지 처리기
BOOL CMFCApplication2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	HICON hIcon = AfxGetApp()->LoadStandardIcon(IDI_INFORMATION);
	CButton* pBtn = (CButton*)GetDlgItem(IDC_BUTTON1_INFO);
	pBtn->SetIcon(hIcon);

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	ModifyStyle(0, WS_CAPTION | WS_SYSMENU);  // twkim
	//SetWindowText(_T("Circle application passing through 3 poinits"));
	m_nRadius = 8; 
	m_nThickness = 1;
	OnUpdatePoints(0, 0);
	UpdateData(FALSE);     

	return TRUE;
}

void CMFCApplication2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCApplication2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCApplication2Dlg::OnBnClickedInitbutton()
{
	if (m_pRandomThread != nullptr)
	{
		m_pRandomThread->StopThread();
		WaitForSingleObject(m_pRandomThread->m_hThread, 2000);
		m_pRandomThread = nullptr;
	}

	// not necessary m_btnRandom.EnableWindow(TRUE);
	m_drawArea.Reset();

	// update input_control
	m_nThickness = m_drawArea.GetThickness();  
	m_nRadius = m_drawArea.GetRadius();       
	UpdateData(FALSE);
}

void CMFCApplication2Dlg::OnEnChangeEdit1Radius()
{
	UpdateData(TRUE);
	
	if (m_nRadius < RADIUS_MIN)
		m_nRadius = RADIUS_MIN;

	if (m_nRadius > RADIUS_MAX)
		m_nRadius = RADIUS_MAX;

	m_drawArea.SetRadius(m_nRadius);  
	m_drawArea.Invalidate();
}

void CMFCApplication2Dlg::OnEnChangeEdit2Thickness()
{
	UpdateData(TRUE);

	if (m_nThickness < THICKNESS_MIN)
		m_nThickness = THICKNESS_MIN;

	if (m_nThickness > THICKNESS_MAX)
		m_nThickness = THICKNESS_MAX;

	m_drawArea.SetThickness(m_nThickness);  
	m_drawArea.Invalidate();
}

void CMFCApplication2Dlg::OnBnClickedRandombutton()
{
	m_btnRandom.EnableWindow(FALSE);
	if (m_pRandomThread != nullptr)
	{
		TRACE("[debug] ### m_pRandomThread is still running !!! \n");
		return;
	}

	m_pRandomThread = (CRandomWorkerThread*)AfxBeginThread(RUNTIME_CLASS(CRandomWorkerThread));
	m_pRandomThread->m_hDrawAreaWnd = m_drawArea.GetSafeHwnd();
	m_pRandomThread->m_hMainDialogWnd = this->GetSafeHwnd();
}

void CMFCApplication2Dlg::OnDestroy()
{
	if (m_pRandomThread != nullptr)
	{
		m_pRandomThread->StopThread();
		WaitForSingleObject(m_pRandomThread->m_hThread, 2000);
		m_pRandomThread = nullptr;
	}

	CDialogEx::OnDestroy();
}

LRESULT CMFCApplication2Dlg::OnThreadFinished(WPARAM, LPARAM)
{
	m_btnRandom.EnableWindow(TRUE);
	m_pRandomThread = nullptr;	

	return 0;
}

void CMFCApplication2Dlg::OnDeltaposSpinRadius(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMUPDOWN* pNMUpDown = reinterpret_cast<NMUPDOWN*>(pNMHDR);

	// 1️. 현재 Edit 값 읽기
	CString str;
	GetDlgItemText(IDC_EDIT1_RADIUS, str);

	int value = _ttoi(str);

	// 2️. Spin Control 처리
	// iDelta = -1, 위 클릭 증가
	// iDelta = +1, 아래 클릭 감소
	value -= pNMUpDown->iDelta;

	// 3️. 범위 제한
	if (value < RADIUS_MIN) value = RADIUS_MIN;
	if (value > RADIUS_MAX) value = RADIUS_MAX;

	// 4️. Edit에 다시 설정
	CString newStr;
	newStr.Format(_T("%d"), value);
	SetDlgItemText(IDC_EDIT1_RADIUS, newStr);

	// 5️. DrawArea에 적용
	m_drawArea.SetRadius(value);
	m_drawArea.Invalidate();

	// 6️ Spin 기본 동작 막기
	*pResult = 1;
}

void CMFCApplication2Dlg::OnDeltaposSpinThickness(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMUPDOWN* pNMUpDown = reinterpret_cast<NMUPDOWN*>(pNMHDR);

	// 1️. 현재 Edit 값 읽기
	CString str;
	GetDlgItemText(IDC_EDIT2_THICKNESS, str);

	int value = _ttoi(str);

	// 2️. Spin Control 처리
	// iDelta = -1, 위 클릭 증가
	// iDelta = +1, 아래 클릭 감소
	value -= pNMUpDown->iDelta;

	// 3️. 범위 제한
	if (value < THICKNESS_MIN) value = THICKNESS_MIN;
	if (value > THICKNESS_MAX) value = THICKNESS_MAX;

	// 4️. Edit에 다시 설정
	CString newStr;
	newStr.Format(_T("%d"), value);
	SetDlgItemText(IDC_EDIT2_THICKNESS, newStr);

	// 5️. DrawArea에 적용
	m_drawArea.SetThickness(value);
	m_drawArea.Invalidate();

	// 6️ Spin 기본 동작 막기
	*pResult = 1;
}

LRESULT CMFCApplication2Dlg::OnUpdatePoints(WPARAM, LPARAM)
{
	const std::vector<CPoint>& points = m_drawArea.GetPoints();

	CString str;
	CString p1 = _T("---, ---");
	CString p2 = _T("---, ---");
	CString p3 = _T("---, ---");

	if (points.size() >= 1)
		p1.Format(_T("%d, %d"), points[0].x, points[0].y);

	if (points.size() >= 2)
		p2.Format(_T("%d, %d"), points[1].x, points[1].y);

	if (points.size() >= 3)
		p3.Format(_T("%d, %d"), points[2].x, points[2].y);

	str.Format(_T(" P1 : (%s)\r\n")
		_T(" P2 : (%s)\r\n")
		_T(" P3 : (%s)"),
		p1.GetString(), p2.GetString(), p3.GetString());

	m_staticPoints.SetWindowText(str);

	return 0;
}

void CMFCApplication2Dlg::OnBnClickedBtnInfo()
{
	CString msg;
	msg.Format(_T("Job Application Program\n\n")
		_T("- 초고속 원그리기 적용\n")
		_T("- Static Code Analysis(정적분석) 완료\n")
		_T("- AddressSanitizer(메모리 주소검증) 완료\n")
		_T("- Author : 김태우\n\n")
		_T("© 2026 All Rights Reserved."));

	MessageBox(msg, _T("About This Program"), MB_ICONINFORMATION | MB_OK);
}
