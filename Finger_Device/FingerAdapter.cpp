#include "pch.h"
#include "FingerAdapter.h"
#include <process.h>

FingerAdapter::FingerAdapter() { 
    m_bIsScanning = false; 
    m_hThread = nullptr;

}
FingerAdapter::~FingerAdapter() { Release(); }

bool FingerAdapter::Initialize() {

    return true;
}

void FingerAdapter::StartScanning() {
    if (m_bIsScanning) return;
    m_bIsScanning = true;
    m_hThread = (void*)_beginthreadex(NULL, 0, &FingerAdapter::FingerThreadProc, this, 0, NULL);
}

void FingerAdapter::StopScanning() { m_bIsScanning = false; }

void FingerAdapter::Release() {
    StopScanning();
    
    if (m_hThread) {
        WaitForSingleObject(m_hThread, INFINITE);
        CloseHandle(m_hThread);
        m_hThread = nullptr;
    }

    if (m_pListener) m_pListener->OnDeviceDisconnected();
}

unsigned int __stdcall FingerAdapter::FingerThreadProc(void* pParam)
{
    FingerAdapter* pThis = (FingerAdapter*)pParam;

    //Sleep(2000);

    if (pThis->m_bIsScanning && pThis->m_pListener)
    {
        CitizenCardData fingerData;

        fingerData.strFullName = L"NGUYỄN VĂN FINGER";
        fingerData.strIDNumber = L"083203001509";

        //pThis->m_pListener->OnScanSuccess(fingerData);
    }

    pThis->m_bIsScanning = false;
    return 0;
}