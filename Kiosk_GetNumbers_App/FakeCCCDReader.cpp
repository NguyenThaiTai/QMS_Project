#include "pch.h"
#include "FakeCCCDReader.h"
#include <cstdlib>
#include <ctime>

bool FakeCCCDReader::ScanCard_Simulation(CitizenCardData& outData)
{
    Sleep(3000);

    srand((unsigned int)time(NULL));
    int nCase = rand() % 10;

    outData = CitizenCardData(); // Reset

    if (nCase == 0) {
        outData.strIDNumber = L"083203001509";
        outData.strCMND = L"321614411";
        outData.strFullName = L"Bé Hồng";
        outData.strDateOfBirth = L"08/11/2002";
        outData.strGender = L"Nam";
        outData.strPlaceOfResidence = L"20, Lê Lợi, An Hội, TP. Bến Tre, Bến Tre";
        outData.strDateOfIssue = L"19/02/2022";
    }
    else if (nCase == 1) {
        outData.strIDNumber = L"001199005678";
        outData.strCMND = L"012345678";
        outData.strFullName = L"Bé Nhung";
        outData.strDateOfBirth = L"20/10/1999";
        outData.strGender = L"Nữ";
        outData.strPlaceOfResidence = L"456 Kim Mã, Ba Đình, Hà Nội";
        outData.strDateOfIssue = L"20/10/2019";
    }

    else if (nCase == 2) {
        outData.strIDNumber = L"001199005678";
        outData.strCMND = L"012345678";
        outData.strFullName = L"Ngọc Hiền";
        outData.strDateOfBirth = L"20/10/1999";
        outData.strGender = L"Nữ";
        outData.strPlaceOfResidence = L"456 Kim Mã, Ba Đình, Hà Nội";
        outData.strDateOfIssue = L"20/10/2019";
    }

    else if (nCase == 3) {
        outData.strIDNumber = L"001199005678";
        outData.strCMND = L"012345678";
        outData.strFullName = L"Nguyễn Xuân Thống";
        outData.strDateOfBirth = L"20/10/1999";
        outData.strGender = L"Nữ";
        outData.strPlaceOfResidence = L"456 Kim Mã, Ba Đình, Hà Nội";
        outData.strDateOfIssue = L"20/10/2019";
    }

    else if (nCase == 4) {
        outData.strIDNumber = L"001199005678";
        outData.strCMND = L"012345678";
        outData.strFullName = L"Bé Đào";
        outData.strDateOfBirth = L"20/10/1999";
        outData.strGender = L"Nữ";
        outData.strPlaceOfResidence = L"456 Kim Mã, Ba Đình, Hà Nội";
        outData.strDateOfIssue = L"20/10/2019";
    }

    else {
        outData.strIDNumber = L"045088009999";
        outData.strCMND = L"987654321";
        outData.strFullName = L"Nguyễn Ngọc Khánh";
        outData.strDateOfBirth = L"05/05/1988";
        outData.strGender = L"Nam";
        outData.strPlaceOfResidence = L"789 Lê Duẩn, TP. Đà Nẵng";
        outData.strDateOfIssue = L"05/05/2018";
    }

    return true;
}