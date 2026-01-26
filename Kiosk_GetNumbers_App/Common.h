// File: Common.h
#pragma once
#include <atlstr.h>
#include <vector>

struct CitizenCardData {
    CString strIDNumber;
    CString strCMND;
    CString strFullName;
    CString strDateOfBirth;
    CString strGender;
    CString strPlaceOfResidence;
    CString strDateOfIssue;
};

struct ServiceData {
    int id;
    CString title;
    CString description;

    bool operator==(const ServiceData& other) const {
        return (id == other.id) && (title == other.title);
    }
    bool operator!=(const ServiceData& other) const {
        return !(*this == other);
    }
};
