#pragma once

#include "InterfaceAdapterDevice.h" // Trỏ đúng đường dẫn về file Interface chung

class FaceIDAdapter : public IDeviceAdapter
{
private:
    bool m_bIsScanning;

public:
    FaceIDAdapter();
    ~FaceIDAdapter();

    bool Initialize() override;
    void StartScanning() override;
    void StopScanning() override;
    void Release() override;

    static unsigned int __stdcall FaceIDThreadProc(void* pParam);
};