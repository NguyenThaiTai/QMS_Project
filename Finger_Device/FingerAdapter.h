#pragma once
#include "InterfaceAdapterDevice.h" 

class FingerAdapter : public IDeviceAdapter
{
private:
    bool m_bIsScanning;
    void* m_hThread;

public:
    FingerAdapter();
    ~FingerAdapter();

    bool Initialize() override;
    void StartScanning() override;
    void StopScanning() override;
    void Release() override;

    static unsigned int __stdcall FingerThreadProc(void* pParam);
};