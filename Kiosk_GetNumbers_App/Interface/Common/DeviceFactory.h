// File: DeviceFactory.h
#pragma once

class DeviceFactory {
public:
    static IDeviceAdapter* CreateAdapterFromDLL(CString strDllName) {

        HMODULE hDll = LoadLibrary(strDllName);
        if (!hDll) {
            return nullptr;
        }

        CreateAdapterFunc createFunc = (CreateAdapterFunc)GetProcAddress(hDll, "CreateDeviceInstance");

        if (!createFunc) {
            FreeLibrary(hDll);
            return nullptr;
        }

        return createFunc();
    }
};