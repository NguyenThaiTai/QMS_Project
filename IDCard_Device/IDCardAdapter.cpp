#include "pch.h"
#include "IDCardAdapter.h"
#include <process.h>

// --- LINK THƯ VIỆN CỦA HÃNG ---s
// #pragma comment(lib, "VendorSDK/ZKReader.lib")

IDCardAdapter::IDCardAdapter() {
    m_bIsScanning = false;
    m_hDeviceHandle = 0;
}

IDCardAdapter::~IDCardAdapter() {
    Release();
}

bool IDCardAdapter::Initialize() {

    bool connectSuccess = true;

    if (connectSuccess) {
        if (m_pListener) m_pListener->OnDeviceConnected();
        return true;
    }
    return false;
}

void IDCardAdapter::StartScanning() {
    if (m_bIsScanning) return;
    m_bIsScanning = true;
    _beginthreadex(NULL, 0, &IDCardAdapter::ScanThread, this, 0, NULL);
}

void IDCardAdapter::StopScanning() {
    m_bIsScanning = false;
}

void IDCardAdapter::Release() {
    StopScanning();

    //ZK_Disconnect(m_hDeviceHandle);

    if (m_pListener) m_pListener->OnDeviceDisconnected();
}

unsigned int __stdcall IDCardAdapter::ScanThread(void* pParam)
{
    IDCardAdapter* pThis = (IDCardAdapter*)pParam;

    while (pThis->m_bIsScanning)
    {
        Sleep(4000);
        // bool hasCard = ZK_HasCard();
        bool hasCard = true;

        if (hasCard)
        {
            CitizenCardData outData;
            outData.strIDNumber = L"045088009999";
            outData.strCMND = L"987654321";
            outData.strFullName = L"Nguyễn Văn Device";
            outData.strDateOfBirth = L"05/05/1988";
            outData.strGender = L"Nam";
            outData.strPlaceOfResidence = L"789 Lê Duẩn, TP. Đà Nẵng";
            outData.strDateOfIssue = L"05/05/2018";

            if (pThis->m_pListener) {
                pThis->m_pListener->OnScanSuccess(outData);
            }
            pThis->m_bIsScanning = false;
            break;
        }
        Sleep(100);
    }
    return 0;
}