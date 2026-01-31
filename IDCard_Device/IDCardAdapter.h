#pragma once
// Include Interface chung (Đảm bảo đường dẫn đúng)
#include "InterfaceAdapterDevice.h"
#include <vector>

// --- [BƯỚC 1]: INCLUDE SDK CỦA HÃNG Ở ĐÂY ---
// Ví dụ: #include "VendorSDK/ZKReader.h" 

class IDCardAdapter : public IDeviceAdapter
{
private:
    bool m_bIsScanning;
    // Biến giữ kết nối thiết bị (Handle)
    long m_hDeviceHandle;

public:
    IDCardAdapter();
    virtual ~IDCardAdapter();

    // Override 4 hàm bắt buộc của Interface
    bool Initialize() override;
    void StartScanning() override;
    void StopScanning() override;
    void Release() override;

    // Luồng đọc thẻ chạy ngầm (Background Thread)
    static unsigned int __stdcall ScanThread(void* pParam);

    // Hàm phụ trợ: Chuyển dữ liệu thô từ SDK sang struct của Kiosk
    void ParseDataFromSDK(void* rawData, CitizenCardData& outData);
};