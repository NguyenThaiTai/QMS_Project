
// Kiosk_GetNumbers_App.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CKioskGetNumbersAppApp:
// See Kiosk_GetNumbers_App.cpp for the implementation of this class
//

class CKioskGetNumbersAppApp : public CWinApp
{
public:
	CKioskGetNumbersAppApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CKioskGetNumbersAppApp theApp;
