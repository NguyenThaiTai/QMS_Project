#pragma once
#include "InterfaceAdapterDevice.h"
#include <vector>

// --- INCLUDE SDK ---
// Ví dụ: #include "VendorSDK/ZKReader.h" 

class IDCardAdapter : public IDeviceAdapter
{
private:
    bool m_bIsScanning;
    long m_hDeviceHandle;

public:
    IDCardAdapter();
    virtual ~IDCardAdapter();

    bool Initialize() override;
    void StartScanning() override;
    void StopScanning() override;
    void Release() override;

    static unsigned int __stdcall ScanThread(void* pParam);

    void ParseDataFromSDK(void* rawData, CitizenCardData& outData);
};