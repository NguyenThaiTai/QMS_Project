#include "pch.h"
#include "FaceIDAdapter.h"
#include <process.h>

FaceIDAdapter::FaceIDAdapter() { m_bIsScanning = false; }
FaceIDAdapter::~FaceIDAdapter() { Release(); }

bool FaceIDAdapter::Initialize() {
    // Nếu là thiết bị thật: Mở Camera tại đây (OpenCV / SDK Hãng)
    return true;
}

void FaceIDAdapter::StartScanning() {
    if (m_bIsScanning) return;
    m_bIsScanning = true;
    _beginthreadex(NULL, 0, &FaceIDAdapter::FaceIDThreadProc, this, 0, NULL);
}

void FaceIDAdapter::StopScanning() { m_bIsScanning = false; }

void FaceIDAdapter::Release() {
    StopScanning();
    if (m_pListener) m_pListener->OnDeviceDisconnected();
}

unsigned int __stdcall FaceIDAdapter::FaceIDThreadProc(void* pParam)
{
    FaceIDAdapter* pThis = (FaceIDAdapter*)pParam;

    Sleep(3000);

    if (pThis->m_bIsScanning && pThis->m_pListener)
    {
        CitizenCardData faceData;
        faceData.strFullName = L"Hột lu FACE ID";
        faceData.strIDNumber = L"083203001509";

        pThis->m_pListener->OnScanSuccess(faceData);
    }

    pThis->m_bIsScanning = false;
    return 0;
}