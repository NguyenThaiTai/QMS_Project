// File: IDeviceAdapter.h
#pragma once
#include "../../Common/Common.h"

class IDeviceListener {
public:
    virtual void OnDeviceConnected() = 0;
    virtual void OnDeviceDisconnected() = 0;
    virtual void OnScanSuccess(const CitizenCardData& data) = 0;
    virtual void OnScanError(CString strError) = 0;
};

class IDeviceAdapter
{
protected:
    IDeviceListener* m_pListener;

public:
    IDeviceAdapter() : m_pListener(nullptr) {}
    virtual ~IDeviceAdapter() {}

    void RegisterListener(IDeviceListener* pListener) { m_pListener = pListener; }

    virtual bool Initialize() = 0;
    virtual void StartScanning() = 0;
    virtual void StopScanning() = 0;
    virtual void Release() = 0;
};

typedef IDeviceAdapter* (*CreateAdapterFunc)();