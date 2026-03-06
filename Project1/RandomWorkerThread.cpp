#include "pch.h"
#include "RandomWorkerThread.h"
#include "DrawArea.h"

IMPLEMENT_DYNCREATE(CRandomWorkerThread, CWinThread)

CRandomWorkerThread::CRandomWorkerThread()
{
    m_hDrawAreaWnd = nullptr;
    m_hMainDialogWnd = nullptr;
}

CRandomWorkerThread::~CRandomWorkerThread()
{
}

BOOL CRandomWorkerThread::InitInstance()
{
    return TRUE;
}

void CRandomWorkerThread::StopThread()
{
    TRACE("[debug] ### StopThread was called !!! \n");
    m_bStop = true;
}

int CRandomWorkerThread::Run()
{
    srand((unsigned int)time(NULL));

    // 랜덤한 위치로, 초당2회 총10회반복
    for (int i = 0; i < 10; i++)
    {
        if (m_bStop)
        {
            if (::IsWindow(m_hMainDialogWnd))
            {
                ::PostMessage(m_hMainDialogWnd, WM_THREAD_FINISHED, 0, 0);
            }
            TRACE("[debug] ### Escaping the loop, because StopThread was called !!! \n");
            break;
        }

        if (!::IsWindow(m_hDrawAreaWnd))
        {
            TRACE("[debug] ### Escaping the loop, because Main Program was terminated !!!\n");
            break;
        }

        std::vector<CPoint>* pPoints = new std::vector<CPoint>;

        int x = 0, y = 0;
        for (int j = 0; j < 3; j++)
        {
            x = rand() % 800;
            y = rand() % 600;
            pPoints->push_back(CPoint(x, y));
        }

        ::PostMessage(m_hDrawAreaWnd, WM_UPDATE_RANDOM_POINTS, 0, (LPARAM)pPoints);

        Sleep(500);   // 초당2회
    }

    if (::IsWindow(m_hMainDialogWnd))
    {
        ::PostMessage(m_hMainDialogWnd, WM_THREAD_FINISHED, 0, 0);
    }

    return 0;
}
