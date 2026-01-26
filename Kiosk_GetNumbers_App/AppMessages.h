#pragma once

// add start define common application messages NTTai 20260123

// Message IDs for NoAuthDlg (Service Selection Screen)
#define WM_API_TICKET_RESULT (WM_USER + 201) // Result of ticket issuance
#define WM_API_LIST_RESULT   (WM_USER + 202) // Result of fetching service list

// Message IDs for AuthQRCodeDlg (QR Code Screen)
#define WM_API_QR_RESULT     (WM_USER + 301) // Result of fetching new QR link

// add end define common application messages NTTai 20260123