#include "pch.h"
#include "FaceIDAdapter.h"

extern "C"
{
    __declspec(dllexport) IDeviceAdapter* CreateDeviceInstance()
    {
        return new FaceIDAdapter();
    }
}