#include "pch.h"
#include "FingerAdapter.h"

extern "C"
{
    __declspec(dllexport) IDeviceAdapter* CreateDeviceInstance()
    {
        return new FingerAdapter();
    }
}