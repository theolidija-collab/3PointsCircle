#pragma once
#include <afxwin.h>
#include <vector>
#include <atomic>

class CRandomWorkerThread : public CWinThread
{
    DECLARE_DYNCREATE(CRandomWorkerThread)

public:
    CRandomWorkerThread();
    virtual ~CRandomWorkerThread();

    HWND m_hDrawAreaWnd;   // 메시지를 보낼 대상 윈도우
    HWND m_hMainDialogWnd;
    std::atomic<bool> m_bStop;

    virtual BOOL InitInstance();
    virtual int Run();

    void StopThread();
};
