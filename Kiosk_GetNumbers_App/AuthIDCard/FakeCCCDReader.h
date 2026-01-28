#pragma once
#include <atlstr.h>
#include <vector>
#include "../Common/Common.h"

class FakeCCCDReader
{
public:
    static bool ScanCard_Simulation(CitizenCardData& outData);
};