#include "pch.h"
#include "IDCardAdapter.h"

extern "C"
{
    __declspec(dllexport) IDeviceAdapter* CreateDeviceInstance()
    {
        return new IDCardAdapter();
    }
}